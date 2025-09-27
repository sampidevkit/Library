#include "hardware.h"
#include "SX126x.h"

#ifndef USE_SX126X_DEBUG
#define DEBUG_H
#endif

#include "common/debug.h"

#define DEBUG_PREFIX "\r\n[SX126x] "

// Global Stuff
static uint8_t PacketParams[6];
static uint16_t txLost=0;

void __attribute__((weak)) sx126x_error(int error) // <editor-fold defaultstate="collapsed" desc="Default error indicator function">
{
    __dbsi(DEBUG_PREFIX "error=", error);

    while(1)
        system_wait();
} // </editor-fold>

static void sx126x_delay_ms(uint32_t ms) // <editor-fold defaultstate="collapsed" desc="delay in ms">
{
    uint32_t tk=sx126x_get_system_tick_ms();

    while((sx126x_get_system_tick_ms()-tk)<ms)
        system_wait();
} // </editor-fold>

static uint8_t sx126x_rfsw_getstate(void) // <editor-fold defaultstate="collapsed" desc="Get RF SW state">
{
    uint8_t state=sx126x_gpio_get_level(SX126x_TXEN);
    
    state<<=1;
    state|=1;

    return state;
} // </editor-fold>

public void LoRaInit(void) // <editor-fold defaultstate="collapsed" desc="LoRa initialize">
{
    sx126x_gpio_set_level(SX126x_NSS, HIGH);
    sx126x_gpio_set_level(SX126x_RESET, LOW);
    sx126x_gpio_set_level(SX126x_TXEN, LOW);
} // </editor-fold>

public int16_t LoRaBegin(uint16_t frequencyInMHz, int8_t txPowerInDbm,
                         int32_t tcxo_mV, bool useRegLDO) // <editor-fold defaultstate="collapsed" desc="LoRa begin">
{
    uint8_t wk[2];
    uint16_t syncWord;

    SX126x_Reset();
    SX126x_ReadRegister(SX126X_REG_LORA_SYNC_WORD_MSB, wk, 2); // 0x0740
    syncWord=wk[0]; //(wk[0]<<8)+wk[1];
    syncWord<<=8;
    syncWord+=wk[1];

    if((syncWord!=SX126X_SYNC_WORD_PUBLIC)&&(syncWord!=SX126X_SYNC_WORD_PRIVATE))
    {
        __dbs(DEBUG_PREFIX "Sync Word error, maybe no SPI connection");
        return ERR_INVALID_MODE;
    }

    //SX126x_SetStandby(SX126X_STANDBY_RC);
    SX126x_SetStandby(SX126X_STANDBY_XOSC);

    if(tcxo_mV>0)
        SetDio3AsTcxoCtrl(tcxo_mV, RADIO_TCXO_SETUP_TIME); // Configure the radio to use a TCXO controlled by DIO3

    SX126x_Calibrate(SX126X_CALIBRATE_IMAGE_ON
            |SX126X_CALIBRATE_ADC_BULK_P_ON
            |SX126X_CALIBRATE_ADC_BULK_N_ON
            |SX126X_CALIBRATE_ADC_PULSE_ON
            |SX126X_CALIBRATE_PLL_ON
            |SX126X_CALIBRATE_RC13M_ON
            |SX126X_CALIBRATE_RC64K_ON);

    if(useRegLDO)
        SX126x_SetRegulatorMode(SX126X_REGULATOR_LDO); // set regulator mode: LDO
    else
        SX126x_SetRegulatorMode(SX126X_REGULATOR_DC_DC); // set regulator mode: DC-DC

    //SX126x_SetPaConfig(0x06, 0x00, 0x01, 0x01); // PA Optimal Settings +15 dBm
    SX126x_SetPaConfig(0x04, 0x07, 0x00, 0x01); // PA Optimal Settings +22 dBm
    SX126x_SetOvercurrentProtection(140); // current max 140mA for the whole device
    //SX126x_SetPowerConfig(txPowerInDbm, SX126X_PA_RAMP_200U); //0 fuer Empfaenger
    SX126x_SetPowerConfig(txPowerInDbm, SX126X_PA_RAMP_800U); //0 fuer Empfaenger
    SX126x_SetRfFrequency(frequencyInMHz);
    SX126x_SetBufferBaseAddress(0, 0);

    return ERR_NONE;
} // </editor-fold>

