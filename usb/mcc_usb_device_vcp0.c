#include "mcc_usb_device_vcp.h"
#include "mcc_usb_device_vcp0.h"

private uint8_t PortIdx=0;

private void LineCodingHandler(void) // <editor-fold defaultstate="collapsed" desc="Line coding handler">
{
    //Update the baudrate info in the CDC driver
    CDCSetBaudRate(cdc_notice.GetLineCoding.dwDTERate);
    BridgePort0_Disable();
    BridgePort0_Enable(cdc_notice.GetLineCoding.dwDTERate);
} // </editor-fold>

private void DTRPinHandler(char logic) // <editor-fold defaultstate="collapsed" desc="DTR pin handler">
{
    if(logic==USB_CDC_DTR_ACTIVE_LEVEL)
        VcpCxt[PortIdx].opened=1;
    else
        VcpCxt[PortIdx].opened=0;

    BridgePort0_DtrPinSetState(VcpCxt[PortIdx].opened);
} // </editor-fold>

public void VCP0_Init(uint8_t PortIdx)
{
    PortIdx=PortIdx;
    Vcp0RxBuf.head=0;
    Vcp0RxBuf.tail=0;
    USB_CDC_SetLineCodingHandler(PortIdx, LineCodingHandler);
    DTRPin_SetHandler(PortIdx, DTRPinHandler);
}

public bool VCP_IsTxReady(void)
{
    if(VcpCxt[portIdx].txBuf.len<CDC_DATA_OUT_EP_SIZE)
        return 1;

    return 0;
}

public bool VCP_IsTxDone(void)
{
    if(VcpCxt[portIdx].txBuf.len==0)
        return 1;

    return 0;
}

public bool VCP_IsRxReady(void)
{
    if(VcpCxt[portIdx].rxBuf.len<CDC_DATA_OUT_EP_SIZE)
        return 1;

    return 0;
}

public void VCP_WriteByte(uint8_t portIdx, uint8_t b)
{

}

public uint8_t VCP_ReadByte(void)
{

}
