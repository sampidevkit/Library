#include "mcc_usb_device_vcp.h"
#include "mcc_usb_device_vcp0.h"

port_cxt_t VcpCxt[NUM_OF_CDC_PORTS];
static uint8_t tmpData[CDC_DATA_OUT_EP_SIZE];

private void Vcp_Task(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="VCP task">
{
    uint8_t i;

    if(VcpCxt[portIdx].Ctrl.opened==1)
    {
        uint8_t len=getsUSBUSART(portIdx, tmpData, CDC_DATA_OUT_EP_SIZE);

        if(len>0)
        {
            VcpCxt[portIdx].bridgePort.RxdLedSetState(1);
            Tick_Timer_Reset(VcpCxt[portIdx].tkRxLed);
        }

        for(i=0; i<len; i++)
        {
            VcpCxt[portIdx].rxBuf->data[VcpCxt[portIdx].rxBuf->head++]=tmpData[i];

            if(VcpCxt[portIdx].rxBuf->head>=VcpCxt[portIdx].rxBuf->size)
            {
                VcpCxt[portIdx].rxBuf->head=0;
                VcpCxt[portIdx].rxBuf->tail=0;
            }
        }
        // USB CDC0 put data
        switch(VcpCxt[portIdx].txBuf.next)
        {
            case 0:
                if(Tick_Timer_Is_Over_Ms(VcpCxt[portIdx].txBuf.tkData, 10)||(VcpCxt[portIdx].txBuf.len==CDC_DATA_IN_EP_SIZE))
                {
                    if(VcpCxt[portIdx].txBuf.len>0)
                        VcpCxt[portIdx].txBuf.next=1;
                    else
                        break;
                }
                else
                    break;

            case 1:
                if(USBUSARTIsTxTrfReady(portIdx))
                {
                    VcpCxt[portIdx].txBuf.next=2;
                    putUSBUSART(portIdx, VcpCxt[portIdx].txBuf.data, VcpCxt[portIdx].txBuf.len);
                }
                break;

            default: // Waiting for TX done
                CDCTxService(portIdx);

                if(USBUSARTIsTxTrfReady(portIdx)) // check TX state again
                {
                    VcpCxt[portIdx].txBuf.next=0;
                    VcpCxt[portIdx].txBuf.len=0;
                    Tick_Timer_Reset(VcpCxt[portIdx].txBuf.tkData);
                }
                break;
        }

        if(VcpCxt[portIdx].bridgePort.IsTxDone())
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[portIdx].tkTxLed, 20))
                VcpCxt[portIdx].bridgePort.TxdLedSetState(0);
        }

        if(VcpCxt[portIdx].bridgePort.IsRxReady()==0)
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[portIdx].tkRxLed, 20))
                VcpCxt[portIdx].bridgePort.RxdLedSetState(0);
        }
    }
    else
    {
        Tick_Timer_Reset(VcpCxt[portIdx].txBuf.tkData);
        VcpCxt[portIdx].txBuf.next=0;
        VcpCxt[portIdx].bridgePort.TxdLedSetState(0);
        VcpCxt[portIdx].bridgePort.RxdLedSetState(0);
    }
} // </editor-fold>

