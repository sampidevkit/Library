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

static int RxCount=0;
static uint8_t ReTry=0;
static uint8_t DoNext=0;
static tick_t Tdelay=250;
static tick_timer_t TickRaw={1, 0, 0};
buff_t *pAtCmdRxBkBuff;

void ATCMD_Init(buff_t *pRxBuffer)
{
    ReTry=0;
    DoNext=0;
    RxCount=0;
    Tdelay=250;
    pAtCmdRxBkBuff=pRxBuffer;
    Tick_Timer_Reset(TickRaw);

    if(pAtCmdRxBkBuff!=NULL)
        pAtCmdRxBkBuff->Len=0;
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
            ATCMD_SendRaw((uint8_t*) pTx, strlen(pTx));
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
            ReTry=0;

            if(pAtCmdRxBkBuff!=NULL)
                pAtCmdRxBkBuff->Len=0;

            __dbss("\nTX: ", pTx);
            __dbs("\nRX: ");
            ATCMD_SendRaw((uint8_t*) pTx, strlen(pTx));
            break;

        case 1:
            while(ATCMD_Port_IsRxReady())
            {
                uint8_t c=ATCMD_Port_Read();

                Tick_Timer_Reset(TickRaw);
                __dbc(c);

                if(pAtCmdRxBkBuff->pData!=NULL)
                {
                    pAtCmdRxBkBuff->pData[pAtCmdRxBkBuff->Len++]=c;
                    pAtCmdRxBkBuff->pData[pAtCmdRxBkBuff->Len]=0;
                }

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
                    if(++ReTry>tryCount)
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
                if(++ReTry>tryCount)
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

void ATCMD_Delay(uint16_t delayMs)
{
    DoNext=2;
    Tdelay=delayMs;
    Tick_Timer_Reset(TickRaw);
}