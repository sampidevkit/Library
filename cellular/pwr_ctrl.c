#include "pwr_ctrl.h"
#include "system/system_tick.h"
#include "system/task_manager.h"

#ifndef USE_CELL_PWRCTRL_DEBUG
#define DEBUG_H
#endif

#include "common/debug.h"

typedef enum
{
    PWRCTRL_INIT=0, // 0
    PWRCTRL_MODULE_OFF, // 1
    PWRCTRL_DISCHR, // 2
    PWRCTRL_PWREN, // 3
    PWRCTRL_TRIG_ON, // 4
    PWRCTRL_CHECK_VAUX, // 5
    PWRCTRL_MODULE_RDY, // 6
    PWRCTRL_TRIG_OFF, // 7
    PWRCTRL_RELEASE_OFF, // 8
    PWRCTRL_PWRDISABLE // 9
} pwrctrl_task_t;

typedef struct
{
    uint8_t dischr;
    uint8_t vcell;
    uint8_t vaux;
    uint8_t reboot;
} pwr_try_t;

static struct
{
    uint8_t Retry;
    pwr_try_t Count;
    tick_timer_t Tick;
    pwrctrl_task_t Now;
    pwrctrl_task_t Next;
} PwrCtrlCxt;

static const cell_para_t *CellPara;
static cell_stt_t CellState;

public new_simple_task_t(Cell_PwrCtrl_Tasks) // <editor-fold defaultstate="collapsed" desc="Power control task">
{
    if(CellState.vcelRdy==0)
    {
        if(CELL_VCEL_Get()>=CellPara->CELL_VCEL_STARTUP)
            CellState.vcelRdy=1;
    }
    else if(CELL_VCEL_Get()<CellPara->CELL_VCEL_PWRDN)
        CellState.vcelRdy=0;

    if(CELL_VAUX_Get()>=CellPara->CELL_VAUX_MIN)
        CellState.vauxRdy=1;
    else
        CellState.vauxRdy=0;

    switch(PwrCtrlCxt.Now)
    {
        default:
        case PWRCTRL_MODULE_OFF: // Do nothing
            CellState.busy=0;
            __tsdbs_t("Close task: Cell_PwrCtrl_Tasks");
            TaskManager_End_Task(Cell_PwrCtrl_Tasks);
            break;

        case PWRCTRL_INIT:
            CELL_PWREN_SetState(0);
            CELL_ONOFF_SetState(0);
            Tick_Timer_Reset(PwrCtrlCxt.Tick);
            PwrCtrlCxt.Now=PwrCtrlCxt.Next;
            __tsdbsu_t("Prog=", PwrCtrlCxt.Now);
            break;

        case PWRCTRL_DISCHR:
            CellState.dischrErr=0;

            if(CELL_VCEL_Get()<400)
            {
                Tick_Timer_Reset(PwrCtrlCxt.Tick);
                PwrCtrlCxt.Now=PWRCTRL_PWREN;
                PwrCtrlCxt.Count.dischr=0;
                __tsdbs_t("Pwr ena");
            }
            else if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, CellPara->CELL_PWR_DISCHR_WAIT)) // Power is not ready in CELL_PWR_DISCHR_WAIT ms
            {
                __tsdbsu_t("DisChr err: ", CELL_VCEL_Get());
                CellState.dischrErr=1;
                PwrCtrlCxt.Now=PWRCTRL_INIT;

                if(PwrCtrlCxt.Count.dischr<PwrCtrlCxt.Retry)
                {
                    PwrCtrlCxt.Count.dischr++;
                    __dbsu_t(", retry ", PwrCtrlCxt.Count.dischr);
                }
                else
                    PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
            }
            break;

        case PWRCTRL_PWREN:
            CELL_PWREN_SetState(1);

            if(CellState.vcelRdy==1)
            {
                PwrCtrlCxt.Now=PWRCTRL_TRIG_ON;
                PwrCtrlCxt.Count.vcell=0;
                Tick_Timer_Reset(PwrCtrlCxt.Tick);
                __tsdbsu_t("Pwr ready: ", CELL_VCEL_Get());
                __dbs_t(", Trig on");
            }
            else if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, 1000)) // Power is not ready in 1000ms
            {
                __tsdbsu_t("Pwr err: ", CELL_VCEL_Get());
                PwrCtrlCxt.Now=PWRCTRL_INIT;

                if(PwrCtrlCxt.Count.vcell<PwrCtrlCxt.Retry)
                {
                    PwrCtrlCxt.Count.vcell++;
                    __dbsu_t(", retry ", PwrCtrlCxt.Count.vcell);
                }
                else
                    PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
            }
            break;

        case PWRCTRL_TRIG_ON:
            CELL_ONOFF_SetState(1);

            if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, CellPara->CELL_ONOFF_PULSE))
            {
                CELL_ONOFF_SetState(0);
                PwrCtrlCxt.Now=PWRCTRL_CHECK_VAUX;
                __tsdbs_t("Check Vaux");
            }

            break;

        case PWRCTRL_CHECK_VAUX:
            if(CellState.vauxRdy==1)
            {
                PwrCtrlCxt.Now=PWRCTRL_MODULE_RDY;
                PwrCtrlCxt.Count.vaux=0;
                Tick_Timer_Reset(PwrCtrlCxt.Tick);
                __tsdbs_t("Check SwRdy");
            }
            else if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, CellPara->CELL_VAUX_TIMEOUT)) // VAUX is not ready in CELL_VAUX_TIMEOUT ms
            {
                __tsdbsu_t("Vaux err: ", CELL_VAUX_Get());
                PwrCtrlCxt.Now=PWRCTRL_INIT;

                if(PwrCtrlCxt.Count.vaux<PwrCtrlCxt.Retry)
                {
                    PwrCtrlCxt.Count.vaux++;
                    __dbsu_t(", retry ", PwrCtrlCxt.Count.vaux);
                }
                else
                    PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
            }
            break;

        case PWRCTRL_MODULE_RDY:
            if(CellState.vcelRdy==1)
            {
                if(CellState.vauxRdy==1)
                {
                    CellState.reboot=0;
                    Tick_Timer_Reset(PwrCtrlCxt.Tick);
                }
                else
                {
                    CellState.reboot=1;

                    if(CellPara->CELL_VAUX_TIMEOUT>0)
                    {
                        if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, CellPara->CELL_VAUX_TIMEOUT)) // VAUX is not ready in CELL_VAUX_TIMEOUT ms
                        {
                            __tsdbsu_t("Vaux lost: ", CELL_VAUX_Get());
                            PwrCtrlCxt.Now=PWRCTRL_INIT;

                            if(PwrCtrlCxt.Count.reboot<PwrCtrlCxt.Retry)
                            {
                                PwrCtrlCxt.Count.reboot++;
                                __dbsu_t(", retry ", PwrCtrlCxt.Count.reboot);
                            }
                            else
                                PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
                        }
                    }
                }
            }
            else
            {
                // The Status is not be updated here.
                __tsdbsu_t("Vin low: ", CELL_VCEL_Get());
                __dbs_t(", Power off");
                PwrCtrlCxt.Now=PWRCTRL_TRIG_OFF;
                PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
                Tick_Timer_Reset(PwrCtrlCxt.Tick);
            }
            break;

        case PWRCTRL_TRIG_OFF:
            if(CellState.vauxRdy==1)
            {
                if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, CellPara->CELL_ONOFF_PULSE))
                {
                    CELL_ONOFF_SetState(1);
                    PwrCtrlCxt.Now=PWRCTRL_RELEASE_OFF;
                    __tsdbs_t("Trig off");
                }
            }
            else
            {
                Tick_Timer_Reset(PwrCtrlCxt.Tick);
                PwrCtrlCxt.Now=PWRCTRL_PWRDISABLE;
            }
            break;

        case PWRCTRL_RELEASE_OFF:
            if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, CellPara->CELL_ONOFF_PULSE))
            {
                CELL_ONOFF_SetState(0);
                PwrCtrlCxt.Now=PWRCTRL_PWRDISABLE;
                __tsdbs_t("Rel off");
            }
            break;

        case PWRCTRL_PWRDISABLE:
            if(CellState.vauxRdy==1)
            {
                if(Tick_Timer_Is_Over_Ms(PwrCtrlCxt.Tick, 3000))
                    __tsdbs_t("Force pwr dis");
                else
                    break;
            }

            PwrCtrlCxt.Now=PWRCTRL_INIT;
            break;
    }

    Task_Done();
} // </editor-fold>