void SX126x_FixInvertedIQ(uint8_t iqConfig) // <editor-fold defaultstate="collapsed" desc="Set up fixes IQ configuration for inverted IQ">
{
    // fixes IQ configuration for inverted IQ
    // see SX1262/SX1268 data sheet, chapter 15 Known Limitations, section 15.4 for details
    // When exchanging LoRa packets with inverted IQ polarity, some packet losses may be observed for longer packets.
    // Workaround: Bit 2 at address 0x0736 must be set to:
    // when using inverted IQ polarity (see the SetPacketParam(...) command)
    // when using standard IQ polarity

    // read current IQ configuration
    uint8_t iqConfigCurrent=0;

    SX126x_ReadRegister(SX126X_REG_IQ_POLARITY_SETUP, &iqConfigCurrent, 1); // 0x0736

    // set correct IQ configuration
    //if(iqConfig == SX126X_LORA_IQ_STANDARD) {
    if(iqConfig==SX126X_LORA_IQ_INVERTED)
        iqConfigCurrent&=0xFB; // using inverted IQ polarity
    else
        iqConfigCurrent|=0x04; // using standard IQ polarity

    // update with the new value
    SX126x_WriteRegister(SX126X_REG_IQ_POLARITY_SETUP, &iqConfigCurrent, 1); // 0x0736
} // </editor-fold>

public void LoRaConfig(uint8_t spreadingFactor, uint8_t bandwidth,
                       uint8_t codingRate, uint16_t preambleLength,
                       uint8_t payloadLen, bool crcOn, bool invertIrq) // <editor-fold defaultstate="collapsed" desc="LoRa configuration">
{
    uint8_t ldro=0; // LowDataRateOptimize OFF

    SX126x_SetStopRxTimerOnPreambleDetect(false);
    SX126x_SetLoRaSymbNumTimeout(0);
    SX126x_SetPacketType(SX126X_PACKET_TYPE_LORA); // SX126x.ModulationParams.PacketType : MODEM_LORA
    SX126x_SetModulationParams(spreadingFactor, bandwidth, codingRate, ldro);
    PacketParams[0]=(preambleLength>>8)&0xFF;
    PacketParams[1]=(uint8_t) preambleLength;

    if(payloadLen)
    {
        PacketParams[2]=0x01; // Fixed length packet (implicit header)
        PacketParams[3]=payloadLen;
    }
    else
    {
        PacketParams[2]=0x00; // Variable length packet (explicit header)
        PacketParams[3]=0xFF;
    }

    if(crcOn)
        PacketParams[4]=SX126X_LORA_CRC_ON;
    else
        PacketParams[4]=SX126X_LORA_CRC_OFF;

    if(invertIrq)
        PacketParams[5]=0x01; // Inverted LoRa I and Q signals setup
    else
        PacketParams[5]=0x00; // Standard LoRa I and Q signals setup

    // fixes IQ configuration for inverted IQ
    SX126x_FixInvertedIQ(PacketParams[5]);
    SX126x_WriteCommand(SX126X_CMD_SET_PACKET_PARAMS, PacketParams, 6); // 0x8C
    // Do not use DIO interruptst
    SX126x_SetDioIrqParams(SX126X_IRQ_ALL, //all interrupts enabled
            SX126X_IRQ_NONE, //interrupts on DIO1
            SX126X_IRQ_NONE, //interrupts on DIO2
            SX126X_IRQ_NONE //interrupts on DIO3
            );
    // Receive state no receive timeoout
    SX126x_SetRx(0xFFFFFF);
} // </editor-fold>

public uint8_t LoRaReceive(uint8_t *pData, uint8_t len) // <editor-fold defaultstate="collapsed" desc="LoRa receive">
{
    uint8_t rxLen=0;
    uint16_t IrqRegs=SX126x_GetIrqStatus();

    if(IrqRegs&SX126X_IRQ_RX_DONE)
    {
        SX126x_ClearIrqStatus(SX126X_IRQ_ALL);
        rxLen=SX126x_ReadBuffer(pData, len);
    }

    return rxLen;
} // </editor-fold>

public bool LoRaSend(uint8_t *pData, uint8_t len, uint8_t mode) // <editor-fold defaultstate="collapsed" desc="LoRa send">
{
    uint16_t IrqStatus;
    bool rv=false;

    if(sx126x_gpio_get_level(SX126x_TXEN)==true)
    {
        sx126x_gpio_set_level(SX126x_TXEN, HIGH);

        if(PacketParams[2]==0x00) // Variable length packet (explicit header)
            PacketParams[3]=(uint8_t) len;

        SX126x_WriteCommand(SX126X_CMD_SET_PACKET_PARAMS, PacketParams, 6); // 0x8C
        SX126x_ClearIrqStatus(SX126X_IRQ_ALL);
        SX126x_WriteBuffer(pData, len);
        SX126x_SetTx(500);
        txLost=0;

        if(mode&SX126x_TXMODE_SYNC)
        {
            IrqStatus=SX126x_GetIrqStatus();

            while((!(IrqStatus&SX126X_IRQ_TX_DONE))&&(!(IrqStatus&SX126X_IRQ_TIMEOUT)))
            {
                sx126x_delay_ms(1);
                IrqStatus=SX126x_GetIrqStatus();
            }

            __dbs(DEBUG_PREFIX "IrqStatus=");
            __dbh2(IrqStatus);

            if(IrqStatus&SX126X_IRQ_TX_DONE)
            {
                __dbs(DEBUG_PREFIX "SX126X_IRQ_TX_DONE");
            }

            if(IrqStatus&SX126X_IRQ_TIMEOUT)
            {
                __dbs(DEBUG_PREFIX "SX126X_IRQ_TIMEOUT");
            }

            sx126x_gpio_set_level(SX126x_TXEN, LOW);
            SX126x_SetRx(0xFFFFFF);

            if(IrqStatus&SX126X_IRQ_TX_DONE)
                rv=true;
        }
        else
            rv=true;
    }

    __dbs(DEBUG_PREFIX "Send rv=");
    __dbh2(rv);

    if(rv==false)
        txLost++;

    return rv;
} // </editor-fold>

