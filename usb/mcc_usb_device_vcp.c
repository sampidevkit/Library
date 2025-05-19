#include "mcc_usb_device_vcp.h"
#include "mcc_usb_device_atc.h"
#include "mcc_usb_device_vcp_debug.h"

void VCP_Init(void) // <editor-fold defaultstate="collapsed" desc="USB CDC initialize">
{
    uint8_t i=0;
    
    VCP_Debug_Init(i++);
    
#ifdef USE_VCP_ATC
    VCP_ATC_Init(i++);
#endif
    
} // </editor-fold>
