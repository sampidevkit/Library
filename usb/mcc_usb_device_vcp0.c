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
        VcpCxt[PortIdx].Ctrl.opened=1;
    else
        VcpCxt[PortIdx].Ctrl.opened=0;

    BridgePort0_DtrPinSetState(VcpCxt[PortIdx].Ctrl.opened);
} // </editor-fold>

public void VCP0_Init(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="Initialize">
{
    PortIdx=portIdx;
    Vcp0RxBuf.head=0;
    Vcp0RxBuf.tail=0;
    VcpCxt[PortIdx].rxBuf=&Vcp0RxBuf;
    VcpCxt[PortIdx].bridgePort.IsRxReady=BridgePort0_IsRxReady;
    VcpCxt[PortIdx].bridgePort.IsTxDone=BridgePort0_IsTxDone;
    VcpCxt[PortIdx].bridgePort.IsTxReady=BridgePort0_IsTxReady;
    VcpCxt[PortIdx].bridgePort.ReadByte=BridgePort0_ReadByte;
    VcpCxt[PortIdx].bridgePort.RxdLedSetState=BridgePort0_RxdLedSetState;
    VcpCxt[PortIdx].bridgePort.TxdLedSetState=BridgePort0_TxdLedSetState;
    VcpCxt[PortIdx].bridgePort.WriteByte=BridgePort0_WriteByte;
    USB_CDC_SetLineCodingHandler(PortIdx, LineCodingHandler);
    DTRPin_SetHandler(PortIdx, DTRPinHandler);
} // </editor-fold>

public bool VCP0_IsTxReady(void) // <editor-fold defaultstate="collapsed" desc="check TX state">
{
    if(VcpCxt[PortIdx].txBuf.len==CDC_DATA_IN_EP_SIZE)
        return 0;

    return VcpCxt[PortIdx].Ctrl.opened;
} // </editor-fold>

public bool VCP0_IsTxDone(void) // <editor-fold defaultstate="collapsed" desc="check TX transmitte state">
{
    if(VcpCxt[PortIdx].txBuf.len==0)
        return 0;

    return 1;
} // </editor-fold>

public bool VCP0_IsRxReady(void) // <editor-fold defaultstate="collapsed" desc="check RX state">
{
    if(VcpCxt[PortIdx].rxBuf->tail!=VcpCxt[PortIdx].rxBuf->head)
        return 1;

    return 0;
} // </editor-fold>

public void VCP0_WriteByte(uint8_t b) // <editor-fold defaultstate="collapsed" desc="Write 1 byte to VCP">
{
    Tick_Timer_Reset(VcpCxt[PortIdx].txBuf.tkData);

    while(VcpCxt[PortIdx].txBuf.len==CDC_DATA_IN_EP_SIZE)
    {
        if(VcpCxt[PortIdx].Ctrl.opened==0)
        {
            VcpCxt[PortIdx].txBuf.len=0;
            return;
        }

        TaskManager();
    }

    VcpCxt[PortIdx].txBuf.data[VcpCxt[PortIdx].txBuf.len++]=b;
} // </editor-fold>

public uint8_t VCP0_ReadByte(void) // <editor-fold defaultstate="collapsed" desc="Get 1 byte from VCP">
{
    uint8_t b;
    
    if(VcpCxt[PortIdx].rxBuf->tail==VcpCxt[PortIdx].rxBuf->head)
        return 0xFF;
    
    b=VcpCxt[PortIdx].rxBuf->data[VcpCxt[PortIdx].rxBuf->tail++];

    if(VcpCxt[PortIdx].rxBuf->tail>=VcpCxt[PortIdx].rxBuf->size)
        VcpCxt[PortIdx].rxBuf->tail=0;
    
    return b;
} // </editor-fold>