public bool SX126x_IsInReceiveMode(void) // <editor-fold defaultstate="collapsed" desc="Check receive mode">
{
    uint16_t Irq;
    bool rv=false;

    if(sx126x_gpio_get_level(SX126x_TXEN)==true)
        rv=true;
    else
    {
        Irq=SX126x_GetIrqStatus();

        if(Irq&(SX126X_IRQ_TX_DONE|SX126X_IRQ_TIMEOUT))
        {
            SX126x_SetRx(0xFFFFFF);
            sx126x_gpio_set_level(SX126x_TXEN, LOW);
            rv=true;
        }
    }

    return rv;
} // </editor-fold>

public void SX126x_GetPacketStatus(int8_t *rssiPacket, int8_t *snrPacket) // <editor-fold defaultstate="collapsed" desc="Get packet status">
{
    uint8_t buf[4];

    SX126x_ReadCommand(SX126X_CMD_GET_PACKET_STATUS, buf, 4); // 0x14
    //Signal power in dBm = ?RssiInst/2 (dBm)
    *rssiPacket=(~(buf[3]>>1)+1); // (*rssiPacket=(buf[3]>>1) * -1;

    if(buf[2]<128)
        *snrPacket=buf[2]>>2;
    else
        *snrPacket=((buf[2]-256)>>2);
} // </editor-fold>

public void SX126x_SetTxPower(int8_t txPowerInDbm) // <editor-fold defaultstate="collapsed" desc="Set TX power in dBm">
{
    SX126x_SetPowerConfig(txPowerInDbm, SX126X_PA_RAMP_200U);
} // </editor-fold>

public void SX126x_Reset(void) // <editor-fold defaultstate="collapsed" desc="Reset chip">
{
    sx126x_delay_ms(10);
    sx126x_gpio_set_level(SX126x_RESET, LOW);
    sx126x_delay_ms(20);
    sx126x_gpio_set_level(SX126x_RESET, HIGH);
    sx126x_delay_ms(10);
    // ensure BUSY is low (state machine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "Chip Reset", true);
} // </editor-fold>

public void SX126x_Wakeup(void) // <editor-fold defaultstate="collapsed" desc="Chip wakeup">
{
    SX126x_GetStatus();
} // </editor-fold>

public void SX126x_SetStandby(uint8_t mode) // <editor-fold defaultstate="collapsed" desc="Set standby">
{
    SX126x_WriteCommand(SX126X_CMD_SET_STANDBY, &mode, 1); // 0x80
} // </editor-fold>

public uint8_t SX126x_GetStatus(void) // <editor-fold defaultstate="collapsed" desc="Get status">
{
    uint8_t rv;

    SX126x_ReadCommand(SX126X_CMD_GET_STATUS, &rv, 1); // 0xC0

    return rv;
} // </editor-fold>

public void SetDio3AsTcxoCtrl(int32_t tcxo_mV, uint32_t delay_ns) // <editor-fold defaultstate="collapsed" desc="Set DIO3 as TCXO controller pin">
{
    uint8_t buf[4];
    uint32_t delay;

    if(labs(tcxo_mV-1600)<=1)
        buf[0]=SX126X_DIO3_OUTPUT_1_6;
    else if(labs(tcxo_mV-1700)<=1)
        buf[0]=SX126X_DIO3_OUTPUT_1_7;
    else if(labs(tcxo_mV-1800)<=1)
        buf[0]=SX126X_DIO3_OUTPUT_1_8;
    else if(labs(tcxo_mV-2200)<=1)
        buf[0]=SX126X_DIO3_OUTPUT_2_2;
    else if(labs(tcxo_mV-2400)<=1)
        buf[0]=SX126X_DIO3_OUTPUT_2_4;
    else if(labs(tcxo_mV-2700)<=1)
        buf[0]=SX126X_DIO3_OUTPUT_2_7;
    else if(labs(tcxo_mV-3000)<=1)
        buf[0]=SX126X_DIO3_OUTPUT_3_0;
    else
        buf[0]=SX126X_DIO3_OUTPUT_3_3;

    delay=(delay_ns/15625); // delay=delay_ns/15625ns
    buf[1]=(uint8_t) ((delay>>16)&0xFF);
    buf[2]=(uint8_t) ((delay>>8)&0xFF);
    buf[3]=(uint8_t) (delay&0xFF);

    SX126x_WriteCommand(SX126X_CMD_SET_DIO3_AS_TCXO_CTRL, buf, 4); // 0x97
} // </editor-fold>

