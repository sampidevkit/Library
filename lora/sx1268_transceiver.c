#include "sx1268_transceiver.h"
#include "sx1268.h"
#include "common/debug.h"

static uint8_t gs_rx_done; /**< rx done */
static uint8_t gs_send_buffer[256]; /**< inner send buffer*/

void sx1268_interface_receive_callback(uint16_t type, uint8_t *buf, uint16_t len)
{
    switch(type)
    {
        case SX1268_IRQ_TX_DONE:
        {
            __dbs("sx1268: irq tx done.\n");

            break;
        }
        case SX1268_IRQ_RX_DONE:
        {
            uint16_t i;
            sx1268_bool_t enable;
            uint8_t rssi_pkt_raw;
            uint8_t snr_pkt_raw;
            uint8_t signal_rssi_pkt_raw;
            float rssi_pkt;
            float snr_pkt;
            float signal_rssi_pkt;

            __dbs("sx1268: irq rx done.\n");

            /* get the status */
            if(sx1268_get_lora_packet_status((uint8_t *)&rssi_pkt_raw, (uint8_t *)&snr_pkt_raw,
                    (uint8_t *)&signal_rssi_pkt_raw, (float *) &rssi_pkt,
                    (float *) &snr_pkt, (float *) &signal_rssi_pkt)!=0)
            {
                return;
            }
            //            __dbs("sx1268: rssi is %0.1f.\n", rssi_pkt);
            //            __dbs("sx1268: snr is %0.2f.\n", snr_pkt);
            //            __dbs("sx1268: signal rssi is %0.1f.\n", signal_rssi_pkt);

            /* check the error */
            if(sx1268_check_packet_error(&enable)!=0)
            {
                return;
            }
            if((enable==SX1268_BOOL_FALSE)&&len)
            {
                for(i=0; i<len; i++)
                {
                    //__dbs("0x%02X ", buf[i]);
                }
                __dbs("\n");
                gs_rx_done=1;
            }

            break;
        }
        case SX1268_IRQ_PREAMBLE_DETECTED:
        {
            __dbs("sx1268: irq preamble detected.\n");

            break;
        }
        case SX1268_IRQ_SYNC_WORD_VALID:
        {
            __dbs("sx1268: irq valid sync word detected.\n");

            break;
        }
        case SX1268_IRQ_HEADER_VALID:
        {
            __dbs("sx1268: irq valid header.\n");

            break;
        }
        case SX1268_IRQ_HEADER_ERR:
        {
            __dbs("sx1268: irq header error.\n");

            break;
        }
        case SX1268_IRQ_CRC_ERR:
        {
            __dbs("sx1268: irq crc error.\n");

            break;
        }
        case SX1268_IRQ_CAD_DONE:
        {
            __dbs("sx1268: irq cad done.\n");

            break;
        }
        case SX1268_IRQ_CAD_DETECTED:
        {
            __dbs("sx1268: irq cad detected.\n");

            break;
        }
        case SX1268_IRQ_TIMEOUT:
        {
            __dbs("sx1268: irq timeout.\n");

            break;
        }
        default:
        {
            break;
        }
    }
}

//uint8_t sx1268_interrupt_test_irq_handler(void)
//{
//    if(sx1268_irq_handler()!=0)
//    {
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
//}

