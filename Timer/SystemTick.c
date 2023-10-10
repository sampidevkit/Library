#include "SystemTick.h"
#include "libcomp.h"

#if(SYS_TMR_BIT_LEN==16) && defined(USE_SYS_TMR_INTERRUPT)
volatile uint16_t SysTmrHigh;

static void __SysTmrOvfCallback(void) // <editor-fold defaultstate="collapsed" desc="System timer over-flow callback">
{
    SysTmrHigh++;
} // </editor-fold>

tick_t Tick_Get(void) // <editor-fold defaultstate="collapsed" desc="Get system tick value">
{
    tick_t tk=SysTmrHigh;
    
    tk<<=16;
    tk|=SysTmr_Read();
    
    return tk;
} // </editor-fold>

void SystemTick_Init(void) // <editor-fold defaultstate="collapsed" desc="System tick initialize">
{
    SysTmrHigh=0;
    SysTmr_OvfCallback(__SysTmrOvfCallback);
} // </editor-fold>
#endif

bool Tick_Is_Over(tick_timer_t *pTick, tick_t t) // <editor-fold defaultstate="collapsed" desc="Check timeout">
{
    if(pTick->Over==1)
    {
        pTick->Begin=Tick_Get();

#if(TICK_PER_US>0)
        pTick->TickCount=t*TICK_PER_US;
#else
        pTick->TickCount=t*TICK_PER_MS;
#endif
        pTick->Over=0;
    }

    if((Tick_Get()-pTick->Begin)>=pTick->TickCount)
        pTick->Over=1;

    return pTick->Over;
} // </editor-fold>

void __delay_sec(tick_t sec) // <editor-fold defaultstate="collapsed" desc="delay sec">
{
    tick_timer_t Tick;

    Tick.Begin=Tick_Get();
    Tick.TickCount=sec*TICK_PER_SEC;

    while((Tick_Get()-Tick.Begin)<Tick.TickCount);
} // </editor-fold>

void __delay_ms(tick_t ms) // <editor-fold defaultstate="collapsed" desc="delay ms">
{
    tick_timer_t Tick;

    Tick.Begin=Tick_Get();
    Tick.TickCount=ms*TICK_PER_MS;

    while((Tick_Get()-Tick.Begin)<Tick.TickCount);
} // </editor-fold>

#if(TICK_PER_US>0)

void __delay_us(tick_t us) // <editor-fold defaultstate="collapsed" desc="delay us">
{
    tick_timer_t Tick;

    Tick.Begin=Tick_Get();
    Tick.TickCount=us*TICK_PER_US;

    while((Tick_Get()-Tick.Begin)<Tick.TickCount);
} // </editor-fold>
#endif