public void SX126x_Calibrate(uint8_t calibParam) // <editor-fold defaultstate="collapsed" desc="Calibration">
{
    // The radio performs calibration of RC64k, RC13M, PLL and ADC at the powering up.
    SX126x_WriteCommand(SX126X_CMD_CALIBRATE, &calibParam, 1); // 0x89
} // </editor-fold>

public void SX126x_SetDio2AsRfSwitchCtrl(bool enable) // <editor-fold defaultstate="collapsed" desc="Set DIO2 as RF switch controller pin">
{
    uint8_t data=enable;
    SX126x_WriteCommand(SX126X_CMD_SET_DIO2_AS_RF_SWITCH_CTRL, &data, 1); // 0x9D
} // </editor-fold>

public void SX126x_SetRfFrequency(uint16_t frequency_mhz) // <editor-fold defaultstate="collapsed" desc="Set RF frequency in MHz">
{
    uint8_t buf[4];
    uint32_t freq=0;

    SX126x_CalibrateImage(frequency_mhz);

    // XTAL_FREQ                       32000000UL // Xtal 32MHz
    // FREQ_DIV                        33554432UL // 2^25
    // FREQ_STEP                       (double)(XTAL_FREQ/FREQ_DIV)
    // freq=(uint32_t) ((double) frequency/(double) FREQ_STEP)
    // freq=frequency*FREQ_DIV/XTAL_FREQ
    // freq=(frequency/1000000)*FREQ_DIV/(XTAL_FREQ/1000000)
    // freq=frequency_mhz*33554432/32=frequency_mhz*1048576=frequency_mhz*2^20
    // freq=frequency_mhz<<20
    freq=frequency_mhz;
    freq<<=20; //freq*=1048576;
    buf[0]=(uint8_t) ((freq>>24)&0xFF);
    buf[1]=(uint8_t) ((freq>>16)&0xFF);
    buf[2]=(uint8_t) ((freq>>8)&0xFF);
    buf[3]=(uint8_t) (freq&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_RF_FREQUENCY, buf, 4); // 0x86
} // </editor-fold>

public void SX126x_CalibrateImage(uint16_t frequency_mhz) // <editor-fold defaultstate="collapsed" desc="Calibrate image">
{
    uint8_t calFreq[2];

    if(frequency_mhz>900)
    {
        calFreq[0]=0xE1;
        calFreq[1]=0xE9;
    }
    else if(frequency_mhz>850)
    {
        calFreq[0]=0xD7;
        calFreq[1]=0xDB;
    }
    else if(frequency_mhz>770)
    {
        calFreq[0]=0xC1;
        calFreq[1]=0xC5;
    }
    else if(frequency_mhz>460)
    {
        calFreq[0]=0x75;
        calFreq[1]=0x81;
    }
    else if(frequency_mhz>425)
    {
        calFreq[0]=0x6B;
        calFreq[1]=0x6F;
    }

    SX126x_WriteCommand(SX126X_CMD_CALIBRATE_IMAGE, calFreq, 2); // 0x98
} // </editor-fold>

public void SX126x_SetRegulatorMode(uint8_t mode) // <editor-fold defaultstate="collapsed" desc="Set regulator mode">
{
    SX126x_WriteCommand(SX126X_CMD_SET_REGULATOR_MODE, &mode, 1); // 0x96
} // </editor-fold>

public void SX126x_SetBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress) // <editor-fold defaultstate="collapsed" desc="Set buffer base address">
{
    uint8_t buf[2];

    buf[0]=txBaseAddress;
    buf[1]=rxBaseAddress;
    SX126x_WriteCommand(SX126X_CMD_SET_BUFFER_BASE_ADDRESS, buf, 2); // 0x8F
} // </editor-fold>

public void SX126x_SetPowerConfig(int8_t power, uint8_t rampTime) // <editor-fold defaultstate="collapsed" desc="Set power configuration">
{
    uint8_t buf[2];

    if(power>22)
        power=22;
    else if(power < -3)
        power= -3;

    buf[0]=(uint8_t) power;
    buf[1]=(uint8_t) rampTime;
    SX126x_WriteCommand(SX126X_CMD_SET_TX_PARAMS, buf, 2); // 0x8E
} // </editor-fold>