//uint8_t sx1268_send_test(void)
//{
//    uint8_t res;
//    uint32_t reg;
//    uint8_t modulation;
//    uint8_t config;
//    uint8_t i;
//
//    /* start send test */
//    __dbs("sx1268: start send test.\n");
//
//    /* init the sx1268 */
//    res=sx1268_init();
//    if(res!=0)
//    {
//        __dbs("sx1268: init failed.\n");
//
//        return 1;
//    }
//
//    /* enter standby */
//    res=sx1268_set_standby(SX1268_CLOCK_SOURCE_XTAL_32MHZ);
//    if(res!=0)
//    {
//        __dbs("sx1268: set standby failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* disable stop timer on preamble */
//    res=sx1268_set_stop_timer_on_preamble(SX1268_BOOL_FALSE);
//    if(res!=0)
//    {
//        __dbs("sx1268: stop timer on preamble failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set dc dc ldo */
//    res=sx1268_set_regulator_mode(SX1268_REGULATOR_MODE_DC_DC_LDO);
//    if(res!=0)
//    {
//        __dbs("sx1268: set regulator mode failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set +17dBm power */
//    res=sx1268_set_pa_config(0x02, 0x03);
//    if(res!=0)
//    {
//        __dbs("sx1268: set pa config failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* enter to stdby rc mode */
//    res=sx1268_set_rx_tx_fallback_mode(SX1268_RX_TX_FALLBACK_MODE_STDBY_XOSC);
//    if(res!=0)
//    {
//        __dbs("sx1268: set rx tx fallback mode failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set dio irq */
//    res=sx1268_set_dio_irq_params(0x03FFU, 0x03FFU, 0x0000, 0x0000);
//    if(res!=0)
//    {
//        __dbs("sx1268: set dio irq params failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* clear irq status */
//    res=sx1268_clear_irq_status(0x03FFU);
//    if(res!=0)
//    {
//        __dbs("sx1268: clear irq status failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set lora mode */
//    res=sx1268_set_packet_type(SX1268_PACKET_TYPE_LORA);
//    if(res!=0)
//    {
//        __dbs("sx1268: set packet type failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* +17dBm */
//    res=sx1268_set_tx_params(17, SX1268_RAMP_TIME_10US);
//    if(res!=0)
//    {
//        __dbs("sx1268: set tx params failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* sf9, 125khz, cr4/5, disable low data rate optimize */
//    res=sx1268_set_lora_modulation_params(SX1268_LORA_SF_9, SX1268_LORA_BANDWIDTH_125_KHZ,
//            SX1268_LORA_CR_4_5, SX1268_BOOL_FALSE);
//    if(res!=0)
//    {
//        __dbs("sx1268: set lora modulation params failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* convert the frequency */
//    res=sx1268_frequency_convert_to_register(480100000U, (uint32_t *)&reg);
//    if(res!=0)
//    {
//        __dbs("sx1268: convert to register failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set the frequency */
//    res=sx1268_set_rf_frequency(reg);
//    if(res!=0)
//    {
//        __dbs("sx1268: set rf frequency failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set base address */
//    res=sx1268_set_buffer_base_address(0x00, 0x00);
//    if(res!=0)
//    {
//        __dbs("sx1268: set buffer base address failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* 1 lora symb num */
//    res=sx1268_set_lora_symb_num_timeout(0);
//    if(res!=0)
//    {
//        __dbs("sx1268: set lora symb num timeout failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* reset stats */
//    res=sx1268_reset_stats(0x0000, 0x0000, 0x0000);
//    if(res!=0)
//    {
//        __dbs("sx1268: reset stats failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* clear device errors */
//    res=sx1268_clear_device_errors();
//    if(res!=0)
//    {
//        __dbs("sx1268: clear device errors failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set the lora sync word */
//    res=sx1268_set_lora_sync_word(0x1424U);
//    if(res!=0)
//    {
//        __dbs("sx1268: set lora sync word failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* get tx modulation */
//    res=sx1268_get_tx_modulation((uint8_t *)&modulation);
//    if(res!=0)
//    {
//        __dbs("sx1268: get tx modulation failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//    modulation|=0x04;
//
//    /* set the tx modulation */
//    res=sx1268_set_tx_modulation(modulation);
//    if(res!=0)
//    {
//        __dbs("sx1268: set tx modulation failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set the rx gain */
//    res=sx1268_set_rx_gain(0x94);
//    if(res!=0)
//    {
//        __dbs("sx1268: set rx gain failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* set the ocp */
//    res=sx1268_set_ocp(0x38);
//    if(res!=0)
//    {
//        __dbs("sx1268: set ocp failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* get the tx clamp config */
//    res=sx1268_get_tx_clamp_config((uint8_t *)&config);
//    if(res!=0)
//    {
//        __dbs("sx1268: get tx clamp config failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//    config|=0x1E;
//
//    /* set the tx clamp config */
//    res=sx1268_set_tx_clamp_config(config);
//    if(res!=0)
//    {
//        __dbs("sx1268: set tx clamp config failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* generate the buffer */
//    for(i=0; i<192; i++)
//    {
//        gs_send_buffer[i]=i;
//    }
//
//    /* send the data */
//    res=sx1268_lora_transmit(SX1268_CLOCK_SOURCE_XTAL_32MHZ,
//            50, SX1268_LORA_HEADER_EXPLICIT,
//            SX1268_LORA_CRC_TYPE_ON, SX1268_BOOL_FALSE,
//            (uint8_t *) gs_send_buffer, 192, 0);
//    if(res!=0)
//    {
//        __dbs("sx1268: lora send failed.\n");
//        (void) sx1268_deinit();
//
//        return 1;
//    }
//
//    /* finish send test */
//    __dbs("sx1268: finish send test.\n");
//
//    /* deinit */
//    (void) sx1268_deinit();
//
//    return 0;
//}

