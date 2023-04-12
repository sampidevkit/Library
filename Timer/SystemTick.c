#include "SystemTick.h"
#include "libcomp.h"

bool Tick_Is_Over(tick_timer_t *pTick, tick_t us) // <editor-fold defaultstate="collapsed" desc="Check timeout">
{
    if(pTick->Over==1)
    {
        pTick->Begin=Tick_Get();
        pTick->TickCount=us*TICK_PER_US;
        pTick->Over=0;
    }

    if((Tick_Get()-pTick->Begin)>=pTick->TickCount)
        pTick->Over=1;

    return pTick->Over;
} // </editor-fold>

void __delay_ms(tick_t ms) // <editor-fold defaultstate="collapsed" desc="delay ms">
{
    tick_timer_t Tick;

    Tick.Begin=Tick_Get();
    Tick.TickCount=ms*TICK_PER_MS;

    while((Tick_Get()-Tick.Begin)<Tick.TickCount);
} // </editor-fold>

void __delay_us(tick_t us) // <editor-fold defaultstate="collapsed" desc="delay us">
{
    tick_timer_t Tick;

    Tick.Begin=Tick_Get();
    Tick.TickCount=us*TICK_PER_US;

    while((Tick_Get()-Tick.Begin)<Tick.TickCount);
} // </editor-fold>