public void SX126x_SetPaConfig(uint8_t paDutyCycle, uint8_t hpMax,
                               uint8_t deviceSel, uint8_t paLut) // <editor-fold defaultstate="collapsed" desc="Set PA configuration">
{
    uint8_t buf[4];

    buf[0]=paDutyCycle;
    buf[1]=hpMax;
    buf[2]=deviceSel;
    buf[3]=paLut;
    SX126x_WriteCommand(SX126X_CMD_SET_PA_CONFIG, buf, 4); // 0x95
} // </editor-fold>

public void SX126x_SetOvercurrentProtection(uint16_t currentLimit_mA) // <editor-fold defaultstate="collapsed" desc="Set over current protection">
{
    if(currentLimit_mA<=140)
    {
        uint8_t ocpVal;

        //buf[0]=(uint8_t) ((float)currentLimit_mA/2.5);
        currentLimit_mA<<=1;
        currentLimit_mA/=5;
        ocpVal=(uint8_t) currentLimit_mA;
        SX126x_WriteRegister(SX126X_REG_OCP_CONFIGURATION, &ocpVal, 1); // 0x08E7
    }
} // </editor-fold>

public void SX126x_SetSyncWord(uint16_t sync) // <editor-fold defaultstate="collapsed" desc="Set sync word">
{
    uint8_t buf[2];

    buf[0]=(uint8_t) ((sync>>8)&0x00FF);
    buf[1]=(uint8_t) (sync&0x00FF);
    SX126x_WriteRegister(SX126X_REG_LORA_SYNC_WORD_MSB, buf, 2); // 0x0740
} // </editor-fold>

public void SX126x_SetDioIrqParams(uint16_t IrqMask, uint16_t dio1Mask,
                                   uint16_t dio2Mask, uint16_t dio3Mask) // <editor-fold defaultstate="collapsed" desc="Set IRQ parameters">
{
    uint8_t buf[8];

    buf[0]=(uint8_t) ((IrqMask>>8)&0x00FF);
    buf[1]=(uint8_t) (IrqMask&0x00FF);
    buf[2]=(uint8_t) ((dio1Mask>>8)&0x00FF);
    buf[3]=(uint8_t) (dio1Mask&0x00FF);
    buf[4]=(uint8_t) ((dio2Mask>>8)&0x00FF);
    buf[5]=(uint8_t) (dio2Mask&0x00FF);
    buf[6]=(uint8_t) ((dio3Mask>>8)&0x00FF);
    buf[7]=(uint8_t) (dio3Mask&0x00FF);
    SX126x_WriteCommand(SX126X_CMD_SET_DIO_IRQ_PARAMS, buf, 8); // 0x08
} // </editor-fold>

public void SX126x_SetStopRxTimerOnPreambleDetect(bool enable) // <editor-fold defaultstate="collapsed" desc="Set RX timer on preamble detect">
{
    uint8_t data=enable;

    SX126x_WriteCommand(SX126X_CMD_STOP_TIMER_ON_PREAMBLE, &data, 1); // 0x9F
} // </editor-fold>

public void SX126x_SetLoRaSymbNumTimeout(uint8_t SymbNum) // <editor-fold defaultstate="collapsed" desc="Set LoRa SymbNum timeout">
{
    SX126x_WriteCommand(SX126X_CMD_SET_LORA_SYMB_NUM_TIMEOUT, &SymbNum, 1); // 0xA0
} // </editor-fold>

public void SX126x_SetPacketType(uint8_t packetType) // <editor-fold defaultstate="collapsed" desc="Set packet type">
{
    SX126x_WriteCommand(SX126X_CMD_SET_PACKET_TYPE, &packetType, 1); // 0x01
} // </editor-fold>

public void SX126x_SetModulationParams(uint8_t spreadingFactor,
                                       uint8_t bandwidth,
                                       uint8_t codingRate,
                                       uint8_t lowDataRateOptimize) // <editor-fold defaultstate="collapsed" desc="Set modulation parameters">
{
    uint8_t data[4];
    //currently only LoRa supported
    data[0]=spreadingFactor;
    data[1]=bandwidth;
    data[2]=codingRate;
    data[3]=lowDataRateOptimize;
    SX126x_WriteCommand(SX126X_CMD_SET_MODULATION_PARAMS, data, 4); // 0x8B
} // </editor-fold>