/**
 * @brief     receive test
 * @param[in] s timeout
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t sx1268_receive_test(uint32_t s)
{
    uint8_t res;
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;
    uint8_t setup;

    /* start receive test */
    __tsdbs("\ninit the sx1268");
    res=sx1268_init();
    if(res!=0)
    {
        __dbs("sx1268: init failed.\n");

        return 1;
    }

    __tsdbs("\nenter standby");
    res=sx1268_set_standby(SX1268_CLOCK_SOURCE_XTAL_32MHZ);
    if(res!=0)
    {
        __dbs("sx1268: set standby failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\ndisable stop timer on preamble");
    res=sx1268_set_stop_timer_on_preamble(SX1268_BOOL_FALSE);
    if(res!=0)
    {
        __dbs("sx1268: stop timer on preamble failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset dc dc ldo");
    res=sx1268_set_regulator_mode(SX1268_REGULATOR_MODE_DC_DC_LDO);
    if(res!=0)
    {
        __dbs("sx1268: set regulator mode failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset +17dBm power");
    res=sx1268_set_pa_config(0x02, 0x03);
    if(res!=0)
    {
        __dbs("sx1268: set pa config failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nenter to stdby rc mode");
    res=sx1268_set_rx_tx_fallback_mode(SX1268_RX_TX_FALLBACK_MODE_STDBY_XOSC);
    if(res!=0)
    {
        __dbs("sx1268: set rx tx fallback mode failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset dio irq");
    res=sx1268_set_dio_irq_params(0x03FFU, 0x03FFU, 0x0000, 0x0000);
    if(res!=0)
    {
        __dbs("sx1268: set dio irq params failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nclear irq status");
    res=sx1268_clear_irq_status(0x03FFU);
    if(res!=0)
    {
        __dbs("sx1268: clear irq status failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset lora mode");
    res=sx1268_set_packet_type(SX1268_PACKET_TYPE_LORA);
    if(res!=0)
    {
        __dbs("sx1268: set packet type failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\n+17dBm");
    res=sx1268_set_tx_params(17, SX1268_RAMP_TIME_10US);
    if(res!=0)
    {
        __dbs("sx1268: set tx params failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nsf9, 125khz, cr4/5, disable low data rate optimize");
    res=sx1268_set_lora_modulation_params(SX1268_LORA_SF_9, SX1268_LORA_BANDWIDTH_125_KHZ,
            SX1268_LORA_CR_4_5, SX1268_BOOL_FALSE);
    if(res!=0)
    {
        __dbs("sx1268: set lora modulation params failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nconvert the frequency");
    res=sx1268_frequency_convert_to_register(480100000U, (uint32_t *)&reg);
    if(res!=0)
    {
        __dbs("sx1268: convert to register failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset the frequency");
    res=sx1268_set_rf_frequency(reg);
    if(res!=0)
    {
        __dbs("sx1268: set rf frequency failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset base address");
    res=sx1268_set_buffer_base_address(0x00, 0x00);
    if(res!=0)
    {
        __dbs("sx1268: set buffer base address failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset lora symb num");
    res=sx1268_set_lora_symb_num_timeout(0);
    if(res!=0)
    {
        __dbs("sx1268: set lora symb num timeout failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nreset stats");
    res=sx1268_reset_stats(0x0000, 0x0000, 0x0000);
    if(res!=0)
    {
        __dbs("sx1268: reset stats failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nclear device errors");
    res=sx1268_clear_device_errors();
    if(res!=0)
    {
        __dbs("sx1268: clear device errors failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset the lora sync word");
    res=sx1268_set_lora_sync_word(0x1424U);
    if(res!=0)
    {
        __dbs("sx1268: set lora sync word failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nget tx modulation");
    res=sx1268_get_tx_modulation((uint8_t *)&modulation);
    if(res!=0)
    {
        __dbs("sx1268: get tx modulation failed.\n");
        (void) sx1268_deinit();

        return 1;
    }
    modulation|=0x04;

    __tsdbs("\nset the tx modulation");
    res=sx1268_set_tx_modulation(modulation);
    if(res!=0)
    {
        __dbs("sx1268: set tx modulation failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset the rx gain");
    res=sx1268_set_rx_gain(0x94);
    if(res!=0)
    {
        __dbs("sx1268: set rx gain failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset the ocp");
    res=sx1268_set_ocp(0x38);
    if(res!=0)
    {
        __dbs("sx1268: set ocp failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nget the tx clamp config");
    res=sx1268_get_tx_clamp_config((uint8_t *)&config);
    if(res!=0)
    {
        __dbs("sx1268: get tx clamp config failed.\n");
        (void) sx1268_deinit();

        return 1;
    }
    config|=0x1E;

    __tsdbs("\nset the tx clamp config");
    res=sx1268_set_tx_clamp_config(config);
    if(res!=0)
    {
        __dbs("sx1268: set tx clamp config failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nset lora packet params");
    res=sx1268_set_lora_packet_params(50,
            SX1268_LORA_HEADER_EXPLICIT, 255,
            SX1268_LORA_CRC_TYPE_ON, SX1268_BOOL_FALSE);
    if(res!=0)
    {
        __dbs("sx1268: set lora packet params failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nget iq polarity");
    res=sx1268_get_iq_polarity((uint8_t *)&setup);
    if(res!=0)
    {
        __dbs("sx1268: get iq polarity failed.\n");
        (void) sx1268_deinit();

        return 1;
    }
    setup|=1<<2;

    __tsdbs("\nset the iq polarity");
    res=sx1268_set_iq_polarity(setup);
    if(res!=0)
    {
        __dbs("sx1268: set iq polarity failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    __tsdbs("\nstart receive");
    res=sx1268_continuous_receive();
    if(res!=0)
    {
        __dbs("sx1268: lora continuous receive failed.\n");
        (void) sx1268_deinit();

        return 1;
    }

    return 0;
}

bool sx1268_transceiver_init(void) // <editor-fold defaultstate="collapsed" desc="initialize">
{
    sx1268_receive_test(100);
    return 0;
} // </editor-fold>

bool sx1268_transceiver_is_rx_ready(void) // <editor-fold defaultstate="collapsed" desc="check rx state">
{
    return 0;
} // </editor-fold>

bool sx1268_transceiver_is_tx_ready(void) // <editor-fold defaultstate="collapsed" desc="comment">
{
    return 0;
}

bool sx1268_transceiver_is_tx_done(void)
{
    return 0;
}

void sx1268_transceiver_send(const uint8_t *pD, uint8_t len)
{
    //sx1268_send_test();
}

uint8_t sx1268_transceiver_receive(uint8_t *pD)
{
    return 0;
}

void sx1268_task(void)
{
    if(sx1268_interface_busy_gpio_read()==0)
        sx1268_irq_handler();
}