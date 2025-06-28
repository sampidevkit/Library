#include "at_cmd.h"
#include "misc/util.h"
#include "system/system_tick.h"

#ifndef USE_ATCMD_DEBUG
#define DEBUG_H
#endif

#include "Common/Debug.h"

/* ********************************************************** Local variables */
static const char CONST_RES_OK[]="\r\nOK\r\n";
static const char CONST_RES_ERROR[]="\r\nERROR\r\n";
static const char CONST_RES_READY[]="\r\nREADY\r\n";

static size_t AckCount=0;
static size_t NAckCount=0;
static size_t RxCount=0;
static uint8_t ReTry=0;
static uint8_t TestCount=0;
static uint8_t DoNext=0;
static tick_t Tdelay=500;
static int8_t lastRslt=RESULT_DONE;
static tick_timer_t TickRaw={1, 0, 0};
static buff_t *pAtCmdRxBuff;

void ATCMD_Init(void) // <editor-fold defaultstate="collapsed" desc="initialize">
{
    pAtCmdRxBuff=ATCMD_Port_Init();
    ReTry=0;
    DoNext=0;
    RxCount=0;
    TestCount=0;
    AckCount=0;
    NAckCount=0;
    Tdelay=500;
    pAtCmdRxBuff->Len=0;
    lastRslt=RESULT_DONE;
    Tick_Timer_Reset(TickRaw);
} // </editor-fold>

void ATCMD_Deinit(void) // <editor-fold defaultstate="collapsed" desc="deinitialize">
{
    ATCMD_Port_Deinit();
} // </editor-fold>

const char *ATCMD_GetResConst(res_const_t resCode) // <editor-fold defaultstate="collapsed" desc="Get response const">
{
    switch(resCode)
    {
        case RES_OK:
            return CONST_RES_OK;

        case RES_ERROR:
            return CONST_RES_ERROR;

        case RES_READY:
            return CONST_RES_READY;

        default:
            break;
    }

    return NULL;
} // </editor-fold>

int8_t ATCMD_SendRaw(const uint8_t *pD, int sz, uint16_t Wait) // <editor-fold defaultstate="collapsed" desc="send raw data">
{
    // Get all response data in Rx buffer before send new data
    __dbs("\n\nClear Rx: ");

    while(ATCMD_Port_IsRxReady())
    {
        uint8_t c=ATCMD_Port_Read();

        if(is_printable(c))
            __dbc(c);
        else
        {
            __dbc('<');
            __dbh2(c);
            __dbc('>');
        }

        if(Tick_Timer_Is_Over_Ms(TickRaw, Wait))
        {
            __dbs("\nTX timeout");
            return RESULT_ERR;
        }
    }

    __dbs(" Done");
    __tsdbs("TX: ");
    Tick_Timer_Reset(TickRaw);
    RxCount=0;

    while(sz>0)
    {
        if(ATCMD_Port_IsTxReady())
        {
            if(is_printable(*pD))
                __dbc(*pD);
            else
            {
                __dbc('<');
                __dbh2(*pD);
                __dbc('>');
            }

            ATCMD_Port_Write(*pD++);
            sz--;
        }
    }

    while(!ATCMD_Port_IsTxDone());

    return RESULT_DONE;
} // </editor-fold>

int8_t ATCMD_GetRaw(uint8_t *pD, int *pSz, uint16_t firstWait, uint16_t lastWait) // <editor-fold defaultstate="collapsed" desc="get raw data">
{
    while(ATCMD_Port_IsRxReady())
    {
        uint8_t c=ATCMD_Port_Read();

        Tick_Timer_Reset(TickRaw);

        if(is_printable(c))
            __dbc(c);
        else
        {
            __dbc('<');
            __dbh2(c);
            __dbc('>');
        }

        if(RxCount>=pAtCmdRxBuff->Size)
            RxCount=0;

        pD[RxCount++]=c;
    }

    if(RxCount==0)
    {
        if(Tick_Timer_Is_Over_Ms(TickRaw, firstWait))
        {
            if(pSz!=NULL)
                *pSz=RxCount;

            pD[RxCount]=0x00;
            RxCount=0;

            __dbs("\nNo response");
            return RESULT_ERR;
        }
    }
    else if(Tick_Timer_Is_Over_Ms(TickRaw, lastWait))
    {
        if(pSz!=NULL)
            *pSz=RxCount;

        pD[RxCount]=0x00;
        RxCount=0;

        return RESULT_DONE;
    }

    return RESULT_BUSY;
} // </editor-fold>

