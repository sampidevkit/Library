#include "UartMultiflexer.h"
#include "Util.h"

static bool Stuffing;
static uint8_t PortIdx=0;
static uint8_t TxByteCount=0;
static uint8_t DoNext=0;

void UartMux_RxTask(void)
{
    uint8_t c;

    if(!UpStreamPort_IsRxReady())
        break;

    c=UpStreamPort_ReadByte();

    switch(DoNext)
    {
        case 0:
            if(c==0xFC)
            {
                Stuffing=0;
                PortIdx=0;
                DoNext++;
            }
            break;

        case 1:
            switch(c)
            {
                case 0x20:
                    Stuffing^=1;
                    break;

                default:
                    while(DownStreamPortCxt[PortIdx].isTxReady())
                        
                        break;
            }
            break;
    }
}

void UartMux_TxTask(void)
{

}
