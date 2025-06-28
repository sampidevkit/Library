#include "system_tick.h"

static simple_fnc_t CbFnc=NULL;
static const tick_para_t *pTickPara;

public bool Tick_Timer_Is_Over(tick_timer_t *pTick, tick_t Time, tick_timer_type_t TickType) // <editor-fold defaultstate="collapsed" desc="Check tick over microsecond">
{
    if(pTick->Timeout==2)
    {
        pTick->Start=Tick_Timer_Get_TickVal();

#if defined(__XC32__) || defined(USE_SYSTEM_TICK_US)
        if(TickType==US)
            pTick->Duration=pTickPara->TICK_PER_US*Time;
        else
#endif
        {
            if(TickType==MS)
                pTick->Duration=pTickPara->TICK_PER_MS*Time;
            else
                pTick->Duration=pTickPara->TICK_PER_SEC*Time;
        }

        pTick->Timeout=0;
    }
    else
    {
        tick_t Now=Tick_Get();
        tick_t Dt=(tick_t) (Now-pTick->Start);

        if(Dt>=pTick->Duration)
        {
            pTick->Timeout++;

            if(pTick->Timeout>=2) // Double check
                return 1;
        }
    }

    return 0;
} // </editor-fold>

public void Delay(tick_t Time, tick_timer_type_t TickType) // <editor-fold defaultstate="collapsed" desc="Delay">
{
    tick_t Duration;
    tick_t Start=Tick_Timer_Get_TickVal();

#if defined(__XC32__) || defined(USE_SYSTEM_TICK_US)
    if(TickType==US)
        Duration=pTickPara->TICK_PER_US*Time;
    else
#endif
    {
        if(TickType==MS)

            Duration=pTickPara->TICK_PER_MS*Time;
        else
            Duration=pTickPara->TICK_PER_SEC*Time;
    }

    while((Tick_Timer_Get_TickVal()-Start)<Duration)
    {
        if(CbFnc)
            CbFnc();
    }
} // </editor-fold>

public void Tick_Timer_SetFncCallInDelay(simple_fnc_t pFnc) // <editor-fold defaultstate="collapsed" desc="Set callback function">
{
    CbFnc=pFnc;
} // </editor-fold>

public void Tick_Timer_Init(simple_fnc_t pFnc) // <editor-fold defaultstate="collapsed" desc="Tick timer init">
{
    pTickPara=Tick_Timer_Hal_Init();
    Tick_Timer_SetFncCallInDelay(pFnc);
} // </editor-fold>

inline void Tick_Reset(tick_timer_t *pTick)
{
    pTick->Timeout=2;
}

inline tick_t Tick_Per_Sec(void)
{
    return pTickPara->TICK_PER_SEC;
}

inline tick_t Tick_Per_Ms(void)
{
    return pTickPara->TICK_PER_MS;
}

#ifdef TICK_32BIT
inline tick_t Tick_Per_Us(void)
{
    return pTickPara->TICK_PER_US;
}
#endif

inline tick_t Tick_GetTimeMs(void)
{
    return (Tick_Timer_Get_TickVal()/pTickPara->TICK_PER_MS);
}

inline tick_t Tick_GetTimeSec(void)
{
    return (Tick_Timer_Get_TickVal()/pTickPara->TICK_PER_SEC);
}

tick_t Tick_Dif(tick_t Tk0, tick_t Tk1, tick_timer_type_t TickType)
{
    if(TickType==SEC)
        return (Tk1-Tk0)/pTickPara->TICK_PER_SEC;
    else if(TickType==MS)
        return (Tk1-Tk0)/pTickPara->TICK_PER_MS;

#ifdef TICK_32BIT
    return (Tk1-Tk0)/pTickPara->TICK_PER_US;
#else
    return 0;
#endif
}

#ifdef TICK_32BIT

inline tick_t Tick_GetTimeUs(void)
{
    return (Tick_Timer_Get_TickVal()/pTickPara->TICK_PER_US);
}
#endif