int8_t ATCMD_SendGetDat(const char *pTx, char *pRx, uint16_t firstWait, uint16_t lastWait) // <editor-fold defaultstate="collapsed" desc="send cmd then get response">
{
    int8_t rslt=RESULT_BUSY;

    switch(DoNext)
    {
        default:
        case 3:
            if(Tick_Timer_Is_Over_Ms(TickRaw, Tdelay))
                DoNext=0;
            break;

        case 0:
            DoNext++;
            Tick_Timer_Reset(TickRaw);

        case 1:
            rslt=ATCMD_SendRaw((const uint8_t*) pTx, slen(pTx), 1000);

            if(rslt==RESULT_DONE)
            {
                DoNext++;
                rslt=RESULT_BUSY;
                __tsdbs("RX: ");
            }
            else if(rslt==RESULT_ERR)
                DoNext=0;
            break;

        case 2:
            rslt=ATCMD_GetRaw((uint8_t*) pRx, NULL, firstWait, lastWait);

            if(rslt!=RESULT_BUSY)
                DoNext=0;
            break;
    }

    return rslt;
} // </editor-fold>

int8_t ATCMD_SendGetAck(const char *pTx, const char *pAck, const char *pNAck,
                        uint16_t firstWait, uint16_t lastWait, uint8_t tryCount) // <editor-fold defaultstate="collapsed" desc="send cmd then get ack">
{
    int8_t rslt=RESULT_BUSY;

    switch(DoNext)
    {
        default:
        case 3:
            if(Tick_Timer_Is_Over_Ms(TickRaw, Tdelay))
                DoNext=0;
            break;

        case 0:
            DoNext++;
            AckCount=0;
            NAckCount=0;
            pAtCmdRxBuff->Len=0;
            lastRslt=RESULT_ERR;
            Tick_Timer_Reset(TickRaw);

        case 1:
            rslt=ATCMD_SendRaw((const uint8_t*) pTx, (int) slen(pTx), 1000);

            if(rslt==RESULT_DONE)
            {
                DoNext++;
                rslt=RESULT_BUSY;
                __tsdbs("RX: ");
            }
            else if(rslt==RESULT_ERR)
                DoNext=0;
            break;

        case 2:
            while(ATCMD_Port_IsRxReady())
            {
                char c=ATCMD_Port_Read();

                if(is_printable(c))
                    __dbc(c);
                else
                {
                    __dbc('<');
                    __dbh2(c);
                    __dbc('>');
                }

                pAtCmdRxBuff->pData[pAtCmdRxBuff->Len++]=c;
                pAtCmdRxBuff->pData[pAtCmdRxBuff->Len]=0;

                if((pAtCmdRxBuff->Len+1)==pAtCmdRxBuff->Size)
                    pAtCmdRxBuff->Len=0;

                if(FindString(c, &AckCount, (const char *) pAck))
                    lastRslt=RESULT_ACK;

                if(lastRslt!=RESULT_ACK)
                {
                    if(FindString(c, &NAckCount, (const char *) pNAck))
                        lastRslt=RESULT_NACK;
                }

                if(c!=0x00)
                    Tick_Timer_Reset(TickRaw);
                else
                    break;
            }

            if(pAtCmdRxBuff->Len==0)
            {
                if(Tick_Timer_Is_Over_Ms(TickRaw, firstWait))
                {
                    if(++ReTry>=tryCount)
                    {
                        ReTry=0;
                        DoNext=0;
                        __dbsu("\nRX timeout: ", firstWait);
                        return RESULT_ERR;
                    }
                    else
                        DoNext=3;
                }
            }
            else if(Tick_Timer_Is_Over_Ms(TickRaw, lastWait))
            {
                if(lastRslt==RESULT_ERR)
                {
                    if(++ReTry>=tryCount)
                    {
                        ReTry=0;
                        DoNext=0;
                        __dbs("\nNot found");
                        return RESULT_ERR;
                    }
                    else
                        DoNext=3;
                }
                else
                {
                    ReTry=0;
                    DoNext=0;

                    if(lastRslt==RESULT_ACK)
                        __dbs("\nFound Ack");
                    else
                        __dbs("\nFound Nack");

                    __dbsu(", t=", lastWait);

                    return lastRslt;
                }
            }
            break;
    }

    return RESULT_BUSY;
} // </editor-fold>

