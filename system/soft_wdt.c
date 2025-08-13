#include <xc.h>
#include "soft_wdt.h"
#include "system_tick.h"

private tick_timer_t Tick={1, 0, 0};
private simple_fnc_t CbFnc=NULL;

volatile uint32_t softWdtCount=0;
volatile uint32_t softWdtCountMax=NO_WDT;

public void SYS_SoftReset(void) // <editor-fold defaultstate="collapsed" desc="Software reset">
{
    uint32_t dummy=0;

#ifdef __PIC32MM
    SYSKEY=0x0; //write invalid key to force lock
    SYSKEY=0xAA996655; //write Key1 to SYSKEY
    SYSKEY=0x556699AA; //write Key2 to SYSKEY
    RSWRSTSET=1;
    /* read RSWRST register to trigger reset */
    dummy=RSWRST;
#elif defined(__AVR_ARCH__)
    _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRST_bm);
#endif
    while(1)
        dummy--;
} // </editor-fold>

public void softWDT_Isr(void)
{
    softWDT_HwWdtClear();

    if(softWdtCountMax==0xFFFFFF) // No soft WDT
        return;

    if(++softWdtCount==softWdtCountMax)
    {
        if(CbFnc)
            CbFnc();

        SYS_SoftReset();
    }
}

public new_simple_task_t(softWDT_Tasks) // <editor-fold defaultstate="collapsed" desc="WDT tasks">
{
    if(Tick_Timer_Is_Over_Ms(Tick, 500))
    {
        softWdtCount=0;
        softWDT_LedSetState(2); // toggle
    }

    Task_Done();
} // </editor-fold>

public void softWDT_Enable(void) // <editor-fold defaultstate="collapsed" desc="WDT enable">
{
    softWDT_LedSetState(1);
    Tick_Timer_Reset(Tick);
    TaskManager_Create_NewSimpleTask(softWDT_Tasks);
} // </editor-fold>

public void softWDT_Disable(void) // <editor-fold defaultstate="collapsed" desc="WDT disable">
{
    softWDT_LedSetState(0);
    TaskManager_End_Task(softWDT_Tasks);
} // </editor-fold>

public void softWDT_SetCbBeforeReset(simple_fnc_t fnc)
{
    CbFnc=fnc;
}

void softWDT_Init(uint32_t tout, simple_fnc_t cb)
{
    softWdtCountMax=tout;
    softWdtCount=0;
    softWDT_SetCbBeforeReset(cb);
    softWDT_Enable();
    softWDT_TmrSetInterruptHandler(softWDT_Isr);
}
