#include "mcc_usb_device_vcp.h"
#include "mcc_usb_device_vcp0.h"

port_cxt_t VcpCxt[NUM_OF_CDC_PORTS];

private void BridgePort_TxdLedSetState(uint8_t portIdx, bool logic) // <editor-fold defaultstate="collapsed" desc="TX led">
{
    switch(portIdx)
    {
        case 0:
            BridgePort0_TxdLedSetState(logic);
            break;

#if(NUM_OF_CDC_PORTS>=2)
        case 1:
            BridgePort1_TxdLedSetState(logic);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=3)
        case 2:
            BridgePort2_TxdLedSetState(logic);
            break;
#endif
#if(NUM_OF_CDC_PORTS>=4)
        case 3:
            BridgePort3_TxdLedSetState(logic);
            break;
#endif
#if(NUM_OF_CDC_PORTS>=5)
        case 4:
            BridgePort4_TxdLedSetState(logic);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=6)
        case 5:
            BridgePort5_TxdLedSetState(logic);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=7)
        case 6:
            BridgePort6_TxdLedSetState(logic);
            break;
#endif

        default:
            break;
    }
} // </editor-fold>

private void BridgePort_RxdLedSetState(uint8_t portIdx, bool logic) // <editor-fold defaultstate="collapsed" desc="RX led">
{
    switch(portIdx)
    {
        case 0:
            BridgePort0_RxdLedSetState(logic);
            break;

#if(NUM_OF_CDC_PORTS>=2)
        case 1:
            BridgePort1_RxdLedSetState(logic);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=3)
        case 2:
            BridgePort2_RxdLedSetState(logic);
            break;
#endif
#if(NUM_OF_CDC_PORTS>=4)
        case 3:
            BridgePort3_RxdLedSetState(logic);
            break;
#endif
#if(NUM_OF_CDC_PORTS>=5)
        case 4:
            BridgePort4_RxdLedSetState(logic);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=6)
        case 5:
            BridgePort5_RxdLedSetState(logic);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=7)
        case 6:
            BridgePort6_RxdLedSetState(logic);
            break;
#endif

        default:
            break;
    }
} // </editor-fold>