int8_t ATCMD_GetAck(const char *pAck, const char *pNAck,
                    uint16_t firstWait, uint16_t lastWait) // <editor-fold defaultstate="collapsed" desc="get ack only">
{
    int8_t rslt=RESULT_BUSY;

    switch(DoNext)
    {
        default:
        case 3:
        case 0:
            DoNext++;
            AckCount=0;
            NAckCount=0;
            pAtCmdRxBuff->Len=0;
            Tick_Timer_Reset(TickRaw);

        case 1:
            DoNext++;

        case 2:
            while(ATCMD_Port_IsRxReady())
            {
                char c=ATCMD_Port_Read();

                if(is_printable(c))
                    __dbc(c);
                else
                {
                    __dbc('<');
                    __dbh2(c);
                    __dbc('>');
                }

                pAtCmdRxBuff->pData[pAtCmdRxBuff->Len++]=c;
                pAtCmdRxBuff->pData[pAtCmdRxBuff->Len]=0;

                if((pAtCmdRxBuff->Len+1)==pAtCmdRxBuff->Size)
                    pAtCmdRxBuff->Len=0;

                if(FindString(c, &AckCount, (const char *) pAck))
                    lastRslt=RESULT_ACK;

                if(lastRslt!=RESULT_ACK)
                {
                    if(FindString(c, &NAckCount, (const char *) pNAck))
                        lastRslt=RESULT_NACK;
                }

                if(c!=0x00)
                    Tick_Timer_Reset(TickRaw);
                else
                    break;
            }

            if(pAtCmdRxBuff->Len==0)
            {
                if(Tick_Timer_Is_Over_Ms(TickRaw, firstWait))
                {
                    ReTry=0;
                    DoNext=0;
                    __dbs("\nRX timeout");
                    return RESULT_ERR;
                }
            }
            else if(Tick_Timer_Is_Over_Ms(TickRaw, lastWait))
            {
                if(lastRslt==RESULT_ERR)
                {
                    ReTry=0;
                    DoNext=0;
                    __dbs("\nNot found");
                    return RESULT_ERR;
                }
                else
                {
                    ReTry=0;
                    DoNext=0;

                    if(lastRslt==RESULT_ACK)
                        __dbs("\nFound Ack");
                    else
                        __dbs("\nFound Nack");

                    return lastRslt;
                }
            }
            break;
    }

    return rslt;
} // </editor-fold>

int8_t __ATCMD_Test(uint8_t tryCount) // <editor-fold defaultstate="collapsed" desc="test at cmd">
{
    int8_t rslt;
    uint8_t type=tryCount&0xC0;

    tryCount&=0x3F;
    rslt=ATCMD_SendGetAck("ATE0\r", CONST_RES_OK, NULL, 500, 10, 1);

    switch(type)
    {
        default:
        case ALL_STATE_ON:
            if(rslt==RESULT_DONE)
            {
                if(++TestCount>=tryCount)
                {
                    TestCount=0;
                    return RESULT_DONE;
                }
            }
            else if(rslt==RESULT_ERR)
            {
                TestCount=0;
                return RESULT_ERR;
            }
            break;

        case ALL_STATE_OFF:
            if(rslt==RESULT_DONE)
            {
                TestCount=0;
                return RESULT_ERR;
            }
            else if(rslt==RESULT_ERR)
            {
                if(++TestCount>=tryCount)
                {
                    TestCount=0;
                    return RESULT_DONE;
                }
            }
            break;

        case AT_LEAST_1ON:
            if(rslt==RESULT_DONE)
            {
                TestCount=0;
                return RESULT_DONE;
            }
            else if(rslt==RESULT_ERR)
            {
                if(++TestCount>=tryCount)
                {
                    TestCount=0;
                    return RESULT_ERR;
                }
            }
            break;

        case AT_LEAST_1OFF:
            if(rslt==RESULT_DONE)
            {
                if(++TestCount>=tryCount)
                {
                    TestCount=0;
                    return RESULT_ERR;
                }
            }
            else if(rslt==RESULT_ERR)
            {
                TestCount=0;
                return RESULT_DONE;
            }
            break;
    }

    return RESULT_BUSY;
} // </editor-fold>

void ATCMD_Delay(uint16_t delayMs) // <editor-fold defaultstate="collapsed" desc="set delay before cmd sent">
{
    DoNext=3;
    Tdelay=delayMs;
    Tick_Timer_Reset(TickRaw);
} // </editor-fold>

uint8_t *ATCMD_GetRxBuffer(uint16_t idx)
{
    return &pAtCmdRxBuff->pData[idx];
}

size_t ATCMD_GetRxSize(void)
{
    return pAtCmdRxBuff->Size;
}

size_t ATCMD_GetRxLen(void)  
{
    return pAtCmdRxBuff->Len;
}

/* ********************************************************************* APIs */

int8_t ATCMD_EchoOff(uint8_t reTry)
{
    if(reTry>63)
        reTry=63;

    return __ATCMD_Test(reTry|AT_LEAST_1ON);
}