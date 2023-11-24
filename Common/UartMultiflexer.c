#include "UartMultiflexer.h"
#include "Util.h"

static struct
{
    bool Stuffing;
    uint8_t PortIdx;
    uint8_t ByteCount;
    uint8_t DoNext;
} TxCxt={0, 0, 0, 0};

static struct
{
    bool Stuffing;
    uint8_t PortIdx;
    uint8_t DoNext;
} RxCxt={0, 0, 0};

#ifdef USE_RTOS

void UartMux_Tasks(void *pvParameters)
{
    while(1)
    {

#elif defined(TASKMANAGER_H)

new_simple_task_t(UartMux_Tasks)
{
#else

void UartMux_Tasks(void *pvParameters)
{
#endif
        uint8_t c, SOF;

        /* **************************************************** Transmit Data */
        if(UpStreamPort_IsRxReady())
        {
            c=UpStreamPort_ReadByte();

            if(RxCxt.DoNext==0)
            {
                if(c==0xFC) // Start of frame
                {
                    RxCxt.DoNext=1;
                    RxCxt.PortIdx=0;
                    RxCxt.Stuffing=0;
                }
            }
            else
            {
                switch(c)
                {
                    case 0xFC: // Repeat Start of frame
                        RxCxt.DoNext=1;
                        RxCxt.PortIdx=0;
                        RxCxt.Stuffing=0;
                        break;

                    case 0xFD: // Change port index
                        RxCxt.Stuffing=0;

                        if(RxCxt.PortIdx<NUM_OF_DOWNSTREAM_PORTS)
                            RxCxt.PortIdx++;
                        break;

                    case 0xFE: // End of frame
                        RxCxt.DoNext=0;
                        break;

                    case 0x20: // Stuffing byte
                        RxCxt.Stuffing=1;
                        break;

                    default:
                        if(RxCxt.PortIdx<NUM_OF_DOWNSTREAM_PORTS)
                        {
                            while(!DownStreamPort[RxCxt.PortIdx].isTxReady());

                            if(RxCxt.Stuffing==1)
                            {
                                RxCxt.Stuffing=0;
                                c^=0x20;
                            }

                            DownStreamPort[RxCxt.PortIdx].WriteByte(c);
                        }
                        break;
                }
            }
        }
        /* ***************************************************** Receive Data */
        for(TxCxt.PortIdx=0, SOF=0; TxCxt.PortIdx<NUM_OF_DOWNSTREAM_PORTS; TxCxt.PortIdx++)
        {
            if(DownStreamPort[TxCxt.PortIdx].isRxReady())
            {
                if(SOF==0)
                {
                    SOF=1;
                    // Send Start frame
                    while(!UpStreamPort_IsTxReady());
                    UpStreamPort_IsTxReady(0xFC);
                }
                // Send Data
                TxCxt.ByteCount=0;

                while(DownStreamPort[TxCxt.PortIdx].isRxReady())
                {
                    c=DownStreamPort[TxCxt.PortIdx].ReadByte();

                    switch(c)
                    {
                        case 0xFC:
                        case 0xFD:
                        case 0xFE:
                        case 0x20:
                            while(!UpStreamPort_IsTxReady());
                            UpStreamPort_IsTxReady(0x20);
                            c^=20;
                            break;

                        default:
                            break;
                    }

                    while(!UpStreamPort_IsTxReady());
                    UpStreamPort_IsTxReady(c);

                    if(++TxCxt.ByteCount>=MAX_TX_DATA_PER_PORT)
                        break;
                }
                // Send port breaker
                if((TxCxt.PortIdx+1)<NUM_OF_DOWNSTREAM_PORTS)
                {
                    while(!UpStreamPort_IsTxReady());
                    UpStreamPort_IsTxReady(0xFD);
                }
                // Send End of frame
                while(!UpStreamPort_IsTxReady());
                UpStreamPort_IsTxReady(0xFE);
            }
        }
#ifdef USE_RTOS
        vTaskDelay(1/portTICK_PERIOD_MS);
    }
#elif defined(TASKMANAGER_H)
        Task_Done();
#endif
}