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

public void VCP0_Task(void) // <editor-fold defaultstate="collapsed" desc="VCP0 task">
{
    uint8_t i;

    if(VcpCxt[PortIdx].opened==1)
    {
        uint8_t data[CDC_DATA_IN_EP_SIZE];
        uint8_t len=getsUSBUSART(PortIdx, data, CDC_DATA_IN_EP_SIZE);

        if(len>0)
        {
            BridgePort0_RxdLedSetState(1);
            Tick_Timer_Reset(VcpCxt[PortIdx].tkRxLed);
        }

        for(i=0; i<len; i++)
        {
            while(!BridgePort0_IsTxReady());
            BridgePort0_WriteByte(data[i]);
        }

        switch(VcpCxt[PortIdx].txBuf.next)
        {
            case 0: // Get RX data
                if(BridgePort0_IsRxReady())
                {
                    i=0;

                    do
                    {
                        VcpCxt[PortIdx].txBuf.data[i]=BridgePort0_ReadByte();

                        if(++i>=CDC_DATA_OUT_EP_SIZE)
                            break;
                    }
                    while(BridgePort0_IsRxReady());

                    VcpCxt[PortIdx].txBuf.len=i;
                    VcpCxt[PortIdx].txBuf.next=1;
                }
                else
                    break;

            case 1: // Put TX data
                if(USBUSARTIsTxTrfReady(PortIdx))
                {
                    VcpCxt[PortIdx].txBuf.next=2;
                    putUSBUSART(PortIdx, VcpCxt[PortIdx].txBuf.data, VcpCxt[PortIdx].txBuf.len);
                }
                break;

            default: // Waiting for TX done
                CDCTxService(PortIdx);
                BridgePort0_TxdLedSetState(1);
                Tick_Timer_Reset(VcpCxt[PortIdx].tkTxLed);

                if(USBUSARTIsTxTrfReady(PortIdx)||(VcpCxt[PortIdx].opened==0)) // check TX state again
                    VcpCxt[PortIdx].txBuf.next=0;
                break;
        }

        if(BridgePort0_IsTxDone())
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[PortIdx].tkTxLed, 20))
                BridgePort0_TxdLedSetState(0);
        }

        if(BridgePort0_IsRxReady()==0)
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[PortIdx].tkRxLed, 20))
                BridgePort0_RxdLedSetState(0);
        }
    }
    else
    {
        Tick_Timer_Reset(VcpCxt[PortIdx].txBuf.tkData);
        VcpCxt[PortIdx].txBuf.next=0;
        BridgePort0_TxdLedSetState(0);
        BridgePort0_RxdLedSetState(0);
    }
} // </editor-fold>



public void VCP0_Init(uint8_t PortIdx)
{
    PortIdx=PortIdx;
    BridgePort0_TxdLedSetState(0);
    BridgePort0_RxdLedSetState(0);
    Tick_Timer_Reset(VcpCxt[PortIdx].tkRxLed);
    Tick_Timer_Reset(VcpCxt[PortIdx].tkTxLed);
    USB_CDC_SetLineCodingHandler(PortIdx, LineCodingHandler);
    DTRPin_SetHandler(PortIdx, DTRPinHandler);
}