private void Bridge_Task(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="Bridge port task">
{
    uint8_t i;

    if(VcpCxt[portIdx].Ctrl.opened==1)
    {
        uint8_t len=getsUSBUSART(portIdx, tmpData, CDC_DATA_OUT_EP_SIZE);

        if(len>0)
        {
            VcpCxt[portIdx].bridgePort.RxdLedSetState(1);
            Tick_Timer_Reset(VcpCxt[portIdx].tkRxLed);
        }

        for(i=0; i<len; i++)
        {
            while(!VcpCxt[portIdx].bridgePort.IsTxReady());
            VcpCxt[portIdx].bridgePort.WriteByte(tmpData[i]);
        }

        switch(VcpCxt[portIdx].txBuf.next)
        {
            case 0: // Get RX data
                if(VcpCxt[portIdx].bridgePort.IsRxReady())
                {
                    i=0;

                    do
                    {
                        VcpCxt[portIdx].txBuf.data[i]=VcpCxt[portIdx].bridgePort.ReadByte();

                        if(++i>=CDC_DATA_IN_EP_SIZE)
                            break;
                    }
                    while(VcpCxt[portIdx].bridgePort.IsRxReady());

                    VcpCxt[portIdx].txBuf.len=i;
                    VcpCxt[portIdx].txBuf.next=1;
                }
                else
                    break;

            case 1: // Put TX data
                if(USBUSARTIsTxTrfReady(portIdx))
                {
                    VcpCxt[portIdx].txBuf.next=2;
                    putUSBUSART(portIdx, VcpCxt[portIdx].txBuf.data, VcpCxt[portIdx].txBuf.len);
                }
                break;

            default: // Waiting for TX done
                CDCTxService(portIdx);
                VcpCxt[portIdx].bridgePort.TxdLedSetState(1);
                Tick_Timer_Reset(VcpCxt[portIdx].tkTxLed);

                if(USBUSARTIsTxTrfReady(portIdx)||(VcpCxt[portIdx].Ctrl.opened==0)) // check TX state again
                    VcpCxt[portIdx].txBuf.next=0;
                break;
        }

        if(VcpCxt[portIdx].bridgePort.IsTxDone())
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[portIdx].tkTxLed, 20))
                VcpCxt[portIdx].bridgePort.TxdLedSetState(0);
        }

        if(VcpCxt[portIdx].bridgePort.IsRxReady()==0)
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[portIdx].tkRxLed, 20))
                VcpCxt[portIdx].bridgePort.RxdLedSetState(0);
        }
    }
    else
    {
        Tick_Timer_Reset(VcpCxt[portIdx].txBuf.tkData);
        VcpCxt[portIdx].txBuf.next=0;
        VcpCxt[portIdx].bridgePort.TxdLedSetState(0);
        VcpCxt[portIdx].bridgePort.RxdLedSetState(0);
    }
} // </editor-fold>

private new_simple_task_t(VCP_Tasks) // <editor-fold defaultstate="collapsed" desc="VCP Tasks">
{
    uint8_t i;

    if((USBGetDeviceState()<CONFIGURED_STATE)||(USBIsDeviceSuspended()==true))
        Task_Break();

    for(i=0; i<NUM_OF_CDC_PORTS; i++)
    {
        if(VcpCxt[i].Ctrl.mode==1)
            Bridge_Task(i);
        else
            Vcp_Task(i);
    }

    Task_Done();
} // </editor-fold>

public void VCP_Init(void) // <editor-fold defaultstate="collapsed" desc="USB CDC initialize">
{
    uint8_t i=0;

    for(i=0; i<NUM_OF_CDC_PORTS; i++)
    {
        switch(i)
        {
            case 0:
                VCP0_Init(i);
                break;

#if(NUM_OF_CDC_PORTS>=2)
            case 1:
                VCP1_Init(i);
                break;
#endif

#if(NUM_OF_CDC_PORTS>=3)
            case 2:
                VCP2_Init(i);
                break;
#endif

#if(NUM_OF_CDC_PORTS>=4)
            case 3:
                VCP3_Init(i);
                break;
#endif

#if(NUM_OF_CDC_PORTS>=5)
            case 4:
                VCP4_Init(i);
                break;
#endif

#if(NUM_OF_CDC_PORTS>=6)
            case 5:
                VCP5_Init(i);
                break;
#endif

#if(NUM_OF_CDC_PORTS>=7)
            case 6:
                VCP6_Init(i);
                break;
#endif

            default:
                break;
        }

        VcpCxt[i].bridgePort.TxdLedSetState(0);
        VcpCxt[i].bridgePort.RxdLedSetState(0);
        Tick_Timer_Reset(VcpCxt[i].tkRxLed);
        Tick_Timer_Reset(VcpCxt[i].tkTxLed);
    }

    TaskManager_Create_NewSimpleTask(VCP_Tasks);
} // </editor-fold>
