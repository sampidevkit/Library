#include "libcomp.h"
#include "AtCmd.h"

#ifdef USE_ATCMD_DEBUG
#include "debug.h"
#else
#undef __dbc
#undef __dbs
#undef __dbi
#undef __dbh
#undef __dbh2
#undef __dbsi
#undef __dbsh
#undef __dbsc
#undef __dbss

#define __dbc(c)
#define __dbs(str)
#define __dbi(x)
#define __dbh(x)
#define __dbh2(x)
#define __dbsi(str, x)
#define __dbsh(str, x)
#define __dbsc(str, c)
#define __dbss(str1, str2)
#endif

// Response constants
const char RES_OK[]="\r\nOK\r\n";
uint8_t AtCmdData[ATCMD_BUFFER_SIZE];
buff_t AtCmdRxBuff;

static int RxCount=0;
static uint8_t ReTry=0;
static uint8_t TestCount=0;
static uint8_t DoNext=0;
static tick_t Tdelay=250;
static tick_timer_t TickRaw={1, 0, 0};

void ATCMD_Init(void)
{
    ReTry=0;
    DoNext=0;
    RxCount=0;
    TestCount=0;
    Tdelay=250;
    AtCmdRxBuff.Len=0;
    AtCmdRxBuff.Size=ATCMD_BUFFER_SIZE;
    AtCmdRxBuff.pData=AtCmdData;
    Tick_Timer_Reset(TickRaw);
}

void ATCMD_SendRaw(const uint8_t *pD, int sz)
{
    // Get all response data in Rx buffer before send new data
    while(ATCMD_Port_IsRxReady())
        ATCMD_Port_Read();

    while(sz>0)
    {
        if(ATCMD_Port_IsTxReady())
        {
            ATCMD_Port_Write(*pD++);
            sz--;
        }
    }

    while(ATCMD_Port_IsTxDone());
    Tick_Timer_Reset(TickRaw);
    RxCount=0;
}

int8_t ATCMD_GetRaw(uint8_t *pD, int *pSz, uint16_t firstWait, uint16_t lastWait)
{
    while(ATCMD_Port_IsRxReady())
    {
        uint8_t c=ATCMD_Port_Read();

        Tick_Timer_Reset(TickRaw);
        __dbc(c);
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
}

int8_t ATCMD_SendGetDat(const char *pTx, char *pRx, uint16_t firstWait, uint16_t lastWait)
{
    int8_t rslt=RESULT_BUSY;

    switch(DoNext)
    {
        default:
        case 2:
            if(Tick_Timer_Is_Over_Ms(TickRaw, Tdelay))
                DoNext=0;
            break;

        case 0:
            DoNext++;
            __dbss("\nTX: ", pTx);
            __dbs("\nRX: ");
            ATCMD_SendRaw((uint8_t*) pTx, slen(pTx));
            break;

        case 1:
            rslt=ATCMD_GetRaw((uint8_t*) pRx, NULL, firstWait, lastWait);

            if(rslt!=RESULT_BUSY)
                DoNext=0;
            break;
    }

    return rslt;
}

int8_t ATCMD_SendGetAck(const char *pTx, const char *pAck, uint16_t firstWait, uint16_t lastWait, uint8_t tryCount)
{
    switch(DoNext)
    {
        default:
        case 2:
            if(Tick_Timer_Is_Over_Ms(TickRaw, Tdelay))
                DoNext=0;
            break;

        case 0:
            DoNext++;
            AtCmdRxBuff.Len=0;
            __dbss("\nTX: ", pTx);
            __dbs("\nRX: ");
            ATCMD_SendRaw((uint8_t*) pTx, slen(pTx));
            break;

        case 1:
            while(ATCMD_Port_IsRxReady())
            {
                uint8_t c=ATCMD_Port_Read();

                __dbc(c);

                if(c!=0x00)
                    Tick_Timer_Reset(TickRaw);

                AtCmdRxBuff.pData[AtCmdRxBuff.Len++]=c;
                AtCmdRxBuff.pData[AtCmdRxBuff.Len]=0;

                if((AtCmdRxBuff.Len+1)==AtCmdRxBuff.Size)
                    AtCmdRxBuff.Len=0;

                if(FindString(c, &RxCount, (uint8_t *) pAck))
                {
                    DoNext=0;
                    ReTry=0;
                    return RESULT_DONE;
                }
            }

            if(RxCount==0)
            {
                if(Tick_Timer_Is_Over_Ms(TickRaw, firstWait))
                {
                    if(++ReTry>=tryCount)
                    {
                        ReTry=0;
                        DoNext=0;
                        __dbs("RX timeout");
                        return RESULT_ERR;
                    }
                    else
                        DoNext=2;
                }
            }
            else if(Tick_Timer_Is_Over_Ms(TickRaw, lastWait))
            {
                if(++ReTry>=tryCount)
                {
                    ReTry=0;
                    DoNext=0;
                    __dbs("Not found");
                    return RESULT_ERR;
                }
                else
                    DoNext=2;
            }
            break;
    }

    return RESULT_BUSY;
}

int8_t ATCMD_Test(uint8_t tryCount)
{
    int8_t rslt;
    uint8_t type=tryCount>>6;

    tryCount&=0x3F;
    rslt=ATCMD_SendGetAck("AT\r", RES_OK, 250, 250, 1);

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
}

void ATCMD_Delay(uint16_t delayMs)
{
    DoNext=2;
    Tdelay=delayMs;
    Tick_Timer_Reset(TickRaw);
}