#include "bootloader.h"
#include "bld_extmem.h"
#include "flash_access.h"
#include "misc/intel_hex.h"
#include "system/system_tick.h"
#include "system/task_manager.h"

enum UPGRADE_TASKS
{
    UPGRADE_INIT=0,
    UPGRADE_READ,
    UPGRADE_DECODE,
    UPGRADE_SUCCESS,
    UPGRADE_ERROR,
    UPGRADE_REBOOT
};

enum DOWNLOAD_TASKS
{
    DOWNLOAD_INIT=0,
    DOWNLOAD_READ,
    DOWNLOAD_WRITE,
    DOWNLOAD_REPORT_ACK,
    DOWNLOAD_REPORT_NACK,
    DOWNLOAD_REBOOT
};

private int8_t rslt;
private tick_timer_t Tick;
private uint8_t Buffer[BLD_BUFFER_SIZE];
private uint8_t i, DoNext, ToDo, BufferLen;

private new_simple_task_t(Download_Tasks) // <editor-fold defaultstate="collapsed" desc="Download task">
{
    switch(DoNext)
    {
        case DOWNLOAD_INIT:
            BufferLen=0;
            BLD_Comm_Init();
            IHEX_Init(1);
            BLD_ExtMem_WriteState(BLD_STATE_DOWNLOADING);
            Tick_Timer_Reset(Tick);
            BLD_DownloadLed_SetState(1);
            DoNext=DOWNLOAD_READ;
            break;

        case DOWNLOAD_READ:
            if(BLD_IsRxReady())
            {
                Tick_Timer_Reset(Tick);
                Buffer[BufferLen]=BLD_Read();

                if(Buffer[BufferLen]=='\r')
                {
                    BufferLen++;
                    DoNext=DOWNLOAD_WRITE;
                    break;
                }
                else if(BufferLen<BLD_BUFFER_SIZE)
                    BufferLen++;
            }

            if(BufferLen>0)
            {
                if(Tick_Timer_Is_Over_Ms(Tick, 100))
                {
                    if(BufferLen<BLD_BUFFER_SIZE)
                        DoNext=DOWNLOAD_WRITE;
                    else
                    {
                        Buffer[0]='R';
                        DoNext=DOWNLOAD_REPORT_NACK;
                        ToDo=DOWNLOAD_READ;
                    }
                }
            }
            break;

        case DOWNLOAD_WRITE:
            DoNext=DOWNLOAD_REPORT_ACK;
            ToDo=DOWNLOAD_READ;

            for(i=0; i<BufferLen; i++)
            {
                rslt=IHEX_Decode(Buffer[i]);

                if(rslt==IHEX_ERROR)
                {
                    Buffer[0]='W';
                    DoNext=DOWNLOAD_REPORT_NACK;
                }
                else if(rslt==IHEX_DONE)
                    ToDo=DOWNLOAD_REBOOT;
            }

            if(DoNext==DOWNLOAD_REPORT_ACK)
            {
                for(i=0; i<BufferLen; i++)
                    ;//BLD_ExtMem_WriteData(Buffer[i]);

                if(ToDo==DOWNLOAD_REBOOT)
                    ;//BLD_ExtMem_WriteState(BLD_STATE_NEWFW);
            }
            break;

        case DOWNLOAD_REPORT_ACK:
            BLD_Write('A');
            BufferLen=0;
            Tick_Timer_Reset(Tick);
            DoNext=ToDo;
            break;

        case DOWNLOAD_REPORT_NACK:
            BLD_Write('K');
            BLD_Write(Buffer[0]);
            BufferLen=0;
            Tick_Timer_Reset(Tick);
            DoNext=ToDo;
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
            BufferLen=0;
            IHEX_Init(0); // disable lock
            BLD_UpgradeLed_SetState(1);
            DoNext=UPGRADE_READ;
            break;

        case UPGRADE_READ:
            Buffer[BufferLen]=BLD_ExtMem_ReadData();

            if(Buffer[BufferLen]=='\n')
                DoNext=UPGRADE_DECODE;
            else if(BufferLen<BLD_BUFFER_SIZE)
                BufferLen++;
            else
                DoNext=UPGRADE_ERROR;
            break;

        case UPGRADE_DECODE: // Process data
            for(i=0; i<BufferLen; i++)
            {
                rslt=IHEX_Decode(Buffer[i]);

                if(rslt==PROC_DONE)
                {
                    DoNext=UPGRADE_SUCCESS;
                    break;
                }
                else if(rslt==PROC_ERR)
                {
                    DoNext=UPGRADE_ERROR;
                    break;
                }
            }

            if(DoNext==UPGRADE_DECODE)
                DoNext=UPGRADE_READ;
            break;

        case UPGRADE_SUCCESS: // Success
            BLD_ExtMem_WriteState(BLD_STATE_FIRST_RUN);
            Jump2App(); // Jump out this function if no application
            DoNext=UPGRADE_REBOOT;
            break;

        case UPGRADE_ERROR: // Error
            BLD_ExtMem_WriteState(BLD_STATE_DOWNLOADING);
            DoNext=UPGRADE_REBOOT;
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