public void SX126x_SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak,
                                uint8_t cadDetMin, uint8_t cadExitMode,
                                uint32_t cadTimeout) // <editor-fold defaultstate="collapsed" desc="Set cad parameters">
{
    uint8_t data[7];

    data[0]=cadSymbolNum;
    data[1]=cadDetPeak;
    data[2]=cadDetMin;
    data[3]=cadExitMode;
    data[4]=(uint8_t) ((cadTimeout>>16)&0xFF);
    data[5]=(uint8_t) ((cadTimeout>>8)&0xFF);
    data[6]=(uint8_t) (cadTimeout&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_CAD_PARAMS, data, 7); // 0x88
} // </editor-fold>

public void SX126x_SetCad(void) // <editor-fold defaultstate="collapsed" desc="Set cad">
{
    uint8_t data=0;

    SX126x_WriteCommand(SX126X_CMD_SET_CAD, &data, 0); // 0xC5
} // </editor-fold>

public uint16_t SX126x_GetIrqStatus(void) // <editor-fold defaultstate="collapsed" desc="Get IRQ status">
{
    uint8_t data[3];
    uint16_t out;

    SX126x_ReadCommand(SX126X_CMD_GET_IRQ_STATUS, data, 3); // 0x12
    out=data[1];
    out<<=8;
    out|=data[2];

    return out;
} // </editor-fold>

public void SX126x_ClearIrqStatus(uint16_t Irq) // <editor-fold defaultstate="collapsed" desc="Clear IRQ status">
{
    uint8_t buf[2];

    buf[0]=(uint8_t) ((Irq>>8)&0x00FF);
    buf[1]=(uint8_t) (Irq&0x00FF);
    SX126x_WriteCommand(SX126X_CMD_CLEAR_IRQ_STATUS, buf, 2); // 0x02
} // </editor-fold>

public void SX126x_SetRx(uint32_t timeout_ms) // <editor-fold defaultstate="collapsed" desc="Set RX with timeout">
{
    uint8_t retry, buf[3];

    SX126x_SetStandby(SX126X_STANDBY_RC);
    SX126x_SetRxEnable();

    if(timeout_ms>16777215)
        timeout_ms=16777215;

    buf[0]=(uint8_t) ((timeout_ms>>16)&0xFF);
    buf[1]=(uint8_t) ((timeout_ms>>8)&0xFF);
    buf[2]=(uint8_t) (timeout_ms&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_RX, buf, 3); // 0x82

    for(retry=0; retry<10; retry++)
    {
        if((SX126x_GetStatus()&0x70)==0x50)
            break;

        sx126x_delay_ms(1);
    }

    if((SX126x_GetStatus()&0x70)!=0x50)
    {
        __dbs(DEBUG_PREFIX "SX126x_SetRx Illegal Status");
        sx126x_error(ERR_INVALID_SETRX_STATE);
    }
} // </editor-fold>

public void SX126x_SetRxEnable(void) // <editor-fold defaultstate="collapsed" desc="Set RX enable">
{
    // Complementary-pin Control Logic
    // RFC to RF2
    sx126x_gpio_set_level(SX126x_TXEN, HIGH);
    SX126x_SetDio2AsRfSwitchCtrl(false); // Set as RX mode
} // </editor-fold>

public void SX126x_SetTx(uint32_t timeout_ms) // <editor-fold defaultstate="collapsed" desc="Set TX with timeout">
{
    uint8_t retry, buf[3];

    SX126x_SetStandby(SX126X_STANDBY_RC);
    SX126x_SetTxEnable();
    // Timeout duration (us) = Timeout * 15.625 (us)
    // Timeout duration (ms) = Timeout * 0.015625 (ms)
    // Timeout=Timeout duration (ms)/0.015625 (ms)
    // Timeout=Timeout duration (ms)*64 (ms)
    timeout_ms<<=6;//timeout_ms*=64;

    if(timeout_ms>16777215)
        timeout_ms=16777215;

    buf[0]=(uint8_t) ((timeout_ms>>16)&0xFF);
    buf[1]=(uint8_t) ((timeout_ms>>8)&0xFF);
    buf[2]=(uint8_t) (timeout_ms&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_TX, buf, 3); // 0x83

    for(retry=0; retry<10; retry++)
    {
        if((SX126x_GetStatus()&0x70)==0x60)
            break;

        system_wait();
    }

    if((SX126x_GetStatus()&0x70)!=0x60)
    {
        __dbs(DEBUG_PREFIX "SetTx Illegal Status");
        sx126x_error(ERR_INVALID_SETTX_STATE);
    }
} // </editor-fold>

public void SX126x_SetTxEnable(void) // <editor-fold defaultstate="collapsed" desc="Set TX enable">
{
    // Complementary-pin Control Logic
    // RFC to RF1
    sx126x_gpio_set_level(SX126x_TXEN, LOW);
    SX126x_SetDio2AsRfSwitchCtrl(true); // Set as TX mode
} // </editor-fold>

