#include "../bootloader.h"
#include "../flash_access.h"
#include "../memmap.h"

extern void appFunction(void); // defined in app.s

void _general_exception_handler(void) // <editor-fold defaultstate="collapsed" desc="Exception handler">
{
    // Do nothing
    while(1);
} // </editor-fold>

void Jump2App(void) // <editor-fold defaultstate="collapsed" desc="Jump to application">
{
    uint32_t AppPtr;

    Flash_Access_Read(APP_RESET_ADDRESS, (void *) &AppPtr, 1);

    if(AppPtr!=0xFFFFFFFF)
    {
        if(FSECbits.CP==1) // Code protection bit must be enabled
            appFunction();
    }
} // </editor-fold>
