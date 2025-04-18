#include "sx1268.h"
#include <math.h>

// <editor-fold defaultstate="collapsed" desc="chip information definition">
#define CHIP_NAME                 "SX1268"                /**< chip name */
#define MANUFACTURER_NAME         "Semtech"               /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.8f                    /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        3.7f                    /**< chip max supply voltage */
#define MAX_CURRENT               107.0f                  /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                  /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                   /**< chip max operating temperature */
#define DRIVER_VERSION            1000                    /**< driver version */
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="chip command definition">
#define SX1268_COMMAND_SET_SLEEP                         0x84        /**< set sleep command */
#define SX1268_COMMAND_SET_STANDBY                       0x80        /**< set standby command */
#define SX1268_COMMAND_SET_FS                            0xC1        /**< set fs command */
#define SX1268_COMMAND_SET_TX                            0x83        /**< set tx command */
#define SX1268_COMMAND_SET_RX                            0x82        /**< set rx command */
#define SX1268_COMMAND_STOP_TIMER_ON_PREAMBLE            0x9F        /**< stop timer on preamble command */
#define SX1268_COMMAND_SET_RX_DUTY_CYCLE                 0x94        /**< set rx duty cycle command */
#define SX1268_COMMAND_SET_CAD                           0xC5        /**< set cad command */
#define SX1268_COMMAND_SET_TX_CONTINUOUS_WAVE            0xD1        /**< set tx continuous wave command */
#define SX1268_COMMAND_SET_TX_INFINITE_PREAMBLE          0xD2        /**< set tx infinite preamble command */
#define SX1268_COMMAND_SET_REGULATOR_MODE                0x96        /**< set regulator mode command */
#define SX1268_COMMAND_SET_CALIBRATE                     0x89        /**< set calibrate command */
#define SX1268_COMMAND_SET_CALIBRATE_IMAGE               0x98        /**< set calibrate image command */
#define SX1268_COMMAND_SET_PA_CONFIG                     0x95        /**< set pa config command */
#define SX1268_COMMAND_SET_RX_TX_FALLBACK_MODE           0x93        /**< set rx tx fallback mode command */
#define SX1268_COMMAND_WRITE_REGISTER                    0x0D        /**< write register command */
#define SX1268_COMMAND_READ_REGISTER                     0x1D        /**< read register command */
#define SX1268_COMMAND_WRITE_BUFFER                      0x0E        /**< write buffer command */
#define SX1268_COMMAND_READ_BUFFER                       0x1E        /**< read buffer command */
#define SX1268_COMMAND_SET_DIO_IRQ_PARAMS                0x08        /**< set dio irq params command */
#define SX1268_COMMAND_GET_IRQ_STATUS                    0x12        /**< get irq status command */
#define SX1268_COMMAND_CLEAR_IRQ_STATUS                  0x02        /**< clear irq status command */
#define SX1268_COMMAND_SET_DIO2_AS_RF_SWITCH_CTRL        0x9D        /**< set dio2 as rf switch ctrl command */
#define SX1268_COMMAND_SET_DIO3_AS_TCXO_CTRL             0x97        /**< set dio3 as tcxo ctrl command */
#define SX1268_COMMAND_SET_RF_FREQUENCY                  0x86        /**< set rf frequency command */
#define SX1268_COMMAND_SET_PACKET_TYPE                   0x8A        /**< set packet type command */
#define SX1268_COMMAND_GET_PACKET_TYPE                   0x11        /**< get packet type command */
#define SX1268_COMMAND_SET_TX_PARAMS                     0x8E        /**< set tx params command */
#define SX1268_COMMAND_SET_MODULATION_PARAMS             0x8B        /**< set modulation params command */
#define SX1268_COMMAND_SET_PACKET_PARAMS                 0x8C        /**< set packet params command */
#define SX1268_COMMAND_SET_CAD_PARAMS                    0x88        /**< set cad params command */
#define SX1268_COMMAND_SET_BUFFER_BASE_ADDRESS           0x8F        /**< set buffer base address command */
#define SX1268_COMMAND_SET_LORA_SYMB_NUM_TIMEOUT         0xA0        /**< set lora symb num timeout command */
#define SX1268_COMMAND_GET_STATUS                        0xC0        /**< get status command */
#define SX1268_COMMAND_GET_RSSI_LNST                     0x15        /**< get rssi inst command */
#define SX1268_COMMAND_GET_RX_BUFFER_STATUS              0x13        /**< get rx buffer status command */
#define SX1268_COMMAND_GET_PACKET_STATUS                 0x14        /**< get packet status command */
#define SX1268_COMMAND_GET_DEVICE_ERRORS                 0x17        /**< get device errors command */
#define SX1268_COMMAND_CLEAR_DEVICE_ERRORS               0x07        /**< clear device errors command */
#define SX1268_COMMAND_GET_STATS                         0x10        /**< get stats command */
#define SX1268_COMMAND_RESET_STATS                       0x00        /**< reset stats command */
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="chip register definition">
#define SX1268_REG_DIOX_OUTPUT_ENABLE                    0x0580      /**< diox output enable register */
#define SX1268_REG_DIOX_INPUT_ENABLE                     0x0583      /**< diox input enable register */
#define SX1268_REG_DIOX_PULL_UP_CONTROL                  0x0584      /**< diox pull up control register */
#define SX1268_REG_DIOX_PULL_DOWN_CONTROL                0x0585      /**< diox pull down control register */
#define SX1268_REG_WHITENING_INIT_VALUE_MSB              0x06B8      /**< whitening initial value msb register */
#define SX1268_REG_WHITENING_INIT_VALUE_LSB              0x06B9      /**< whitening initial value lsb register */
#define SX1268_REG_CRC_INIT_VALUE_MSB                    0x06BC      /**< crc msb initial value register */
#define SX1268_REG_CRC_INIT_VALUE_LSB                    0x06BD      /**< crc lsb initial value register */
#define SX1268_REG_CRC_POLYNOMIAL_VALUE_MSB              0x06BE      /**< crc msb polynomial value register */
#define SX1268_REG_CRC_POLYNOMIAL_VALUE_LSB              0x06BF      /**< crc lsb polynomial value register */
#define SX1268_REG_SYNC_WORD_0                           0x06C0      /**< 1st byte of the sync word in fsk mode register */
#define SX1268_REG_SYNC_WORD_1                           0x06C1      /**< 2nd byte of the sync word in fsk mode register */
#define SX1268_REG_SYNC_WORD_2                           0x06C2      /**< 3rd byte of the sync word in fsk mode register */
#define SX1268_REG_SYNC_WORD_3                           0x06C3      /**< 4th byte of the sync word in fsk mode register */
#define SX1268_REG_SYNC_WORD_4                           0x06C4      /**< 5th byte of the sync word in fsk mode register */
#define SX1268_REG_SYNC_WORD_5                           0x06C5      /**< 6th byte of the sync word in fsk mode register */
#define SX1268_REG_SYNC_WORD_6                           0x06C6      /**< 7th byte of the sync word in fsk mode register */
#define SX1268_REG_SYNC_WORD_7                           0x06C7      /**< 8th byte of the sync word in fsk mode register */
#define SX1268_REG_NODE_ADDRESS                          0x06CD      /**< node address register */
#define SX1268_REG_BROADCAST_ADDRESS                     0x06CE      /**< broadcast address register */
#define SX1268_REG_IQ_POLARITY_SETUP                     0x0736      /**< iq polarity setup register */
#define SX1268_REG_LORA_SYNC_WORD_MSB                    0x0740      /**< lora sync word msb register */
#define SX1268_REG_LORA_SYNC_WORD_LSB                    0x0741      /**< lora sync word lsb register */
#define SX1268_REG_RANDOM_NUMBER_GEN_0                   0x0819      /**< random number gen 0 register */
#define SX1268_REG_RANDOM_NUMBER_GEN_1                   0x081A      /**< random number gen 1 register */
#define SX1268_REG_RANDOM_NUMBER_GEN_2                   0x081B      /**< random number gen 2 register */
#define SX1268_REG_RANDOM_NUMBER_GEN_3                   0x081C      /**< random number gen 3 register */
#define SX1268_REG_TX_MODULATION                         0x0889      /**< tx modulation register */
#define SX1268_REG_RX_GAIN                               0x08AC      /**< rx gain register */
#define SX1268_REG_TX_CLAMP_CONFIG                       0x08D8      /**< tx clamp config register */
#define SX1268_REG_OCP_CONFIGURATION                     0x08E7      /**< ocp configuration register */
#define SX1268_REG_RTC_CONTROL                           0x0902      /**< rtc control register */
#define SX1268_REG_XTA_TRIM                              0x0911      /**< xta trim register */
#define SX1268_REG_XTB_TRIM                              0x0912      /**< xtb trim register */
#define SX1268_REG_DIO3_OUTPUT_CONTROL                   0x0920      /**< dio3 output voltage control register */
#define SX1268_REG_EVENT_MASK                            0x0944      /**< event mask register */
// </editor-fold>

