#include "mcc_usb_device_vcp.h"
#include "mcc_usb_device_vcp0.h"

port_cxt_t VcpCxt[NUM_OF_VCP_PORT];

private new_simple_task_t(VCP_Tasks) // <editor-fold defaultstate="collapsed" desc="VCP Tasks">
{
    if((USBGetDeviceState()<CONFIGURED_STATE)||(USBIsDeviceSuspended()==true))
        Task_Break();

    VCP0_Task();

#if(NUM_OF_VCP_PORT>=2)
    VCP1_Task();
#endif

#if(NUM_OF_VCP_PORT>=3)
    VCP2_Task();
#endif

#if(NUM_OF_VCP_PORT>=4)
    VCP3_Task();
#endif

#if(NUM_OF_VCP_PORT>=5)
    VCP4_Task();
#endif

#if(NUM_OF_VCP_PORT>=6)
    VCP5_Task();
#endif

#if(NUM_OF_VCP_PORT>=7)
    VCP6_Task();
#endif

    Task_Done();
} // </editor-fold>

public void VCP_Init(void) // <editor-fold defaultstate="collapsed" desc="USB CDC initialize">
{
    uint8_t i=0;

    VCP0_Init(i++);

#if(NUM_OF_VCP_PORT>=2)
    VCP1_Init(i++);
#endif

#if(NUM_OF_VCP_PORT>=3)
    VCP2_Init(i++);
#endif

#if(NUM_OF_VCP_PORT>=4)
    VCP3_Init(i++);
#endif

#if(NUM_OF_VCP_PORT>=5)
    VCP4_Init(i++);
#endif

#if(NUM_OF_VCP_PORT>=6)
    VCP5_Init(i++);
#endif

#if(NUM_OF_VCP_PORT>=7)
    VCP6_Init(i++);
#endif
} // </editor-fold>