private bool BridgePort_IsTxReady(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="Bridge port TX state">
{
    switch(portIdx)
    {
        case 0:
            return BridgePort0_IsTxReady();

#if(NUM_OF_CDC_PORTS>=2)
        case 1:
            return BridgePort1_IsTxReady();
#endif

#if(NUM_OF_CDC_PORTS>=3)
        case 2:
            return BridgePort2_IsTxReady();
#endif
#if(NUM_OF_CDC_PORTS>=4)
        case 3:
            return BridgePort3_IsTxReady();
#endif
#if(NUM_OF_CDC_PORTS>=5)
        case 4:
            return BridgePort4_IsTxReady();
#endif

#if(NUM_OF_CDC_PORTS>=6)
        case 5:
            return BridgePort5_IsTxReady();
#endif

#if(NUM_OF_CDC_PORTS>=7)
        case 6:
            return BridgePort6_IsTxReady();
#endif

        default:
            return 0;
    }
} // </editor-fold>

private bool BridgePort_IsTxDone(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="Bridge port TX done">
{
    switch(portIdx)
    {
        case 0:
            return BridgePort0_IsTxDone();

#if(NUM_OF_CDC_PORTS>=2)
        case 1:
            return BridgePort1_IsTxDone();
#endif

#if(NUM_OF_CDC_PORTS>=3)
        case 2:
            return BridgePort2_IsTxDone();
#endif
#if(NUM_OF_CDC_PORTS>=4)
        case 3:
            return BridgePort3_IsTxDone();
#endif
#if(NUM_OF_CDC_PORTS>=5)
        case 4:
            return BridgePort4_IsTxDone();
#endif

#if(NUM_OF_CDC_PORTS>=6)
        case 5:
            return BridgePort5_IsTxDone();
#endif

#if(NUM_OF_CDC_PORTS>=7)
        case 6:
            return BridgePort6_IsTxDone();
#endif

        default:
            return 0;
    }
} // </editor-fold>

private bool BridgePort_IsRxReady(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="Bridge port RX state">
{
    switch(portIdx)
    {
        case 0:
            return BridgePort0_IsRxReady();

#if(NUM_OF_CDC_PORTS>=2)
        case 1:
            return BridgePort1_IsRxReady();
#endif

#if(NUM_OF_CDC_PORTS>=3)
        case 2:
            return BridgePort2_IsRxReady();
#endif
#if(NUM_OF_CDC_PORTS>=4)
        case 3:
            return BridgePort3_IsRxReady();
#endif
#if(NUM_OF_CDC_PORTS>=5)
        case 4:
            return BridgePort4_IsRxReady();
#endif

#if(NUM_OF_CDC_PORTS>=6)
        case 5:
            return BridgePort5_IsRxReady();
#endif

#if(NUM_OF_CDC_PORTS>=7)
        case 6:
            return BridgePort6_IsRxReady();
#endif

        default:
            return 0;
    }
} // </editor-fold>

private void BridgePort_WriteByte(uint8_t portIdx, uint8_t b) // <editor-fold defaultstate="collapsed" desc="Bridge port write byte">
{
    switch(portIdx)
    {
        case 0:
            BridgePort0_WriteByte(b);
            break;

#if(NUM_OF_CDC_PORTS>=2)
        case 1:
            BridgePort1_WriteByte(b);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=3)
        case 2:
            BridgePort2_WriteByte(b);
            break;
#endif
#if(NUM_OF_CDC_PORTS>=4)
        case 3:
            BridgePort3_WriteByte(b);
            break;
#endif
#if(NUM_OF_CDC_PORTS>=5)
        case 4:
            BridgePort4_WriteByte(b);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=6)
        case 5:
            BridgePort5_WriteByte(b);
            break;
#endif

#if(NUM_OF_CDC_PORTS>=7)
        case 6:
            BridgePort6_WriteByte(b);
            break;
#endif

        default:
            break;
    }
} // </editor-fold>

private uint8_t BridgePort_ReadByte(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="Bridge port read byte">
{
    switch(portIdx)
    {
        case 0:
            return BridgePort0_ReadByte();

#if(NUM_OF_CDC_PORTS>=2)
        case 1:
            return BridgePort1_ReadByte();
#endif

#if(NUM_OF_CDC_PORTS>=3)
        case 2:
            return BridgePort2_ReadByte();
#endif
#if(NUM_OF_CDC_PORTS>=4)
        case 3:
            return BridgePort3_ReadByte();
#endif
#if(NUM_OF_CDC_PORTS>=5)
        case 4:
            return BridgePort4_ReadByte();
#endif

#if(NUM_OF_CDC_PORTS>=6)
        case 5:
            return BridgePort5_ReadByte();
#endif

#if(NUM_OF_CDC_PORTS>=7)
        case 6:
            return BridgePort6_ReadByte();
#endif

        default:
            return 0;
    }
} // </editor-fold>

private void VCP_Task(uint8_t portIdx) // <editor-fold defaultstate="collapsed" desc="VCP0 task">
{
    uint8_t i;

    if(VcpCxt[portIdx].opened==1)
    {
        uint8_t data[CDC_DATA_IN_EP_SIZE];
        uint8_t len=getsUSBUSART(portIdx, data, CDC_DATA_IN_EP_SIZE);

        if(len>0)
        {
            BridgePort_RxdLedSetState(portIdx, 1);
            Tick_Timer_Reset(VcpCxt[portIdx].tkRxLed);
        }

        for(i=0; i<len; i++)
        {
            while(!BridgePort_IsTxReady(portIdx));
            BridgePort_WriteByte(portIdx, data[i]);
        }

        switch(VcpCxt[portIdx].txBuf.next)
        {
            case 0: // Get RX data
                if(BridgePort_IsRxReady(portIdx))
                {
                    i=0;

                    do
                    {
                        VcpCxt[portIdx].txBuf.data[i]=BridgePort_ReadByte(portIdx);

                        if(++i>=CDC_DATA_OUT_EP_SIZE)
                            break;
                    }
                    while(BridgePort_IsRxReady(portIdx));

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
                BridgePort_TxdLedSetState(portIdx, 1);
                Tick_Timer_Reset(VcpCxt[portIdx].tkTxLed);

                if(USBUSARTIsTxTrfReady(portIdx)||(VcpCxt[portIdx].opened==0)) // check TX state again
                    VcpCxt[portIdx].txBuf.next=0;
                break;
        }

        if(BridgePort_IsTxDone(portIdx))
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[portIdx].tkTxLed, 20))
                BridgePort_TxdLedSetState(portIdx, 0);
        }

        if(BridgePort_IsRxReady(portIdx)==0)
        {
            if(Tick_Timer_Is_Over_Ms(VcpCxt[portIdx].tkRxLed, 20))
                BridgePort_RxdLedSetState(portIdx, 0);
        }
    }
    else
    {
        Tick_Timer_Reset(VcpCxt[portIdx].txBuf.tkData);
        VcpCxt[portIdx].txBuf.next=0;
        BridgePort_TxdLedSetState(portIdx, 0);
        BridgePort_RxdLedSetState(portIdx, 0);
    }
} // </editor-fold>

private new_simple_task_t(VCP_Tasks) // <editor-fold defaultstate="collapsed" desc="VCP Tasks">
{
    uint8_t i;

    if((USBGetDeviceState()<CONFIGURED_STATE)||(USBIsDeviceSuspended()==true))
        Task_Break();

    for(i=0; i<NUM_OF_CDC_PORTS; i++)
        VCP_Task(i);

    Task_Done();
} // </editor-fold>

public void VCP_Init(void) // <editor-fold defaultstate="collapsed" desc="USB CDC initialize">
{
    uint8_t i=0;

    for(i=0; i<NUM_OF_CDC_PORTS; i++)
    {
        BridgePort_TxdLedSetState(i, 0);
        BridgePort_RxdLedSetState(i, 0);
        Tick_Timer_Reset(VcpCxt[i].tkRxLed);
        Tick_Timer_Reset(VcpCxt[i].tkTxLed);

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
    }
} // </editor-fold>