void Cell_TurnOn(bool dischrFirst, uint8_t Retry) // <editor-fold defaultstate="collapsed" desc="Turn on the module">
{
    if(Cell_Ready()==0)
    {
        __tsdbs("Turn on");
        CellState.busy=1;
        CellState.reboot=1;
        PwrCtrlCxt.Retry=Retry;
        PwrCtrlCxt.Count.dischr=0;
        PwrCtrlCxt.Count.reboot=0;
        PwrCtrlCxt.Count.vaux=0;
        PwrCtrlCxt.Count.vcell=0;
        PwrCtrlCxt.Now=PWRCTRL_INIT;

        if(dischrFirst==1)
            PwrCtrlCxt.Next=PWRCTRL_DISCHR;
        else
            PwrCtrlCxt.Next=PWRCTRL_PWREN;

        __dbs("\nCreate task: Cell_PwrCtrl_Tasks");
        TaskManager_Create_NewSimpleTask(Cell_PwrCtrl_Tasks);
    }
    else
        __tsdbs("Already on");
} // </editor-fold>

void Cell_TurnOff(void) // <editor-fold defaultstate="collapsed" desc="Turn off the module">
{
    if(CellState.vauxRdy==1)
    {
        PwrCtrlCxt.Now=PWRCTRL_MODULE_OFF;
        PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
        __tsdbs("Turn off");
    }
    else if(PwrCtrlCxt.Now!=PWRCTRL_INIT)
    {
        PwrCtrlCxt.Now=PWRCTRL_INIT;
        PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
        __tsdbs("Force off");
    }
    else
        __tsdbs("Already off");
} // </editor-fold>

bool Cell_PwrCtrl_IsError(void) // <editor-fold defaultstate="collapsed" desc="Check error state">
{
    if((CellState.val&0b00010001)>0) // reboot & discharge error
        return 1;

    if((CellState.val&0b00001110)<0b00001110)// no Sw Rdy, no VAUX, no Vcel
        return 1;

    return 0;
} // </editor-fold>

void Cell_PwrCtrl_Init(void) // <editor-fold defaultstate="collapsed" desc="Initialize">
{
    CellPara=CELL_HAL_Init();
    Tick_Timer_Reset(PwrCtrlCxt.Tick);
    CellState.val=0;
    PwrCtrlCxt.Now=PWRCTRL_INIT;
    PwrCtrlCxt.Next=PWRCTRL_MODULE_OFF;
} // </editor-fold>