public uint16_t SX126x_GetPacketLost(void) // <editor-fold defaultstate="collapsed" desc="Get packet lost">
{
    return txLost;
} // </editor-fold>

public int8_t SX126x_GetRssiInst(void) // <editor-fold defaultstate="collapsed" desc="Get RSSI">
{
    uint8_t buf[2];

    SX126x_ReadCommand(SX126X_CMD_GET_RSSI_INST, buf, 2); // 0x15
    // Signal power in dBm = ?RssiInst/2 (dBm)
    return (int8_t) (~(buf[1]>>1)+1);
} // </editor-fold>

public void SX126x_GetRxBufferStatus(uint8_t *payloadLength, uint8_t *rxStartBufferPointer) // <editor-fold defaultstate="collapsed" desc="Get RX buffer status">
{
    uint8_t buf[3];

    SX126x_ReadCommand(SX126X_CMD_GET_RX_BUFFER_STATUS, buf, 3); // 0x13
    *payloadLength=buf[1];
    *rxStartBufferPointer=buf[2];
} // </editor-fold>

public void SX126x_WaitForIdleBegin(uint32_t timeout_ms, char *text) // <editor-fold defaultstate="collapsed" desc="Waiting for idle begin">
{
    // ensure BUSY is low (state machine ready)
    int retry;
    bool stop=false;

    for(retry=0; retry<10; retry++)
    {
        bool ret;

        if(retry==9)
            stop=true;

        ret=SX126x_WaitForIdle(BUSY_WAIT, text, stop);

        if(ret==true)
            break;

        __dbsi(DEBUG_PREFIX "WaitForIdle fail retry=", retry);
        system_wait();
    }
} // </editor-fold>

public bool SX126x_WaitForIdle(uint32_t timeout_ms, const char *text, bool stop) // <editor-fold defaultstate="collapsed" desc="Waiting for idle">
{
    bool ret=true;
    uint32_t start=sx126x_get_system_tick_ms();

    while(sx126x_get_system_tick_ms()-start<timeout_ms)
    {
        if(sx126x_gpio_get_level(SX126x_BUSY)==0)
            return true;
        // Give up CPU execution rights
        system_wait();
    }

    __dbss(DEBUG_PREFIX "WaitForIdle Timeout text=", text);
    __dbsu(" timeout=", timeout_ms);
    __dbsu(" start=", start);

    if(stop)
        sx126x_error(ERR_IDLE_TIMEOUT);
    else
        ret=false;

    return ret;
} // </editor-fold>

public uint8_t SX126x_ReadBuffer(uint8_t *rxData, uint8_t rxDataLen) // <editor-fold defaultstate="collapsed" desc="Read buffer">
{
    uint8_t buf[LORA_MSG_LEN_MAX+3];
    uint8_t offset=0, payloadLength=0;

    SX126x_GetRxBufferStatus(&payloadLength, &offset);

    if(payloadLength>rxDataLen)
    {
        __dbsu(DEBUG_PREFIX "ReadBuffer rxDataLen too small. payloadLength=", payloadLength);
        __dbsu(">rxDataLen=", rxDataLen);

        return 0;
    }

    if(payloadLength>LORA_MSG_LEN_MAX)
    {
        __dbsu(DEBUG_PREFIX "ReadBuffer LORA_MSG_LEN_MAX too small. payloadLength=", payloadLength);
        __dbsu(">LORA_MSG_LEN_MAX=", LORA_MSG_LEN_MAX);

        return 0;
    }
    // ensure BUSY is low (state machine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start ReadBuffer", true);
    // start transfer
    buf[0]=SX126X_CMD_READ_BUFFER; // 0x1E
    buf[1]=offset; // offset in rx fifo
    buf[2]=SX126X_CMD_NOP;
    memset(&buf[3], SX126X_CMD_NOP, payloadLength);
    sx126x_spi_exchange(buf, buf, payloadLength+3);
    memcpy(rxData, &buf[3], payloadLength);
    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end ReadBuffer", false);

    return payloadLength;
} // </editor-fold>

public void SX126x_WriteBuffer(uint8_t *txData, uint8_t txDataLen) // <editor-fold defaultstate="collapsed" desc="Write buffer">
{
    uint8_t buf[LORA_MSG_LEN_MAX+2];

    if(txDataLen>LORA_MSG_LEN_MAX)
    {
        __dbs(DEBUG_PREFIX "WriteBuffer out of memory");

        return;
    }
    // ensure BUSY is low (state machine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start WriteBuffer", true);
    // start transfer
    buf[0]=SX126X_CMD_WRITE_BUFFER; // 0x0E
    buf[1]=0; // offset in tx fifo
    memcpy(&buf[2], txData, txDataLen);
    sx126x_spi_exchange(buf, NULL, (txDataLen+2));
    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end WriteBuffer", false);
} // </editor-fold>

