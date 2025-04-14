#include "bootloader.h"
#include "bld_extmem.h"
#include "flash_access.h"
#include "system/system_tick.h"
#include "system/soft_wdt.h"

enum
{
    BLD_COMM_READ=0,
    BLD_DATA_PROCESS,
    BLD_RSLT_SUCCESS,
    BLD_RSLT_ERROR,
    BLD_COMM_WRITE,
    BLD_IDLE
};

private uint8_t DoNext;
private uint8_t *pBuffer;
private tick_timer_t Tick;
private uint8_t i, BufferLen;
private uint8_t Buffer[BLD_BUFFER_SIZE];

extern void appFunction(void);

void _general_exception_handler(void) // <editor-fold defaultstate="collapsed" desc="Exception handler">
{
    // Do nothing
    while(1);
} // </editor-fold>

private void Jump2App(void) // <editor-fold defaultstate="collapsed" desc="Jump to application">
{
    uint32_t AppPtr;

    Flash_Access_Read(APP_RESET_ADDRESS, (void *) &AppPtr, 1);

    if(AppPtr!=0xFFFFFFFF)
    {
        if(BLD_CheckCodeProtect()) // Code protection bit must be enabled
            appFunction();
    }
} // </editor-fold>

public void BootLoader_Tasks(void) // <editor-fold defaultstate="collapsed" desc="Boot loader task">
{
    switch(DoNext)
    {
        case BLD_COMM_READ:
            Buffer[BufferLen]=BLD_ExtMem_ReadData();

            if(Buffer[BufferLen]=='\n')
            {
                if(BufferLen<BLD_BUFFER_SIZE)
                    DoNext=BLD_DATA_PROCESS;
                else
                    DoNext=BLD_RSLT_ERROR;

                Tick_Timer_Reset(Tick);
            }
            else if(BufferLen<BLD_BUFFER_SIZE)
                BufferLen++;
            break;

        case BLD_DATA_PROCESS: // Process data
            for(i=0; i<BufferLen; i++)
            {
                int8_t rslt;

                rslt=IHEX_Decode(Buffer[i]);

                if(rslt==PROC_DONE)
                {
                    DoNext=BLD_RSLT_SUCCESS;
                    break;
                }
                else if(rslt==PROC_ERR)
                {
                    DoNext=BLD_RSLT_ERROR;
                    break;
                }
            }

            DoNext=BLD_RSLT_SUCCESS;
            break;

        case BLD_RSLT_SUCCESS: // Success
            Buffer[0]='A';
            Buffer[1]=0;
            BufferLen=0;
            DoNext=BLD_COMM_WRITE;
            break;

        case BLD_RSLT_ERROR: // Error
            Buffer[0]='N';
            Buffer[1]=0;
            BufferLen=0;
            DoNext=BLD_COMM_WRITE;
            break;

        case BLD_COMM_WRITE: // Send response
            while(Buffer[BufferLen]!=0x00)
            {
                if(BLD_IsTxReady())
                    BLD_Write(Buffer[BufferLen++]);
                else
                    return;
            }

            DoNext=BLD_COMM_READ;
            BufferLen=0;
            Tick_Timer_Reset(Tick);
            break;

        case BLD_IDLE: // Do nothing
        default:
            break;
    }
} // </editor-fold>

public void BootLoader_Initialize(void) // <editor-fold defaultstate="collapsed" desc="Boot loader initialize">
{
    Tick_Timer_Reset(Tick);
    Enable_Peripheral_Interrupt();
    Enable_Global_Interrupt();

    if(BLD_Trigger_GetState()) // No trigger
    {
        if(BLD_ExtMem_Init()==BLD_STATE_NEWFW) // New FW ready
        {
            BufferLen=0;
            DoNext=BLD_COMM_READ;
            return;
        }
        else // Up to date
            Jump2App(); // Jump out this function if no application
    }

    DoNext=BLD_IDLE;
    BLD_Comm_Init();
} // </editor-fold>

public void BootLoader_Deinitialize(void) // <editor-fold defaultstate="collapsed" desc="Bootloader deinitialize">
{
    ClrWdt();
    softWDT_Disable();
} // </editor-fold>
