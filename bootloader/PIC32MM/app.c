#include "../bootloader.h"
#include "../flash_access.h"
#include "../memmap.h"

extern void appFunction(void); // defined in app.s

static uint32_t _excep_code, _excep_addr;

void _general_exception_handler(void) // <editor-fold defaultstate="collapsed" desc="Exception handler">
{
    /* Mask off Mask of the ExcCode Field from the Cause Register
    Refer to the MIPs Software User's manual */
    _excep_code=(_CP0_GET_CAUSE() & 0x0000007C)>>2;
    _excep_addr=_CP0_GET_EPC();

#ifdef __DEBUG    
    __builtin_software_breakpoint();
    /* If we are in debug mode, cause a software breakpoint in the debugger */
#endif

    while(1)
    {
        /* TODO:  Insert exception handling code. */
    }
} // </editor-fold>

void Jump2App(void) // <editor-fold defaultstate="collapsed" desc="Jump to application">
{
    uint32_t AppPtr;

    Flash_Access_Read(APP_RESET_ADDRESS, (void *) &AppPtr, 1);

    if(AppPtr!=0xFFFFFFFF)
    {
        if(FSECbits.CP==0) // Code protection bit must be enabled
            appFunction();
    }
} // </editor-fold>