public void SX126x_WriteRegister(uint16_t reg, uint8_t* data, uint8_t numBytes) // <editor-fold defaultstate="collapsed" desc="Write register">
{
    uint8_t n, buf[16];
    // ensure BUSY is low (state machine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start WriteRegister", true);

    __dbs(DEBUG_PREFIX "WriteRegister: REG=");
    __dbh2(reg);
    __dbs(", DataOut: ");

    for(n=0; n<numBytes; n++)
    {
        if(n>0)
            __dbs(", ");

        __dbh2(data[n]);
    }
    // start transfer
    buf[0]=SX126X_CMD_WRITE_REGISTER;
    buf[1]=(reg&0xFF00)>>8;
    buf[2]=reg&0xff;
    memcpy(&buf[3], data, numBytes);
    sx126x_spi_exchange(buf, NULL, (3+numBytes));
    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end WriteRegister", false);
} // </editor-fold>

public void SX126x_ReadRegister(uint16_t reg, uint8_t* data, uint8_t numBytes) // <editor-fold defaultstate="collapsed" desc="Read register">
{
    uint8_t n, buf[16];

    // ensure BUSY is low (state machine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start ReadRegister", true);

    __dbs(DEBUG_PREFIX "ReadRegister: REG=");
    __dbh2(reg);

    // start transfer
    memset(buf, SX126X_CMD_NOP, sizeof (buf));
    buf[0]=SX126X_CMD_READ_REGISTER;
    buf[1]=(reg&0xFF00)>>8;
    buf[2]=reg&0xff;
    sx126x_spi_exchange(buf, buf, 4+numBytes);
    memcpy(data, &buf[4], numBytes);
    __dbs(DEBUG_PREFIX "DataIn: ");

    for(n=0; n<numBytes; n++)
    {
        if(n>0)
            __dbs(", ");

        __dbh2(data[n]);
    }
    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end ReadRegister", false);
} // </editor-fold>

public void SX126x_WriteCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes) // <editor-fold defaultstate="collapsed" desc="Write command with retry">
{
    uint8_t status, retry;

    for(retry=1; retry<10; retry++)
    {
        status=SX126x_WriteCommand2(cmd, data, numBytes);
        __dbs(DEBUG_PREFIX "status=");
        __dbh2(status);

        if(status==0)
            break;

        __dbs(DEBUG_PREFIX "WriteCommand status=");
        __dbh2(status);
        __dbsi(" retry=", retry);
    }

    if(status!=0)
    {
        __dbs(DEBUG_PREFIX "SPI Transaction error:");
        __dbh2(status);
        sx126x_error(ERR_SPI_TRANSACTION);
    }
} // </editor-fold>

public uint8_t SX126x_WriteCommand2(uint8_t cmd, uint8_t* data, uint8_t numBytes) // <editor-fold defaultstate="collapsed" desc="Write command 2 with retry">
{
    uint8_t status, cmd_status, buf[16];
    // ensure BUSY is low (state machine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start WriteCommand2", true);
    __dbs(DEBUG_PREFIX "SX126x_WriteCommand: CMD=");
    __dbh2(cmd);

    // start transfer
    buf[0]=cmd;
    memcpy(&buf[1], data, numBytes);
    sx126x_spi_exchange(buf, buf, numBytes+1);
    status=0;
    cmd_status=buf[1]&0xe;

    switch(cmd_status)
    {
        case SX126X_STATUS_CMD_TIMEOUT:
        case SX126X_STATUS_CMD_INVALID:
        case SX126X_STATUS_CMD_FAILED:
            status=cmd_status;
            break;

        case 0:
        case 7:
            status=SX126X_STATUS_SPI_FAILED;
            break;

        default:
            break; // success
    }

    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end WriteCommand2", false);

    return status;
} // </editor-fold>

public void SX126x_ReadCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes) // <editor-fold defaultstate="collapsed" desc="Read command">
{
    uint8_t buf[16];
    // ensure BUSY is low (state machine ready)
    SX126x_WaitForIdleBegin(BUSY_WAIT, "start ReadCommand");

    __dbs(DEBUG_PREFIX "ReadCommand: CMD=");
    __dbh2(cmd);

    // start transfer
    memset(buf, SX126X_CMD_NOP, sizeof (buf));
    buf[0]=cmd;
    sx126x_spi_exchange(buf, buf, 1+numBytes);

    if(data!=NULL&&numBytes)
        memcpy(data, &buf[1], numBytes);

    // wait for BUSY to go low
    system_wait();
    SX126x_WaitForIdle(BUSY_WAIT, "end ReadCommand", false);
} // </editor-fold>
