#include "bootloader.h"
#include "bld_extmem.h"
#include "flash_access.h"
#include "misc/intel_hex.h"
#include "system/system_tick.h"
#include "system/task_manager.h"
#include "misc/util.h"

enum UPGRADE_TASKS
{
    UPGRADE_INIT=0,
    UPGRADE_PROCESS,
    UPGRADE_REBOOT
};

enum DOWNLOAD_TASKS
{
    DOWNLOAD_INIT=0,
    DOWNLOAD_PROCESS,
    DOWNLOAD_REPORT,
    DOWNLOAD_REBOOT
};

private bool first;
private int Opcode;
private size_t Idx;
private int8_t rslt;
private tick_timer_t Tick;
private uint8_t DoNext, RebootReq, Buff;

private new_simple_task_t(Download_Tasks) // <editor-fold defaultstate="collapsed" desc="Download task">
{
    switch(DoNext)
    {
        case DOWNLOAD_INIT:
            BLD_Comm_Init();
            BLD_ExtMem_WriteState(BLD_STATE_DOWNLOADING);
            Tick_Timer_Reset(Tick);
            BLD_DownloadLed_SetState(1);
            DoNext=DOWNLOAD_PROCESS;
            RebootReq=0;
            first=1;
            Idx=0;
            break;

        case DOWNLOAD_PROCESS:
            if(BLD_IsRxReady())
            {
                first=0;
                Tick_Timer_Reset(Tick);
                Buff=BLD_Read();
                //BLD_ExtMem_WriteData(Buff);

                if(FindString(Buff, &Idx, ":00000001FF"))
                {
                    RebootReq=1;
                    BLD_ExtMem_WriteState(BLD_STATE_NEWFW);
                }

                if(Buff=='\n')
                {
                    Buff='A';
                    DoNext=DOWNLOAD_REPORT;
                }
            }

            if(first==0)
            {
                if(Tick_Timer_Is_Over_Ms(Tick, 1000))
                {
                    Buff='N';
                    DoNext=DOWNLOAD_REPORT;
                }
            }
            break;

        case DOWNLOAD_REPORT:
            if(BLD_IsTxReady())
            {
                BLD_Write(Buff);
                Tick_Timer_Reset(Tick);

                if(RebootReq==1)
                    DoNext=DOWNLOAD_REBOOT;
                else
                {
                    Idx=0;
                    DoNext=DOWNLOAD_PROCESS;
                }
            }
            break;

        case DOWNLOAD_REBOOT:
        default:
            BLD_SystemReboot();
            TaskManager_End_Task(Download_Tasks);
            break;
    }

    Task_Done();
} // </editor-fold>

private new_simple_task_t(Upgrade_Tasks) // <editor-fold defaultstate="collapsed" desc="Upgrade task">
{
    switch(DoNext)
    {
        case UPGRADE_INIT:
            IHEX_Init();
            BLD_UpgradeLed_SetState(1);
            DoNext=UPGRADE_PROCESS;
            break;

        case UPGRADE_PROCESS:
            BLD_UpgradeLed_SetState(0);
            Opcode=BLD_ExtMem_ReadData();

            if(Opcode==EOF)
                rslt=IHEX_ERROR;
            else
            {
                BLD_UpgradeLed_SetState(1);
                rslt=IHEX_Decode((uint8_t) Opcode);
            }

            if(rslt==IHEX_DONE)
            {
                BLD_ExtMem_WriteState(BLD_STATE_FIRST_RUN);
                Jump2App(); // Jump out this function if no application
                DoNext=UPGRADE_REBOOT;
                break;
            }
            else if(rslt==IHEX_ERROR)
            {
                BLD_ExtMem_WriteState(BLD_STATE_DOWNLOADING);
                DoNext=UPGRADE_REBOOT;
                break;
            }
            break;

        case UPGRADE_REBOOT: // Do nothing
        default:
            BLD_SystemReboot();
            TaskManager_End_Task(Upgrade_Tasks);
            break;
    }

    Task_Done();
} // </editor-fold>

public void BootLoader_Initialize(void) // <editor-fold defaultstate="collapsed" desc="Boot loader initialize">
{
    bld_stt_t bldStt=BLD_ExtMem_Init();

    BLD_DownloadLed_SetState(0);
    BLD_UpgradeLed_SetState(0);

    if((BLD_Trigger_GetState()==0)||(bldStt==BLD_STATE_DOWNLOADING)||(bldStt==BLD_STATE_FIRST_RUN)) // IO trigger
    {
        // bldStt==BLD_STATE_DOWNLOADING : Download process is in progress
        // bldStt==BLD_STATE_FIRST_RUN : Application crash, it can not clear this state before system reboot
        DoNext=DOWNLOAD_INIT;
        TaskManager_Create_NewSimpleTask(Download_Tasks);
    }
    else if(bldStt==BLD_STATE_NEWFW) // New FW ready
    {
        DoNext=UPGRADE_INIT;
        TaskManager_Create_NewSimpleTask(Upgrade_Tasks);
        return;
    }
    else // Up to date
    {
        Jump2App(); // Jump out this function if no application
        // If application is not available, goto downloading task
        DoNext=DOWNLOAD_INIT;
        TaskManager_Create_NewSimpleTask(Download_Tasks);
    }
} // </editor-fold>

public void BootLoader_Deinitialize(void) // <editor-fold defaultstate="collapsed" desc="Bootloader deinitialize">
{
    BLD_SystemReboot();
} // </editor-fold>
