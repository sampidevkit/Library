#include "sx1268_transceiver.h"
#include "sx1268.h"
#include "common/debug.h"

#define SX1268_RXTX_BUFF_SIZE 32

static struct
{
    uint8_t head;
    uint8_t tail;
    uint8_t data[SX1268_RXTX_BUFF_SIZE];
} rxBuff;

static uint8_t txBusy=0;

void sx1268_interface_receive_callback(uint16_t type, uint8_t *buf, uint16_t len)
{
    __dbsu("\r\nsx1268 irq ", type);

    switch(type)
    {
        case SX1268_IRQ_TX_DONE:
        {
            txBusy=0;
            __dbs(": tx done.");
            break;
        }

        case SX1268_IRQ_RX_DONE:
        {
            uint16_t i;
            sx1268_bool_t enable;
            __dbs(": rx ready.");

            /* check the error */
            if(sx1268_check_packet_error(&enable)!=0)
                return;

            if((enable==SX1268_BOOL_FALSE)&&len)
            {
                for(i=0; i<len; i++)
                {
                    if(rxBuff.head==rxBuff.tail) // over flow
                    {
                        rxBuff.head=0;
                        rxBuff.tail=0;
                    }

                    rxBuff.data[rxBuff.head]=*buf;
                    buf++;
                    rxBuff.head++;

                    if(rxBuff.head>=SX1268_RXTX_BUFF_SIZE)
                        rxBuff.head=0;
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

bool sx1268_setRxMode(void) // <editor-fold defaultstate="collapsed" desc="Set RX mode">
{
    uint8_t setup;

    __tsdbs("set lora packet params");

    if(sx1268_set_lora_packet_params(50,
            SX1268_LORA_HEADER_EXPLICIT, 255,
            SX1268_LORA_CRC_TYPE_ON, SX1268_BOOL_FALSE)!=0)
        return 1;

    __tsdbs("get iq polarity");

    if(sx1268_get_iq_polarity((uint8_t *)&setup)!=0)
        return 1;

    setup|=1<<2;

    __tsdbs("set the iq polarity");

    if(sx1268_set_iq_polarity(setup)!=0)
        return 1;

    __tsdbs("start receive");

    if(sx1268_continuous_receive()!=0)
        return 1;

    return 0;
} // </editor-fold>

bool sx1268_transceiver_init(void) // <editor-fold defaultstate="collapsed" desc="initialize">
{
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;

    __tsdbs("sx1268 init");

    txBusy=0;
    rxBuff.head=0;
    rxBuff.tail=0;

    if(sx1268_init()!=0)
    {
        __dbs(", error");
        return 1;
    }

    while(1)
    {
        __tsdbs("enter standby");

        if(sx1268_set_standby(SX1268_CLOCK_SOURCE_XTAL_32MHZ)!=0)
            break;

        __tsdbs("disable stop timer on preamble");

        if(sx1268_set_stop_timer_on_preamble(SX1268_BOOL_FALSE)!=0)
            break;

        __tsdbs("set dc dc ldo");

        if(sx1268_set_regulator_mode(SX1268_REGULATOR_MODE_DC_DC_LDO)!=0)
            break;

        __tsdbs("set +17dBm power");

        if(sx1268_set_pa_config(0x02, 0x03)!=0)
            break;

        __tsdbs("enter to stdby rc mode");

        if(sx1268_set_rx_tx_fallback_mode(SX1268_RX_TX_FALLBACK_MODE_STDBY_XOSC)!=0)
            break;

        __tsdbs("set dio irq");

        if(sx1268_set_dio_irq_params(0x03FFU, 0x03FFU, 0x0000, 0x0000)!=0)
            break;

        __tsdbs("clear irq status");

        if(sx1268_clear_irq_status(0x03FFU)!=0)
            break;

        __tsdbs("set lora mode");

        if(sx1268_set_packet_type(SX1268_PACKET_TYPE_LORA)!=0)
            break;

        __tsdbs("set TX power +17dBm");

        if(sx1268_set_tx_params(17, SX1268_RAMP_TIME_10US)!=0)
            break;

        __tsdbs("set SF9, 125kHz, cr4/5, disable low data rate optimize");

        if(sx1268_set_lora_modulation_params(SX1268_LORA_SF_9, SX1268_LORA_BANDWIDTH_125_KHZ,
                SX1268_LORA_CR_4_5, SX1268_BOOL_FALSE)!=0)
            break;

        __tsdbs("convert the frequency");

        if(sx1268_frequency_convert_to_register(480100000U, (uint32_t *)&reg)!=0)
            break;

        __tsdbs("set the frequency");

        if(sx1268_set_rf_frequency(reg)!=0)
            break;

        __tsdbs("set base address");

        if(sx1268_set_buffer_base_address(0x00, 0x00)!=0)
            break;

        __tsdbs("set lora symb num");

        if(sx1268_set_lora_symb_num_timeout(0)!=0)
            break;

        __tsdbs("reset stats");

        if(sx1268_reset_stats(0x0000, 0x0000, 0x0000)!=0)
            break;

        __tsdbs("clear device errors");

        if(sx1268_clear_device_errors()!=0)
            break;

        __tsdbs("set the lora sync word");

        if(sx1268_set_lora_sync_word(0x1424U)!=0)
            break;

        __tsdbs("get tx modulation");

        if(sx1268_get_tx_modulation((uint8_t *)&modulation)!=0)
            break;

        modulation|=0x04;

        __tsdbs("set the tx modulation");

        if(sx1268_set_tx_modulation(modulation)!=0)
            break;

        __tsdbs("set the rx gain");

        if(sx1268_set_rx_gain(0x94)!=0)
            break;

        __tsdbs("set the ocp");

        if(sx1268_set_ocp(0x38)!=0)
            break;

        __tsdbs("get the tx clamp config");

        if(sx1268_get_tx_clamp_config((uint8_t *)&config)!=0)
            break;

        config|=0x1E;

        __tsdbs("set the tx clamp config");

        if(sx1268_set_tx_clamp_config(config)!=0)
            break;

        if(sx1268_setRxMode())
            break;

        return 0;
    }

    __dbs(", error code ");
    sx1268_deinit();
    return 1;
} // </editor-fold>

bool sx1268_transceiver_is_rx_ready(void) // <editor-fold defaultstate="collapsed" desc="check rx state">
{
    return (rxBuff.head!=rxBuff.tail);
} // </editor-fold>

bool sx1268_transceiver_is_tx_ready(void) // <editor-fold defaultstate="collapsed" desc="check tx state">
{
    return (txBusy==1);
} // </editor-fold>

bool sx1268_transceiver_is_tx_done(void) // <editor-fold defaultstate="collapsed" desc="check tx process state">
{
    return (txBusy==0);
} // </editor-fold>

uint8_t sx1268_transceiver_send(const uint8_t *pD, uint8_t len) // <editor-fold defaultstate="collapsed" desc="send data">
{
    uint8_t sent;

    txBusy=1;

    if(len>SX1268_RXTX_BUFF_SIZE) // max 192 bytes
        sent=SX1268_RXTX_BUFF_SIZE;
    else
        sent=len;

    if(sx1268_lora_transmit(SX1268_CLOCK_SOURCE_XTAL_32MHZ,
            50, SX1268_LORA_HEADER_EXPLICIT,
            SX1268_LORA_CRC_TYPE_ON, SX1268_BOOL_FALSE,
            (uint8_t *) pD, sent, 0))
    {
        sx1268_deinit();

        return 0;
    }

    return sent;
} // </editor-fold>

uint8_t sx1268_transceiver_receive(uint8_t *pD, uint8_t len) // <editor-fold defaultstate="collapsed" desc="receive data">
{
    uint8_t i=0;

    while((rxBuff.tail!=rxBuff.head)&&(i<len))
    {
        pD[i]=rxBuff.data[rxBuff.tail];
        i++;
        rxBuff.tail++;
    }

    return i;
} // </editor-fold>

void sx1268_transceiver_task(void) // <editor-fold defaultstate="collapsed" desc="transceiver task">
{
    if(sx1268_interface_busy_gpio_read()==0)
        sx1268_irq_handler();
} // </editor-fold>