sx1268_handle_t sx1268Handle; // external variable defined in sx1268.h

/* ******************************************************** Private Functions */
static uint8_t a_sx1268_spi_read(uint8_t reg, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="read bytes">
{
    if(sx1268_interface_spi_write_read(&reg, 1, pD, len)!=0) /* spi read */
        return 1;

    return 0;
} // </editor-fold>

static uint8_t a_sx1268_spi_write(uint8_t reg, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="write bytes">
{
    if(len>(INNER_BUFFER_SIZE-1)) /* check the length */
        return 2;

    memset(sx1268Handle.buf, 0, sizeof (uint8_t)*INNER_BUFFER_SIZE);
    sx1268Handle.buf[0]=reg; /* set the reg */
    memcpy(&sx1268Handle.buf[1], pD, len); /* copy the buffer */

    if(sx1268_interface_spi_write_read(sx1268Handle.buf, len+1, NULL, 0)!=0) /* spi write */
        return 1;

    return 0;
} // </editor-fold>

static uint8_t a_sx1268_spi_read_register(uint16_t reg, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="read register bytes">
{
    uint8_t reg_buf[3];

    if(len>(INNER_BUFFER_SIZE-1)) /* check the length */
        return 2;

    memset(sx1268Handle.buf, 0, sizeof (uint8_t)*INNER_BUFFER_SIZE);
    reg_buf[0]=SX1268_COMMAND_READ_REGISTER; /* set the command */
    reg_buf[1]=(reg>>8) & 0xFF;
    reg_buf[2]=(reg>>0) & 0xFF;

    if(sx1268_interface_spi_write_read((uint8_t *) reg_buf, 3, sx1268Handle.buf, len+1)!=0) /* spi read */
        return 1;

    memcpy(pD, sx1268Handle.buf+1, len); /* copy the data */
    return 0;
} // </editor-fold>

static uint8_t a_sx1268_spi_write_register(uint16_t reg, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="write register bytes">
{
    if(len>(INNER_BUFFER_SIZE-3)) /* check the length */
        return 2;

    memset(sx1268Handle.buf, 0, sizeof (uint8_t)*INNER_BUFFER_SIZE);
    sx1268Handle.buf[0]=SX1268_COMMAND_WRITE_REGISTER; /* set the command */
    sx1268Handle.buf[1]=(reg>>8) & 0xFF; /* set reg msb */
    sx1268Handle.buf[2]=(reg>>0) & 0xFF; /* set reg lsb */
    memcpy(&sx1268Handle.buf[3], pD, len); /* copy the buffer */

    if(sx1268_interface_spi_write_read(sx1268Handle.buf, len+3, NULL, 0)!=0) /* spi write */
        return 1;

    return 0;
} // </editor-fold>

static uint8_t a_sx1268_spi_write_buffer(uint8_t offset, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="write buffer">
{
    if(len>(INNER_BUFFER_SIZE-2)) /* check the length */
        return 2;

    memset(sx1268Handle.buf, 0, sizeof (uint8_t)*INNER_BUFFER_SIZE);
    sx1268Handle.buf[0]=SX1268_COMMAND_WRITE_BUFFER; /* set the command */
    sx1268Handle.buf[1]=offset; /* set reg msb */
    memcpy(&sx1268Handle.buf[2], pD, len); /* copy the buffer */

    if(sx1268_interface_spi_write_read(sx1268Handle.buf, len+2, NULL, 0)!=0) /* spi write */
        return 1;

    return 0;
} // </editor-fold>

static uint8_t a_sx1268_spi_read_buffer(uint8_t offset, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="read buffer">
{
    uint8_t reg_buf[2];

    if(len>(INNER_BUFFER_SIZE-1)) /* check the length */
        return 2;

    memset(sx1268Handle.buf, 0, sizeof (uint8_t)*INNER_BUFFER_SIZE);
    reg_buf[0]=SX1268_COMMAND_READ_BUFFER; /* set the command */
    reg_buf[1]=offset;

    if(sx1268_interface_spi_write_read((uint8_t *) reg_buf, 2, sx1268Handle.buf, len+1)!=0) /* spi write */
        return 1;

    memcpy(pD, sx1268Handle.buf+1, len); /* copy the data */
    return 0;
} // </editor-fold>

static uint8_t a_sx1268_check_busy(void) // <editor-fold defaultstate="collapsed" desc="check command busy">
{
    uint8_t level;
    uint8_t timeout;

    timeout=100; /* set max 100 */

    while(1) /* loop */
    {
        level=sx1268_interface_busy_gpio_read();

        if(level==0) /* check level */
        {
            return 0;
        }
        else if((level==1) && (timeout!=0)) /* check level and timeout */
        {
            sx1268_interface_delay_ms(10); /* delay 10 ms */
            timeout--; /* timeout-- */

            if(timeout!=0) /* check timeout */
            {
                continue; /* continue */
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
} // </editor-fold>

static uint16_t a_sx1268_make16(const uint8_t *pD) // <editor-fold defaultstate="collapsed" desc="convert 8-bit array to 16-bit integer">
{
    uint16_t val=*pD++;

    val<<=8;
    val|=*pD;

    return val;
} // </editor-fold>

static uint32_t a_sx1268_make32(const uint8_t *pD) // <editor-fold defaultstate="collapsed" desc="convert 8-bit array to 32-bit integer">
{
    uint32_t val=*pD++;

    val<<=8;
    val|=*pD++;
    val<<=8;
    val|=*pD++;
    val<<=8;
    val|=*pD++;

    return val;
} // </editor-fold>

/* ********************************************************* Public Functions */
bool sx1268_is_inited(void) // <editor-fold defaultstate="collapsed" desc="check handle initialization">
{
    if(sx1268Handle.inited==1)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_irq_handler(void) // <editor-fold defaultstate="collapsed" desc="irq handler">
{
    uint8_t buf[3];
    uint16_t status;

    memset(buf, 0, sizeof (uint8_t) * 3);

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_IRQ_STATUS, buf, 3)!=0)
        return 1;

    //status=((uint16_t) buf[1]<<8)|buf[2];
    status=a_sx1268_make16(&buf[1]);

    if(a_sx1268_spi_write(SX1268_COMMAND_CLEAR_IRQ_STATUS, (uint8_t *)&buf[1], 2)!=0) /* clear irq status failed */
        return 1;

    sx1268Handle.crc_error=0; /* clear crc error */

    if((status&SX1268_IRQ_PREAMBLE_DETECTED)!=0) /* if preamble detected */
        sx1268_interface_receive_callback(SX1268_IRQ_PREAMBLE_DETECTED, NULL, 0);

    if((status&SX1268_IRQ_SYNC_WORD_VALID)!=0) /* if valid sync word detected */
        sx1268_interface_receive_callback(SX1268_IRQ_SYNC_WORD_VALID, NULL, 0);

    if((status&SX1268_IRQ_HEADER_VALID)!=0) /* if valid header */
        sx1268_interface_receive_callback(SX1268_IRQ_HEADER_VALID, NULL, 0);

    if((status&SX1268_IRQ_HEADER_ERR)!=0) /* if header error */
        sx1268_interface_receive_callback(SX1268_IRQ_HEADER_ERR, NULL, 0);

    if((status&SX1268_IRQ_CRC_ERR)!=0) /* if crc error */
    {
        sx1268_interface_receive_callback(SX1268_IRQ_CRC_ERR, NULL, 0);
        sx1268Handle.crc_error=1; /* set crc error */
    }

    if((status&SX1268_IRQ_CAD_DONE)!=0) /* if cad done */
    {
        sx1268_interface_receive_callback(SX1268_IRQ_CAD_DONE, NULL, 0);
        sx1268Handle.cad_done=1; /* set cad done */
    }

    if((status&SX1268_IRQ_CAD_DETECTED)!=0) /* if cad detected */
    {
        sx1268_interface_receive_callback(SX1268_IRQ_CAD_DETECTED, NULL, 0);
        sx1268Handle.cad_detected=1; /* set detected */
    }

    if((status&SX1268_IRQ_TIMEOUT)!=0)
    {
        uint8_t ctrl;
        uint8_t mask;

        ctrl=0x00;

        if(a_sx1268_spi_write_register(SX1268_REG_DIO3_OUTPUT_CONTROL, &ctrl, 1)!=0)
            return 1;

        if(a_sx1268_spi_read_register(SX1268_REG_EVENT_MASK, &mask, 1)!=0)
            return 1;

        mask|=0x02;

        if(a_sx1268_spi_write_register(SX1268_REG_EVENT_MASK, &mask, 1)!=0)
            return 1;

        sx1268_interface_receive_callback(SX1268_IRQ_TIMEOUT, NULL, 0);
        sx1268Handle.timeout=1; /* flag timeout */
    }

    if((status&SX1268_IRQ_TX_DONE)!=0) /* if tx done */
    {
        sx1268_interface_receive_callback(SX1268_IRQ_TX_DONE, NULL, 0);
        sx1268Handle.tx_done=1; /* flag tx done */
    }

    if((status&SX1268_IRQ_RX_DONE)!=0) /* if rx done */
    {
        uint8_t payload_length_rx;
        uint8_t rx_start_buffer_pointer;

        memset(buf, 0, sizeof (uint8_t) * 3);

        if(a_sx1268_spi_read(SX1268_COMMAND_GET_RX_BUFFER_STATUS, buf, 3)!=0) /* get rx buffer status failed */
            return 1;

        payload_length_rx=buf[1];
        rx_start_buffer_pointer=buf[2];

        if(a_sx1268_spi_read_buffer(rx_start_buffer_pointer, sx1268Handle.receive_buf, payload_length_rx)!=0) /* read buffer failed */
            return 1;

        if(sx1268Handle.crc_error==0) /* check crc error */
            sx1268_interface_receive_callback(SX1268_IRQ_RX_DONE, sx1268Handle.receive_buf, payload_length_rx);
        else
            sx1268_interface_receive_callback(SX1268_IRQ_RX_DONE, NULL, 0);
    }

    return 0;
} // </editor-fold>

uint8_t sx1268_init(void) // <editor-fold defaultstate="collapsed" desc="initialize the chip">
{
    uint8_t buf[1];
    uint8_t error_code=0;

    while(1)
    {
        if(sx1268_interface_spi_init()!=0) /* spi initialization */
        {
            error_code=1;
            break;
        }

        sx1268_interface_reset_gpio_write(1); /* set high */
        sx1268_interface_delay_ms(5); /* delay 5 ms */
        sx1268_interface_reset_gpio_write(0); /* set low */
        sx1268_interface_delay_ms(10); /* delay 10 ms */
        sx1268_interface_reset_gpio_write(1); /* set high */
        sx1268_interface_delay_ms(5); /* delay 5 ms */

        if(a_sx1268_spi_read(SX1268_COMMAND_GET_STATUS, buf, 1)!=0)
        {
            error_code=6;
            break;
        }

        buf[0]=0x00;

        if(a_sx1268_spi_write(SX1268_COMMAND_SET_STANDBY, buf, 1)!=0)
        {
            error_code=6;
            break;
        }

        sx1268Handle.inited=1; /* flag finish initialization */
        break;
    }

    if(error_code>0)
    {
        sx1268_interface_spi_deinit(); /* spi deinit */
        sx1268_interface_reset_gpio_write(0); /* set low */
    }

    return error_code;
} // </editor-fold>

void sx1268_deinit(void) // <editor-fold defaultstate="collapsed" desc="close the chip">
{
    uint8_t prev;

    sx1268Handle.inited=1;

    if(a_sx1268_check_busy()!=0)
        goto EXIT;

    prev=0x00; /* set power down */
    a_sx1268_spi_write(SX1268_COMMAND_SET_SLEEP, &prev, 1);

EXIT:
    sx1268_interface_spi_deinit(); /* spi deinit */
    sx1268_interface_reset_gpio_write(0); /* set low */
} // </editor-fold>

uint8_t sx1268_single_receive(double us) // <editor-fold defaultstate="collapsed" desc="enter to the single receive mode">
{
    uint8_t buf[3];
    uint16_t clear_irq_param;
    uint32_t timeout;

    if(a_sx1268_check_busy()!=0)
        return 4;

    clear_irq_param=0x03FF;
    buf[0]=(clear_irq_param>>8) & 0xFF;
    buf[1]=(clear_irq_param>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_CLEAR_IRQ_STATUS, buf, 2)!=0) /* clear irq status failed */
        return 1;

    if(a_sx1268_check_busy()!=0)
        return 4;

    timeout=(uint32_t) (us/15.625);
    buf[0]=(timeout>>16) & 0xFF; /* bit 23 : 16 */
    buf[1]=(timeout>>8) & 0xFF; /* bit 15 : 8 */
    buf[2]=(timeout>>0) & 0xFF; /* bit 7 : 0 */

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_RX, buf, 3)!=0) /* set rx failed */
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_continuous_receive(void) // <editor-fold defaultstate="collapsed" desc="enter to the continuous receive mode">
{
    uint16_t clear_irq_param;
    uint8_t buf[3];

    if(a_sx1268_check_busy()!=0)
        return 4;

    clear_irq_param=0x03FF;
    buf[0]=(clear_irq_param>>8) & 0xFF;
    buf[1]=(clear_irq_param>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_CLEAR_IRQ_STATUS, buf, 2)!=0) /* clear irq status failed */
        return 1;

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=0xFF; /* bit 23 : 16 */
    buf[1]=0xFF; /* bit 15 : 8 */
    buf[2]=0xFF; /* bit 7 : 0 */

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_RX, buf, 3)!=0) /* set rx failed */
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_lora_cad(sx1268_bool_t *enable) // <editor-fold defaultstate="collapsed" desc="run the cad">
{
    uint8_t res;
    uint16_t clear_irq_param;
    uint8_t buf[2];
    uint16_t timeout;

    if(a_sx1268_check_busy()!=0)
        return 4;

    clear_irq_param=0x03FF;
    buf[0]=(clear_irq_param>>8) & 0xFF;
    buf[1]=(clear_irq_param>>0) & 0xFF;
    res=a_sx1268_spi_write(SX1268_COMMAND_CLEAR_IRQ_STATUS, buf, 2);

    if(res!=0) /* clear irq status failed */
        return 1;

    if(a_sx1268_check_busy()!=0)
        return 4;

    sx1268Handle.cad_done=0; /* clear cad done */
    sx1268Handle.cad_detected=0; /* clear cad done */
    res=a_sx1268_spi_write(SX1268_COMMAND_SET_CAD, NULL, 0);

    if(res!=0) /* set cad failed */
        return 1;

    timeout=10000; /*  set timeout */

    while((timeout!=0) && (sx1268Handle.cad_done==0)) /* wait */
    {
        sx1268_interface_delay_ms(1); /* delay 1 ms */
        timeout--; /* timeout-- */
    }

    if(timeout==0) /* cad timeout */
        return 5;

    if(sx1268Handle.cad_detected==1) /* set cad */
        *enable=SX1268_BOOL_TRUE; /* enable */
    else
        *enable=SX1268_BOOL_FALSE; /* disable */

    return 0;
} // </editor-fold>

uint8_t sx1268_check_packet_error(sx1268_bool_t *enable) // <editor-fold defaultstate="collapsed" desc="check the packet error">
{
    *enable=(sx1268_bool_t) (sx1268Handle.crc_error); /* check error */

    return 0;
} // </editor-fold>

uint8_t sx1268_lora_transmit(sx1268_clock_source_t standby_src,
                             uint16_t preamble_length, sx1268_lora_header_t header_type,
                             sx1268_lora_crc_type_t crc_type, sx1268_bool_t invert_iq_enable,
                             uint8_t *pD, uint16_t len, uint32_t us) // <editor-fold defaultstate="collapsed" desc="send the lora data">
{
    uint8_t prev;
    uint8_t buf[6];
    uint16_t clear_irq_param;
    uint32_t ms;
    uint32_t reg;

    if(a_sx1268_check_busy()!=0)
        return 4;

    clear_irq_param=0x03FF;
    buf[0]=(clear_irq_param>>8) & 0xFF;
    buf[1]=(clear_irq_param>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_CLEAR_IRQ_STATUS, buf, 2)!=0) /* clear irq status failed */
        return 1;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) standby_src;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_STANDBY, &prev, 1)!=0) /* set standby failed */
        return 1;

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(preamble_length>>8) & 0xFF;
    buf[1]=(preamble_length>>0) & 0xFF;
    buf[2]=(uint8_t) header_type;
    buf[3]=(uint8_t) len;
    buf[4]=(uint8_t) crc_type;
    buf[5]=(uint8_t) invert_iq_enable;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_PACKET_PARAMS, buf, 6)!=0) /* set lora modulation params failed */
        return 1;

    if(invert_iq_enable==SX1268_BOOL_FALSE) /* not invert iq */
    {
        uint8_t setup;

        if(a_sx1268_check_busy()!=0)
            return 4;

        if(a_sx1268_spi_read_register(SX1268_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1)!=0)
            return 1;

        setup|=(1<<2); /* set bit 2 */

        if(a_sx1268_spi_write_register(SX1268_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1)!=0)
            return 1;
    }
    else
    {
        uint8_t setup;

        if(a_sx1268_check_busy()!=0)
            return 4;

        if(a_sx1268_spi_read_register(SX1268_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1)!=0)
            return 1;

        setup&= ~(1<<2); /* clear bit 2 */

        if(a_sx1268_spi_write_register(SX1268_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1)!=0)
            return 1;
    }

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_buffer(0x00, pD, len)!=0)
        return 1;

    if(a_sx1268_check_busy()!=0)
        return 4;

    reg=(uint32_t) ((float) us/15.625); /* convert the timeout */
    buf[0]=(reg>>16) & 0xFF; /* bit 23 : 16 */
    buf[1]=(reg>>8) & 0xFF; /* bit 15 : 8 */
    buf[2]=(reg>>0) & 0xFF; /* bit 7 : 0 */
    sx1268Handle.tx_done=0; /* flag 0 */
    sx1268Handle.timeout=0; /* flag 0 */

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_TX, buf, 3)!=0) /* set tx failed */
        return 1;

    ms=us/1000+10000; /* set timeout */

    while((ms!=0) && (sx1268Handle.tx_done==0) && (sx1268Handle.timeout==0)) /* check timeout */
    {
        sx1268_interface_delay_ms(1); /* delay 1 ms */
        ms--; /* ms-- */
    }

    if((ms!=0) && (sx1268Handle.tx_done==1))
        return 0;

    if((ms==0) && (sx1268Handle.tx_done==0)) /* send timeout */
        return 5;

    if(sx1268Handle.timeout==1) /* irq timeout */
        return 6;

    /* unknown result */
    return 7;
} // </editor-fold>

