#include "mcc_usb_device_vcp.h"
#include "mcc_usb_device_vcp1.h"

private uint8_t PortIdx=1;

private void LineCodingHandler(void) // <editor-fold defaultstate="collapsed" desc="Line coding handler">
{
    //Update the baudrate info in the CDC driver
    CDCSetBaudRate(cdc_notice.GetLineCoding.dwDTERate);
    BridgePort1_Disable();
    BridgePort1_Enable(cdc_notice.GetLineCoding.dwDTERate);
} // </editor-fold>

private void DTRPinHandler(char logic) // <editor-fold defaultstate="collapsed" desc="DTR pin handler">
{
    if(logic==USB_CDC_DTR_ACTIVE_LEVEL)
        VcpCxt[PortIdx].Ctrl.opened=1;
    else
        VcpCxt[PortIdx].Ctrl.opened=0;

    BridgePort1_DtrPinSetState(VcpCxt[PortIdx].Ctrl.opened);
} // </editor-fold>

public void VCP1_Init(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="Initialize">
{
    PortIdx=portIdx;
    Vcp1RxBuf.head=0;
    Vcp1RxBuf.tail=0;
    
    if(Vcp1RxBuf.data==NULL)
        VcpCxt[PortIdx].Ctrl.mode=1; // default is Bright mode
    else
        VcpCxt[PortIdx].Ctrl.mode=0; // default is VCP mode
    
    VcpCxt[PortIdx].rxBuf=&Vcp1RxBuf;
    VcpCxt[PortIdx].bridgePort.IsRxReady=BridgePort1_IsRxReady;
    VcpCxt[PortIdx].bridgePort.IsTxDone=BridgePort1_IsTxDone;
    VcpCxt[PortIdx].bridgePort.IsTxReady=BridgePort1_IsTxReady;
    VcpCxt[PortIdx].bridgePort.ReadByte=BridgePort1_ReadByte;
    VcpCxt[PortIdx].bridgePort.RxdLedSetState=BridgePort1_RxdLedSetState;
    VcpCxt[PortIdx].bridgePort.TxdLedSetState=BridgePort1_TxdLedSetState;
    VcpCxt[PortIdx].bridgePort.WriteByte=BridgePort1_WriteByte;
    USB_CDC_SetLineCodingHandler(PortIdx, LineCodingHandler);
    DTRPin_SetHandler(PortIdx, DTRPinHandler);
} // </editor-fold>

public bool VCP1_IsTxReady(void) // <editor-fold defaultstate="collapsed" desc="check TX state">
{
    if(VcpCxt[PortIdx].txBuf.len==CDC_DATA_IN_EP_SIZE)
        return 0;

    return VcpCxt[PortIdx].Ctrl.opened;
} // </editor-fold>

public bool VCP1_IsTxDone(void) // <editor-fold defaultstate="collapsed" desc="check TX transmitte state">
{
    if(VcpCxt[PortIdx].txBuf.len==0)
        return 0;

    return 1;
} // </editor-fold>

public bool VCP1_IsRxReady(void) // <editor-fold defaultstate="collapsed" desc="check RX state">
{
    if(VcpCxt[PortIdx].rxBuf->tail!=VcpCxt[PortIdx].rxBuf->head)
        return 1;

    return 0;
} // </editor-fold>

public void VCP1_WriteByte(uint8_t b) // <editor-fold defaultstate="collapsed" desc="Write 1 byte to VCP">
{
    while(VcpCxt[PortIdx].txBuf.len==CDC_DATA_IN_EP_SIZE)
    {
        if(VcpCxt[PortIdx].Ctrl.opened==0)
        {
            VcpCxt[PortIdx].txBuf.len=0;
            return;
        }

        TaskManager();
    }

    Tick_Timer_Reset(VcpCxt[PortIdx].txBuf.tkData);
    VcpCxt[PortIdx].txBuf.data[VcpCxt[PortIdx].txBuf.len++]=b;
} // </editor-fold>

public uint8_t VCP1_ReadByte(void) // <editor-fold defaultstate="collapsed" desc="Get 1 byte from VCP">
{
    uint8_t b;
    
    if(VcpCxt[PortIdx].rxBuf->tail==VcpCxt[PortIdx].rxBuf->head)
        return 0xFF;
    
    b=VcpCxt[PortIdx].rxBuf->data[VcpCxt[PortIdx].rxBuf->tail++];

    if(VcpCxt[PortIdx].rxBuf->tail>=VcpCxt[PortIdx].rxBuf->size)
        VcpCxt[PortIdx].rxBuf->tail=0;
    
    return b;
} // </editor-fold>

