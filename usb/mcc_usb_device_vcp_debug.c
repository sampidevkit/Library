#include "mcc_usb_device_vcp.h"
#include "mcc_usb_device_vcp_debug.h"

private uint8_t VcpPortIdx=0;
private port_txbuf_t VcpTxBuf;
private port_rxbuf_t VcpRxBuf;

public void VCP_Debug_Write(uint8_t c) // <editor-fold defaultstate="collapsed" desc="Debug write a byte">
{
    Tick_Timer_Reset(VcpTxBuf.tk);

    while(VcpTxBuf.len==CDC_DATA_IN_EP_SIZE)
    {
        if(VcpTxBuf.ready==0)
        {
            VcpTxBuf.len=0;
            return;
        }

        TaskManager();
    }

    VcpTxBuf.buf[VcpTxBuf.len++]=c;
} // </editor-fold>

private new_simple_task_t(VCP_Debug_Tasks) // <editor-fold defaultstate="collapsed" desc="VCP debug tasks">
{
    if((USBGetDeviceState()<CONFIGURED_STATE)||(USBIsDeviceSuspended()==true))
        Task_Break();

    if(VcpTxBuf.ready)
    {
        // USB CDC0 get data
        if(VcpRxBuf.len==0)
        {
            VcpRxBuf.len=getsUSBUSART_CMD(VcpPortIdx, VcpRxBuf.buf, CDC_DATA_OUT_EP_SIZE);

            if(VcpRxBuf.len>0)
            {
                VcpRxBuf.buf[VcpRxBuf.len]=0x00;

                if(VcpRxBuf.cbfnc!=NULL)
                {
                    VcpRxBuf.cbfnc(VcpRxBuf.buf, VcpRxBuf.len);
                    VcpRxBuf.len=0;
                }
            }
        }
        // USB CDC0 put data
        switch(VcpTxBuf.next)
        {
            case 0:
                if(Tick_Timer_Is_Over_Ms(VcpTxBuf.tk, 10)||(VcpTxBuf.len==CDC_DATA_IN_EP_SIZE))
                {
                    if(VcpTxBuf.len>0)
                        VcpTxBuf.next=1;
                    else
                        break;
                }
                else
                    break;

            case 1:
                if(USBUSARTIsTxTrfReady(0))
                {
                    VcpTxBuf.next=2;
                    putUSBUSART(VcpPortIdx, VcpTxBuf.buf, VcpTxBuf.len);
                }
                break;

            default: // Waiting for TX done
                CDCTxService(VcpPortIdx);

                if(USBUSARTIsTxTrfReady(VcpPortIdx)) // check TX state again
                {
                    VcpTxBuf.next=0;
                    VcpTxBuf.len=0;
                    Tick_Timer_Reset(VcpTxBuf.tk);
                }
                break;
        }
    }
    else
    {
        Tick_Timer_Reset(VcpTxBuf.tk);
        VcpTxBuf.next=0;
        VcpTxBuf.len=0;
    }

    Task_Done();
} // </editor-fold>

public void VCP_Debug_SetRxCallback(void (*cbfnc)(uint8_t *, uint8_t)) // <editor-fold defaultstate="collapsed" desc="VCP Debug set RX callback">
{
    VcpRxBuf.cbfnc=cbfnc;
} // </editor-fold>

private void DTRPinHandler(char logic) // <editor-fold defaultstate="collapsed" desc="DTR pin handler">
{
    if(logic==USB_CDC_DTR_ACTIVE_LEVEL)
        VcpTxBuf.ready=1;
    else
        VcpTxBuf.ready=0;
} // </editor-fold>

public void VCP_Debug_Init(uint8_t PortIdx) // <editor-fold defaultstate="collapsed" desc="VCP debug initialize">
{
    VcpPortIdx=PortIdx;
    VcpTxBuf.next=0;
    VcpRxBuf.len=0;
    VcpRxBuf.cbfnc=NULL;
    DTRPin_SetHandler(VcpPortIdx, DTRPinHandler);
    TaskManager_Create_NewSimpleTask(VCP_Debug_Tasks);
} // </editor-fold>