uint8_t sx1268_write_register(uint16_t reg, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="write the register">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(reg, pD, len)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_read_register(uint16_t reg, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="read the register">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(reg, pD, len)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_write_buffer(uint8_t offset, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="write the buffer">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_buffer(offset, pD, len)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_read_buffer(uint8_t offset, uint8_t *pD, uint16_t len) // <editor-fold defaultstate="collapsed" desc="read the buffer">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_buffer(offset, pD, len)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_sleep(sx1268_start_mode_t mode, sx1268_bool_t rtc_wake_up_enable) // <editor-fold defaultstate="collapsed" desc="enter to the sleep mode">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) ((mode<<2)|(rtc_wake_up_enable<<0));

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_SLEEP, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_standby(sx1268_clock_source_t src) // <editor-fold defaultstate="collapsed" desc="enter to the standby mode">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) src;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_STANDBY, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_frequency_synthesis(void) // <editor-fold defaultstate="collapsed" desc="enter to the frequency synthesis mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_FS, NULL, 0)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_tx(uint32_t timeout) // <editor-fold defaultstate="collapsed" desc="enter to the tx mode">
{
    uint8_t buf[3];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(timeout>>16) & 0xFF; /* bit 23 : 16 */
    buf[1]=(timeout>>8) & 0xFF; /* bit 15 : 8 */
    buf[2]=(timeout>>0) & 0xFF; /* bit 7 : 0 */

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_TX, buf, 3)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_rx(uint32_t timeout) // <editor-fold defaultstate="collapsed" desc="enter to the rx mode">
{
    uint8_t buf[3];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(timeout>>16) & 0xFF; /* bit 23 : 16 */
    buf[1]=(timeout>>8) & 0xFF; /* bit 15 : 8 */
    buf[2]=(timeout>>0) & 0xFF; /* bit 7 : 0 */

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_RX, buf, 3)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_timeout_convert_to_register(double us, uint32_t *reg) // <editor-fold defaultstate="collapsed" desc="convert the timeout to the register raw data">
{
    *reg=(uint32_t) (us/15.625);

    return 0;
} // </editor-fold>

uint8_t sx1268_timeout_convert_to_data(uint32_t reg, double *us) // <editor-fold defaultstate="collapsed" desc="convert the register raw data to the timeout">
{
    *us=(double) (reg) * 15.625;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_stop_timer_on_preamble(sx1268_bool_t enable) // <editor-fold defaultstate="collapsed" desc="stop timer on preamble">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) enable;

    if(a_sx1268_spi_write(SX1268_COMMAND_STOP_TIMER_ON_PREAMBLE, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_rx_duty_cycle(uint32_t rx_period, uint32_t sleep_period) // <editor-fold defaultstate="collapsed" desc="set the rx duty cycle">
{
    uint8_t buf[6];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(rx_period>>16) & 0xFF; /* bit 23 : 16 */
    buf[1]=(rx_period>>8) & 0xFF; /* bit 15 : 8 */
    buf[2]=(rx_period>>0) & 0xFF; /* bit 7 : 0 */
    buf[3]=(sleep_period>>16) & 0xFF; /* bit 23 : 16 */
    buf[4]=(sleep_period>>8) & 0xFF; /* bit 15 : 8 */
    buf[5]=(sleep_period>>0) & 0xFF; /* bit 7 : 0 */

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_RX_DUTY_CYCLE, buf, 6)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_cad(void) // <editor-fold defaultstate="collapsed" desc="run the cad">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_CAD, NULL, 0)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_tx_continuous_wave(void) // <editor-fold defaultstate="collapsed" desc="enter to the tx continuous wave mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_TX_CONTINUOUS_WAVE, NULL, 0)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_tx_infinite_preamble(void) // <editor-fold defaultstate="collapsed" desc="enter to the tx infinite preamble mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_TX_INFINITE_PREAMBLE, NULL, 0)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_regulator_mode(sx1268_regulator_mode_t mode) // <editor-fold defaultstate="collapsed" desc="set the regulator_mode">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) mode;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_REGULATOR_MODE, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_calibration(uint8_t settings) // <editor-fold defaultstate="collapsed" desc="set the calibration settings">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=settings;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_CALIBRATE, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_calibration_image(uint8_t freq1, uint8_t freq2) // <editor-fold defaultstate="collapsed" desc="set the calibration image frequency">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=freq1;
    buf[1]=freq2;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_CALIBRATE_IMAGE, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_pa_config(uint8_t pa_duty_cycle, uint8_t hp_max) // <editor-fold defaultstate="collapsed" desc="set the pa config">
{
    uint8_t buf[4];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=pa_duty_cycle;
    buf[1]=hp_max;
    buf[2]=0x00;
    buf[3]=0x01;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_PA_CONFIG, buf, 4)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_rx_tx_fallback_mode(sx1268_rx_tx_fallback_mode_t mode) // <editor-fold defaultstate="collapsed" desc="set the rx tx fallback mode">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) mode;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_RX_TX_FALLBACK_MODE, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_dio_irq_params(uint16_t irq_mask, uint16_t dio1_mask,
                                  uint16_t dio2_mask, uint16_t dio3_mask) // <editor-fold defaultstate="collapsed" desc="set the dio irq params">
{
    uint8_t buf[8];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(irq_mask>>8) & 0xFF;
    buf[1]=(irq_mask>>0) & 0xFF;
    buf[2]=(dio1_mask>>8) & 0xFF;
    buf[3]=(dio1_mask>>0) & 0xFF;
    buf[4]=(dio2_mask>>8) & 0xFF;
    buf[5]=(dio2_mask>>0) & 0xFF;
    buf[6]=(dio3_mask>>8) & 0xFF;
    buf[7]=(dio3_mask>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_DIO_IRQ_PARAMS, buf, 8)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_irq_status(uint16_t *status) // <editor-fold defaultstate="collapsed" desc="get the irq status">
{
    uint8_t buf[3];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_IRQ_STATUS, buf, 3)!=0)
        return 1;

    //*status=((uint16_t) buf[1]<<8)|buf[2];
    *status=a_sx1268_make16(&buf[1]);

    return 0;
} // </editor-fold>

uint8_t sx1268_clear_irq_status(uint16_t clear_irq_param) // <editor-fold defaultstate="collapsed" desc="clear the irq status">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(clear_irq_param>>8) & 0xFF;
    buf[1]=(clear_irq_param>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_CLEAR_IRQ_STATUS, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_dio2_as_rf_switch_ctrl(sx1268_bool_t enable) // <editor-fold defaultstate="collapsed" desc="set dio2 as rf switch ctrl">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) enable;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_DIO2_AS_RF_SWITCH_CTRL, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_dio3_as_tcxo_ctrl(sx1268_tcxo_voltage_t voltage, uint32_t delay) // <editor-fold defaultstate="collapsed" desc="set dio3 as tcxo ctrl">
{
    uint8_t buf[4];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(uint8_t) voltage;
    buf[1]=(delay>>16) & 0xFF;
    buf[2]=(delay>>8) & 0xFF;
    buf[3]=(delay>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_DIO3_AS_TCXO_CTRL, buf, 4)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_frequency_convert_to_register(uint32_t freq, uint32_t *reg) // <editor-fold defaultstate="collapsed" desc="convert the frequency to the register raw data">
{
    *reg=(uint32_t) (powf(2.0f, 25.0f)/(32.0f*powf(10.0f, 6.0f))*(float) freq);

    return 0;
} // </editor-fold>

uint8_t sx1268_frequency_convert_to_data(uint32_t reg, uint32_t *freq) // <editor-fold defaultstate="collapsed" desc="convert the register raw data to the frequency">
{
    *freq=(uint32_t) (32.0f*powf(10.0f, 6.0f)/powf(2.0f, 25.0f)*(float) reg);

    return 0;
} // </editor-fold>

uint8_t sx1268_set_rf_frequency(uint32_t reg) // <editor-fold defaultstate="collapsed" desc="set the rf frequency">
{
    uint8_t buf[4];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(reg>>24) & 0xFF;
    buf[1]=(reg>>16) & 0xFF;
    buf[2]=(reg>>8) & 0xFF;
    buf[3]=(reg>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_RF_FREQUENCY, buf, 4)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_packet_type(sx1268_packet_type_t type) // <editor-fold defaultstate="collapsed" desc="set the packet type">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=(uint8_t) type;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_PACKET_TYPE, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_packet_type(sx1268_packet_type_t *type) // <editor-fold defaultstate="collapsed" desc="get the packet type">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_PACKET_TYPE, buf, 2)!=0)
        return 1;

    *type=(sx1268_packet_type_t) (buf[1]);

    return 0;
} // </editor-fold>

uint8_t sx1268_set_tx_params(int8_t dbm, sx1268_ramp_time_t t) // <editor-fold defaultstate="collapsed" desc="set the tx params">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(uint8_t) dbm;
    buf[1]=(uint8_t) t;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_TX_PARAMS, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_gfsk_modulation_params(uint32_t br, sx1268_gfsk_pulse_shape_t shape,
                                          sx1268_gfsk_bandwidth_t bw, uint32_t fdev) // <editor-fold defaultstate="collapsed" desc="set the modulation params in GFSK mode">
{
    uint8_t buf[8];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(br>>16) & 0xFF;
    buf[1]=(br>>8) & 0xFF;
    buf[2]=(br>>0) & 0xFF;
    buf[3]=(uint8_t) shape;
    buf[4]=(uint8_t) bw;
    buf[5]=(fdev>>16) & 0xFF;
    buf[6]=(fdev>>8) & 0xFF;
    buf[7]=(fdev>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_MODULATION_PARAMS, buf, 8)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_gfsk_bit_rate_convert_to_register(uint32_t br, uint32_t *reg) // <editor-fold defaultstate="collapsed" desc="convert the bit rate to the register raw data">
{
    *reg=(uint32_t) (32*(32*powf(10.f, 6.0f)))/br;

    return 0;
} // </editor-fold>

uint8_t sx1268_gfsk_bit_rate_convert_to_data(uint32_t reg, uint32_t *br) // <editor-fold defaultstate="collapsed" desc="convert the register raw data to the bit rate">
{
    *br=(uint32_t) (32.0f*32.0f*powf(10.0f, 6.0f)/(float) reg);

    return 0;
} // </editor-fold>

uint8_t sx1268_gfsk_frequency_deviation_convert_to_register(uint32_t freq, uint32_t *reg) // <editor-fold defaultstate="collapsed" desc="convert the frequency deviation to the register raw data">
{
    *reg=(uint32_t) (powf(2.0f, 25.0f)*(float) freq/(32.0f*powf(10.0f, 6.0f)));

    return 0;
} // </editor-fold>

uint8_t sx1268_gfsk_frequency_deviation_convert_to_data(uint32_t reg, uint32_t *freq) // <editor-fold defaultstate="collapsed" desc="convert the register raw data to the frequency deviation">
{
    *freq=(uint32_t) (32.0f*powf(10.0f, 6.0f)/powf(2.0f, 25.0f)*(float) reg);

    return 0;
} // </editor-fold>

uint8_t sx1268_set_lora_modulation_params(sx1268_lora_sf_t sf, sx1268_lora_bandwidth_t bw,
                                          sx1268_lora_cr_t cr, sx1268_bool_t low_data_rate_optimize_enable) // <editor-fold defaultstate="collapsed" desc="set the modulation params in LoRa mode">
{
    uint8_t buf[4];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(uint8_t) sf;
    buf[1]=(uint8_t) bw;
    buf[2]=(uint8_t) cr;
    buf[3]=(uint8_t) low_data_rate_optimize_enable;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_MODULATION_PARAMS, buf, 4)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_gfsk_packet_params(uint16_t preamble_length,
                                      sx1268_gfsk_preamble_detector_length_t detector_length,
                                      uint8_t sync_word_length, sx1268_gfsk_addr_filter_t filter,
                                      sx1268_gfsk_packet_type_t packet_type, uint8_t payload_length,
                                      sx1268_gfsk_crc_type_t crc_type, sx1268_bool_t whitening_enable) // <editor-fold defaultstate="collapsed" desc="set the packet params in GFSK mode">
{
    uint8_t buf[9];

    if(sync_word_length>0x40) /* check sync word is over length */
        return 5;

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(preamble_length>>8) & 0xFF;
    buf[1]=(preamble_length>>0) & 0xFF;
    buf[2]=(uint8_t) detector_length;
    buf[3]=sync_word_length;
    buf[4]=(uint8_t) filter;
    buf[5]=(uint8_t) packet_type;
    buf[6]=payload_length;
    buf[7]=(uint8_t) crc_type;
    buf[8]=(uint8_t) whitening_enable;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_PACKET_PARAMS, buf, 9)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_lora_packet_params(uint16_t preamble_length,
                                      sx1268_lora_header_t header_type, uint8_t payload_length,
                                      sx1268_lora_crc_type_t crc_type, sx1268_bool_t invert_iq_enable) // <editor-fold defaultstate="collapsed" desc="set the packet params in LoRa mode">
{
    uint8_t buf[6];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(preamble_length>>8) & 0xFF;
    buf[1]=(preamble_length>>0) & 0xFF;
    buf[2]=(uint8_t) header_type;
    buf[3]=payload_length;
    buf[4]=(uint8_t) crc_type;
    buf[5]=(uint8_t) invert_iq_enable;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_PACKET_PARAMS, buf, 6)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_cad_params(sx1268_lora_cad_symbol_num_t num,
                              uint8_t cad_det_peak, uint8_t cad_det_min, sx1268_lora_cad_exit_mode_t mode,
                              uint32_t timeout) // <editor-fold defaultstate="collapsed" desc="set the cad params">
{
    uint8_t buf[7];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(uint8_t) num;
    buf[1]=cad_det_peak;
    buf[2]=cad_det_min;
    buf[3]=(uint8_t) mode;
    buf[4]=(timeout>>16) & 0xFF;
    buf[5]=(timeout>>8) & 0xFF;
    buf[6]=(timeout>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_CAD_PARAMS, buf, 7)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_buffer_base_address(uint8_t tx_base_addr, uint8_t rx_base_addr) // <editor-fold defaultstate="collapsed" desc="set the buffer base address">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=tx_base_addr;
    buf[1]=rx_base_addr;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_BUFFER_BASE_ADDRESS, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_lora_symb_num_timeout(uint8_t symb_num) // <editor-fold defaultstate="collapsed" desc="set the lora symbol number timeout">
{
    uint8_t prev;

    if(a_sx1268_check_busy()!=0)
        return 4;

    prev=symb_num;

    if(a_sx1268_spi_write(SX1268_COMMAND_SET_LORA_SYMB_NUM_TIMEOUT, &prev, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_status(uint8_t *status) // <editor-fold defaultstate="collapsed" desc="get the status">
{
    uint8_t buf[1];

    if(a_sx1268_check_busy()!=0)
        return 4;

    memset(buf, 0, sizeof (uint8_t) * 1);

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_STATUS, buf, 1)!=0)
        return 1;

    *status=buf[0];

    return 0;
} // </editor-fold>

uint8_t sx1268_get_rx_buffer_status(uint8_t *payload_length_rx, uint8_t *rx_start_buffer_pointer) // <editor-fold defaultstate="collapsed" desc="get the rx buffer status">
{
    uint8_t buf[3];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_RX_BUFFER_STATUS, buf, 3)!=0)
        return 1;

    *payload_length_rx=buf[1];
    *rx_start_buffer_pointer=buf[2];

    return 0;
} // </editor-fold>

uint8_t sx1268_get_gfsk_packet_status(uint8_t *rx_status, uint8_t *rssi_sync_raw,
                                      uint8_t *rssi_avg_raw, float *rssi_sync, float *rssi_avg) // <editor-fold defaultstate="collapsed" desc="get the packet status in GFSK mode">
{
    uint8_t buf[4];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_PACKET_STATUS, buf, 4)!=0)
        return 1;

    *rx_status=buf[1];
    *rssi_sync_raw=buf[2];
    *rssi_avg_raw=buf[3];
    *rssi_sync= -(float) (*rssi_sync_raw)/2.0f;
    *rssi_avg= -(float) (*rssi_avg_raw)/2.0f;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_lora_packet_status(uint8_t *rssi_pkt_raw, uint8_t *snr_pkt_raw,
                                      uint8_t *signal_rssi_pkt_raw, float *rssi_pkt, float *snr_pkt, float *signal_rssi_pkt) // <editor-fold defaultstate="collapsed" desc="get the packet status in LoRa mode">
{
    uint8_t buf[4];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_PACKET_STATUS, buf, 4)!=0)
        return 1;

    *rssi_pkt_raw=buf[1];
    *snr_pkt_raw=buf[2];
    *signal_rssi_pkt_raw=buf[3];
    *rssi_pkt= -(float) (*rssi_pkt_raw)/2.0f;
    *snr_pkt=(float) (*snr_pkt_raw)/4.0f;
    *signal_rssi_pkt= -(float) (*signal_rssi_pkt_raw)/2.0f;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_instantaneous_rssi(uint8_t *rssi_inst_raw, float *rssi_inst) // <editor-fold defaultstate="collapsed" desc="get the instantaneous rssi">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_RSSI_LNST, buf, 2)!=0)
        return 1;

    *rssi_inst_raw=buf[1];
    *rssi_inst= -(float) (*rssi_inst_raw)/2.0f;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_stats(uint16_t *pkt_received, uint16_t *pkt_crc_error, uint16_t *pkt_length_header_error) // <editor-fold defaultstate="collapsed" desc="get the stats">
{
    uint8_t buf[7];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_STATS, buf, 7)!=0)
        return 1;

    //*pkt_received=(uint16_t) (((uint16_t) buf[1]<<8)|buf[2]); 
    //*pkt_crc_error=(uint16_t) (((uint16_t) buf[3]<<8)|buf[4]); 
    //*pkt_length_header_error=(uint16_t) (((uint16_t) buf[5]<<8)|buf[6]); 
    *pkt_received=a_sx1268_make16(&buf[1]);
    *pkt_crc_error=a_sx1268_make16(&buf[3]);
    *pkt_length_header_error=a_sx1268_make16(&buf[5]);

    return 0;
} // </editor-fold>

uint8_t sx1268_reset_stats(uint16_t pkt_received, uint16_t pkt_crc_error, uint16_t pkt_length_header_error) // <editor-fold defaultstate="collapsed" desc="reset the stats">
{
    uint8_t buf[6];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(pkt_received>>8) & 0xFF;
    buf[1]=(pkt_received>>0) & 0xFF;
    buf[2]=(pkt_crc_error>>8) & 0xFF;
    buf[3]=(pkt_crc_error>>0) & 0xFF;
    buf[4]=(pkt_length_header_error>>8) & 0xFF;
    buf[5]=(pkt_length_header_error>>0) & 0xFF;

    if(a_sx1268_spi_write(SX1268_COMMAND_RESET_STATS, buf, 6)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_device_errors(uint16_t *op_error) // <editor-fold defaultstate="collapsed" desc="get the device errors">
{
    uint8_t buf[3];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read(SX1268_COMMAND_GET_DEVICE_ERRORS, buf, 3)!=0)
        return 1;

    //*op_error=(uint16_t) (((uint16_t) buf[1]<<8)|buf[2]); 
    *op_error=a_sx1268_make16(&buf[1]);

    return 0;
} // </editor-fold>

uint8_t sx1268_clear_device_errors(void) // <editor-fold defaultstate="collapsed" desc="clear the device errors">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=0x00;
    buf[1]=0x00;

    if(a_sx1268_spi_write(SX1268_COMMAND_CLEAR_DEVICE_ERRORS, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_fsk_whitening_initial_value(uint16_t value) // <editor-fold defaultstate="collapsed" desc="set the whitening initial value in FSK mode">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(value>>8) & 0xFF;
    buf[1]=(value>>0) & 0xFF;

    if(a_sx1268_spi_write_register(SX1268_REG_WHITENING_INIT_VALUE_MSB, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_fsk_whitening_initial_value(uint16_t *value) // <editor-fold defaultstate="collapsed" desc="get the whitening initial value in FSK mode">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_WHITENING_INIT_VALUE_MSB, buf, 2)!=0)
        return 1;

    //*value=(uint16_t) ((uint16_t) buf[0]<<8|buf[1]); 
    *value=a_sx1268_make16(&buf[0]);

    return 0;
} // </editor-fold>

uint8_t sx1268_set_fsk_crc_initical_value(uint16_t value) // <editor-fold defaultstate="collapsed" desc="set the crc initial value in FSK mode">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(value>>8) & 0xFF;
    buf[1]=(value>>0) & 0xFF;

    if(a_sx1268_spi_write_register(SX1268_REG_CRC_INIT_VALUE_MSB, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_fsk_crc_initical_value(uint16_t *value) // <editor-fold defaultstate="collapsed" desc="get the crc initical value in FSK mode">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_CRC_INIT_VALUE_MSB, buf, 2)!=0)
        return 1;

    //*value=(uint16_t) ((uint16_t) buf[0]<<8|buf[1]);
    *value=a_sx1268_make16(&buf[0]);

    return 0;
} // </editor-fold>

uint8_t sx1268_set_fsk_crc_polynomial_value(uint16_t value) // <editor-fold defaultstate="collapsed" desc="set the crc polynomial value in FSK mode">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(value>>8) & 0xFF;
    buf[1]=(value>>0) & 0xFF;

    if(a_sx1268_spi_write_register(SX1268_REG_CRC_POLYNOMIAL_VALUE_MSB, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_fsk_crc_polynomial_value(uint16_t *value) // <editor-fold defaultstate="collapsed" desc="get the crc polynomial value in FSK mode">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_CRC_POLYNOMIAL_VALUE_MSB, buf, 2)!=0)
        return 1;

    //*value=(uint16_t) ((uint16_t) buf[0]<<8|buf[1]);
    *value=a_sx1268_make16(&buf[0]);

    return 0;
} // </editor-fold>

uint8_t sx1268_set_fsk_sync_word(uint8_t sync_word[8]) // <editor-fold defaultstate="collapsed" desc="set the sync word in FSK mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_SYNC_WORD_0, (uint8_t *) sync_word, 8)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_fsk_sync_word(uint8_t sync_word[8]) // <editor-fold defaultstate="collapsed" desc="get the sync word in FSK mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_SYNC_WORD_0, (uint8_t *) sync_word, 8)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_fsk_node_address(uint8_t addr) // <editor-fold defaultstate="collapsed" desc="set the node address in FSK mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_NODE_ADDRESS, (uint8_t *)&addr, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_fsk_node_address(uint8_t *addr) // <editor-fold defaultstate="collapsed" desc="get the node address in FSK mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_NODE_ADDRESS, (uint8_t *) addr, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_fsk_broadcast_address(uint8_t addr) // <editor-fold defaultstate="collapsed" desc="set the broadcast address in FSK mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_BROADCAST_ADDRESS, (uint8_t *)&addr, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_fsk_broadcast_address(uint8_t *addr) // <editor-fold defaultstate="collapsed" desc="get the broadcast address in FSK mode">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_BROADCAST_ADDRESS, (uint8_t *) addr, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_iq_polarity(uint8_t setup) // <editor-fold defaultstate="collapsed" desc="set the iq polarity">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_iq_polarity(uint8_t *setup) // <editor-fold defaultstate="collapsed" desc="get the iq polarity">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_IQ_POLARITY_SETUP, (uint8_t *) setup, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_lora_sync_word(uint16_t sync_word) // <editor-fold defaultstate="collapsed" desc="set the lora sync word">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    buf[0]=(sync_word>>8) & 0xFF;
    buf[1]=(sync_word>>0) & 0xFF;

    if(a_sx1268_spi_write_register(SX1268_REG_LORA_SYNC_WORD_MSB, buf, 2)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_lora_sync_word(uint16_t *sync_word) // <editor-fold defaultstate="collapsed" desc="get the lora sync word">
{
    uint8_t buf[2];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_LORA_SYNC_WORD_MSB, buf, 2)!=0)
        return 1;

    //*sync_word=(uint16_t) ((uint16_t) buf[0]<<8|buf[1]); 
    *sync_word=a_sx1268_make16(&buf[0]);

    return 0;
} // </editor-fold>

uint8_t sx1268_get_random_number(uint32_t *r) // <editor-fold defaultstate="collapsed" desc="get the random number">
{
    uint8_t buf[4];

    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_RANDOM_NUMBER_GEN_0, buf, 4)!=0)
        return 1;

    //*r=(uint32_t) ((uint32_t) buf[0]<<24|(uint32_t) buf[1]<<16|(uint32_t) buf[2]<<8|buf[3]);
    *r=a_sx1268_make32(&buf[0]);

    return 0;
} // </editor-fold>

uint8_t sx1268_set_tx_modulation(uint8_t modulation) // <editor-fold defaultstate="collapsed" desc="set the tx modulation">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_TX_MODULATION, &modulation, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_tx_modulation(uint8_t *modulation) // <editor-fold defaultstate="collapsed" desc="get the tx modulation">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_TX_MODULATION, (uint8_t *) modulation, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_rx_gain(uint8_t gain) // <editor-fold defaultstate="collapsed" desc="set the rx gain">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_RX_GAIN, (uint8_t *)&gain, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_rx_gain(uint8_t *gain) // <editor-fold defaultstate="collapsed" desc="get the rx gain">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_RX_GAIN, (uint8_t *) gain, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_tx_clamp_config(uint8_t config) // <editor-fold defaultstate="collapsed" desc="set the tx clamp config">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_TX_CLAMP_CONFIG, (uint8_t *)&config, 1)!=0)

        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_tx_clamp_config(uint8_t *config) // <editor-fold defaultstate="collapsed" desc="get the tx clamp config">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_TX_CLAMP_CONFIG, (uint8_t *) config, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_ocp(uint8_t ocp) // <editor-fold defaultstate="collapsed" desc="set the ocp">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_OCP_CONFIGURATION, (uint8_t *)&ocp, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_ocp(uint8_t *ocp) // <editor-fold defaultstate="collapsed" desc="get the ocp">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_OCP_CONFIGURATION, (uint8_t *) ocp, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_rtc_control(uint8_t ctrl) // <editor-fold defaultstate="collapsed" desc="set the rtc control">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_RTC_CONTROL, &ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_rtc_control(uint8_t *ctrl) // <editor-fold defaultstate="collapsed" desc="get the rtc control">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_RTC_CONTROL, ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_xta_trim(uint8_t trim) // <editor-fold defaultstate="collapsed" desc="set the xta trim">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_XTA_TRIM, (uint8_t *)&trim, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_xta_trim(uint8_t *trim) // <editor-fold defaultstate="collapsed" desc="get the xta trim">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_XTA_TRIM, (uint8_t *) trim, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_xtb_trim(uint8_t trim) // <editor-fold defaultstate="collapsed" desc="set the xtb trim">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_XTB_TRIM, (uint8_t *)&trim, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_xtb_trim(uint8_t *trim) // <editor-fold defaultstate="collapsed" desc="get the xtb trim">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_XTB_TRIM, (uint8_t *) trim, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_dio3_output_control(uint8_t ctrl) // <editor-fold defaultstate="collapsed" desc="set the dio3 output">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_DIO3_OUTPUT_CONTROL, &ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_dio3_output_control(uint8_t *ctrl) // <editor-fold defaultstate="collapsed" desc="get the dio3 output">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_DIO3_OUTPUT_CONTROL, ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_event_mask(uint8_t mask) // <editor-fold defaultstate="collapsed" desc="set the event mask">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_EVENT_MASK, &mask, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_event_mask(uint8_t *mask) // <editor-fold defaultstate="collapsed" desc="get the event mask">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_EVENT_MASK, (uint8_t *) mask, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_dio_output_enable(uint8_t enable) // <editor-fold defaultstate="collapsed" desc="set the dio output enable">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_DIOX_OUTPUT_ENABLE, (uint8_t *)&enable, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_dio_output_enable(uint8_t *enable) // <editor-fold defaultstate="collapsed" desc="get the dio output enable">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_DIOX_OUTPUT_ENABLE, (uint8_t *) enable, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_dio_input_enable(uint8_t enable) // <editor-fold defaultstate="collapsed" desc="set the dio input enable">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_DIOX_INPUT_ENABLE, (uint8_t *)&enable, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_dio_input_enable(uint8_t *enable) // <editor-fold defaultstate="collapsed" desc="get the dio input enable">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_DIOX_INPUT_ENABLE, (uint8_t *) enable, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_pull_up_control(uint8_t ctrl) // <editor-fold defaultstate="collapsed" desc="set the pull up control">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_DIOX_PULL_UP_CONTROL, &ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_pull_up_control(uint8_t *ctrl) // <editor-fold defaultstate="collapsed" desc="get the pull up control">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_DIOX_PULL_UP_CONTROL, ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_set_pull_down_control(uint8_t ctrl) // <editor-fold defaultstate="collapsed" desc="set the pull down control">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_write_register(SX1268_REG_DIOX_PULL_DOWN_CONTROL, &ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_get_pull_down_control(uint8_t *ctrl) // <editor-fold defaultstate="collapsed" desc="get the pull down control">
{
    if(a_sx1268_check_busy()!=0)
        return 4;

    if(a_sx1268_spi_read_register(SX1268_REG_DIOX_PULL_DOWN_CONTROL, ctrl, 1)!=0)
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_write_read_reg(uint8_t *in_buf, uint32_t in_len,
                              uint8_t *out_buf, uint32_t out_len) // <editor-fold defaultstate="collapsed" desc="write and read register">
{
    if(sx1268_interface_spi_write_read(in_buf, in_len, out_buf, out_len)!=0) /* spi read */
        return 1;

    return 0;
} // </editor-fold>

uint8_t sx1268_info(sx1268_info_t *info) // <editor-fold defaultstate="collapsed" desc="get chip's information">
{
    if(info==NULL) /* check handle */
        return 2;

    memset(info, 0, sizeof (sx1268_info_t)); /* initialize sx1268 info structure */
    strncpy(info->chip_name, CHIP_NAME, 8); /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 8); /* copy manufacturer name */
    strncpy(info->interface, "SPI", 4); /* copy interface name */
    info->supply_voltage_min_v=SUPPLY_VOLTAGE_MIN; /* set minimal supply voltage */
    info->supply_voltage_max_v=SUPPLY_VOLTAGE_MAX; /* set maximum supply voltage */
    info->max_current_ma=MAX_CURRENT; /* set maximum current */
    info->temperature_max=TEMPERATURE_MAX; /* set minimal temperature */
    info->temperature_min=TEMPERATURE_MIN; /* set maximum temperature */
    info->driver_version=DRIVER_VERSION; /* set driver version */

    return 0;
} // </editor-fold>
