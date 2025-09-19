#include "hardware.h"
#include "SX126x.h"

#ifndef USE_SX126X_DEBUG
#define DEBUG_H
#endif

#include "common/debug.h"

// Global Stuff
static uint8_t PacketParams[6];
static bool txActive;
static int txLost=0;
static bool debugPrint;

void spi_write_byte(const uint8_t *Datain, size_t DataLength)
{
    sx126x_spi_exchange(Datain, NULL, DataLength);
}

void LoRaError(int error)
{
    if(debugPrint)
    {
        __dbsi("\r\n[SX126x] LoRaErrorDefault=", error);
    }

    while(true)
    {
        system_wait();
    }
}

void LoRaInit(void)
{
    txActive=false;
    debugPrint=false;

    sx126x_gpio_reset_pin(SX126x_SPI_SELECT);
    sx126x_gpio_set_direction(SX126x_SPI_SELECT, GPIO_MODE_OUTPUT);
    sx126x_gpio_set_level(SX126x_SPI_SELECT, 1);

    sx126x_gpio_reset_pin(SX126x_RESET);
    sx126x_gpio_set_direction(SX126x_RESET, GPIO_MODE_OUTPUT);

    sx126x_gpio_reset_pin(SX126x_BUSY);
    sx126x_gpio_set_direction(SX126x_BUSY, GPIO_MODE_INPUT);

    sx126x_gpio_reset_pin(SX126x_TXEN);
    sx126x_gpio_set_direction(SX126x_TXEN, GPIO_MODE_OUTPUT);

    sx126x_gpio_reset_pin(SX126x_RXEN);
    sx126x_gpio_set_direction(SX126x_RXEN, GPIO_MODE_OUTPUT);
}

int16_t LoRaBegin(uint32_t frequencyInHz, int8_t txPowerInDbm, float tcxoVoltage, bool useRegulatorLDO)
{
    if(txPowerInDbm>22)
        txPowerInDbm=22;
    if(txPowerInDbm < -3)
        txPowerInDbm= -3;

    SX126x_Reset();

    uint8_t wk[2];
    SX126x_ReadRegister(SX126X_REG_LORA_SYNC_WORD_MSB, wk, 2); // 0x0740
    uint16_t syncWord=wk[0]; //(wk[0]<<8)+wk[1];
    syncWord<<=8;
    syncWord+=wk[1];

    __dbs("\r\n[SX126x] syncWord=");
    __dbh2(syncWord);

    if(syncWord!=SX126X_SYNC_WORD_PUBLIC&&syncWord!=SX126X_SYNC_WORD_PRIVATE)
    {
        __dbs("\r\n[SX126x] SX126x error, maybe no SPI connection");
        return ERR_INVALID_MODE;
    }

    __dbs("\r\n[SX126x] SX126x installed");
    SX126x_SetStandby(SX126X_STANDBY_RC);

    SetDio2AsRfSwitchCtrl(true);
    __dbsi("\r\n[SX126x] tcxoVoltage(mV)=", (int) (tcxoVoltage*1000.0));
    // set TCXO control, if requested
    if(tcxoVoltage>0.0)
    {
        SetDio3AsTcxoCtrl(tcxoVoltage, RADIO_TCXO_SETUP_TIME); // Configure the radio to use a TCXO controlled by DIO3
    }

    SX126x_Calibrate(SX126X_CALIBRATE_IMAGE_ON
            |SX126X_CALIBRATE_ADC_BULK_P_ON
            |SX126X_CALIBRATE_ADC_BULK_N_ON
            |SX126X_CALIBRATE_ADC_PULSE_ON
            |SX126X_CALIBRATE_PLL_ON
            |SX126X_CALIBRATE_RC13M_ON
            |SX126X_CALIBRATE_RC64K_ON
            );

    __dbsu("\r\n[SX126x] useRegulatorLDO=", useRegulatorLDO);
    if(useRegulatorLDO)
    {
        SX126x_SetRegulatorMode(SX126X_REGULATOR_LDO); // set regulator mode: LDO
    }
    else
    {
        SX126x_SetRegulatorMode(SX126X_REGULATOR_DC_DC); // set regulator mode: DC-DC
    }

    SX126x_SetBufferBaseAddress(0, 0);
#if 0
    // SX1261_TRANCEIVER
    SX126x_SetPaConfig(0x06, 0x00, 0x01, 0x01); // PA Optimal Settings +15 dBm
    // SX1262_TRANCEIVER
    SX126x_SetPaConfig(0x04, 0x07, 0x00, 0x01); // PA Optimal Settings +22 dBm
    // SX1268_TRANCEIVER
    SX126x_SetPaConfig(0x04, 0x07, 0x00, 0x01); // PA Optimal Settings +22 dBm
#endif
    SX126x_SetPaConfig(0x04, 0x07, 0x00, 0x01); // PA Optimal Settings +22 dBm
    SX126x_SetOvercurrentProtection(60.0); // current max 60mA for the whole device
    SX126x_SetPowerConfig(txPowerInDbm, SX126X_PA_RAMP_200U); //0 fuer Empfaenger
    SX126x_SetRfFrequency(frequencyInHz);
    return ERR_NONE;
}

void SX126x_FixInvertedIQ(uint8_t iqConfig)
{
    // fixes IQ configuration for inverted IQ
    // see SX1262/SX1268 datasheet, chapter 15 Known Limitations, section 15.4 for details
    // When exchanging LoRa packets with inverted IQ polarity, some packet losses may be observed for longer packets.
    // Workaround: Bit 2 at address 0x0736 must be set to:
    // “0” when using inverted IQ polarity (see the SetPacketParam(...) command)
    // “1” when using standard IQ polarity

    // read current IQ configuration
    uint8_t iqConfigCurrent=0;
    SX126x_ReadRegister(SX126X_REG_IQ_POLARITY_SETUP, &iqConfigCurrent, 1); // 0x0736

    // set correct IQ configuration
    //if(iqConfig == SX126X_LORA_IQ_STANDARD) {
    if(iqConfig==SX126X_LORA_IQ_INVERTED)
    {
        iqConfigCurrent&=0xFB; // using inverted IQ polarity
    }
    else
    {
        iqConfigCurrent|=0x04; // using standard IQ polarity
    }

    // update with the new value
    SX126x_WriteRegister(SX126X_REG_IQ_POLARITY_SETUP, &iqConfigCurrent, 1); // 0x0736
}

void LoRaConfig(uint8_t spreadingFactor, uint8_t bandwidth, uint8_t codingRate, uint16_t preambleLength, uint8_t payloadLen, bool crcOn, bool invertIrq)
{
    SX126x_SetStopRxTimerOnPreambleDetect(false);
    SX126x_SetLoRaSymbNumTimeout(0);
    SX126x_SetPacketType(SX126X_PACKET_TYPE_LORA); // SX126x.ModulationParams.PacketType : MODEM_LORA
    uint8_t ldro=0; // LowDataRateOptimize OFF
    SX126x_SetModulationParams(spreadingFactor, bandwidth, codingRate, ldro);

    PacketParams[0]=(preambleLength>>8) & 0xFF;
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
}

void LoRaDebugPrint(bool enable)
{
    debugPrint=enable;
}

uint8_t LoRaReceive(uint8_t *pData, int16_t len)
{
    uint8_t rxLen=0;
    uint16_t IrqRegs=SX126x_GetIrqStatus();
    //uint8_t status = SX126x_GetStatus();

    if(IrqRegs&SX126X_IRQ_RX_DONE)
    {
        //SX126x_ClearIrqStatus(SX126X_IRQ_RX_DONE);
        SX126x_ClearIrqStatus(SX126X_IRQ_ALL);
        rxLen=SX126x_ReadBuffer(pData, len);
    }

    return rxLen;
}

bool LoRaSend(uint8_t *pData, int16_t len, uint8_t mode)
{
    uint16_t IrqStatus;
    bool rv=false;

    if(txActive==false)
    {
        txActive=true;
        if(PacketParams[2]==0x00)
        { // Variable length packet (explicit header)
            PacketParams[3]=(uint8_t) len;
        }
        SX126x_WriteCommand(SX126X_CMD_SET_PACKET_PARAMS, PacketParams, 6); // 0x8C

        //SX126x_ClearIrqStatus(SX126X_IRQ_TX_DONE | SX126X_IRQ_TIMEOUT);
        SX126x_ClearIrqStatus(SX126X_IRQ_ALL);

        SX126x_WriteBuffer(pData, len);
        SX126x_SetTx(500);

        if(mode&SX126x_TXMODE_SYNC)
        {
            IrqStatus=SX126x_GetIrqStatus();
            while((!(IrqStatus&SX126X_IRQ_TX_DONE)) && (!(IrqStatus&SX126X_IRQ_TIMEOUT)))
            {
                delay(1);
                IrqStatus=SX126x_GetIrqStatus();
            }
            if(debugPrint)
            {
                __dbs("\r\n[SX126x] IrqStatus=");
                __dbh2(IrqStatus);

                if(IrqStatus&SX126X_IRQ_TX_DONE)
                {
                    __dbs("\r\n[SX126x] SX126X_IRQ_TX_DONE");
                }
                if(IrqStatus&SX126X_IRQ_TIMEOUT)
                {
                    __dbs("\r\n[SX126x] SX126X_IRQ_TIMEOUT");
                }
            }
            txActive=false;

            SX126x_SetRx(0xFFFFFF);

            if(IrqStatus&SX126X_IRQ_TX_DONE)
            {
                rv=true;
            }
        }
        else
        {
            rv=true;
        }
    }
    if(debugPrint)
    {
        __dbs("\r\n[SX126x] Send rv=");
        __dbh2(rv);
    }
    if(rv==false) txLost++;
    return rv;
}

bool SX126x_ReceiveMode(void)
{
    uint16_t Irq;
    bool rv=false;

    if(txActive==false)
    {
        rv=true;
    }
    else
    {
        Irq=SX126x_GetIrqStatus();
        if(Irq&(SX126X_IRQ_TX_DONE|SX126X_IRQ_TIMEOUT))
        {
            SX126x_SetRx(0xFFFFFF);
            txActive=false;
            rv=true;
        }
    }

    return rv;
}

void SX126x_GetPacketStatus(int8_t *rssiPacket, int8_t *snrPacket)
{
    uint8_t buf[4];
    SX126x_ReadCommand(SX126X_CMD_GET_PACKET_STATUS, buf, 4); // 0x14
    *rssiPacket=(buf[3]>>1) * -1;
    (buf[2]<128)?(*snrPacket=buf[2]>>2):(*snrPacket=((buf[2]-256)>>2));
}

void SX126x_SetTxPower(int8_t txPowerInDbm)
{
    SX126x_SetPowerConfig(txPowerInDbm, SX126X_PA_RAMP_200U);
}

void SX126x_Reset(void)
{
    delay(10);
    sx126x_gpio_set_level(SX126x_RESET, 0);
    delay(20);
    sx126x_gpio_set_level(SX126x_RESET, 1);
    delay(10);
    // ensure BUSY is low (state meachine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "SX126x_Reset", true);
}

void SX126x_Wakeup(void)
{
    SX126x_GetStatus();
}

void SX126x_SetStandby(uint8_t mode)
{
    uint8_t data=mode;
    SX126x_WriteCommand(SX126X_CMD_SET_STANDBY, &data, 1); // 0x80
}

uint8_t SX126x_GetStatus(void)
{
    uint8_t rv;
    SX126x_ReadCommand(SX126X_CMD_GET_STATUS, &rv, 1); // 0xC0
    return rv;
}

void SetDio3AsTcxoCtrl(float voltage, uint32_t delay)
{
    uint8_t buf[4];

    //buf[0] = tcxoVoltage & 0x07;
    if(fabs(voltage-1.6)<=0.001)
    {
        buf[0]=SX126X_DIO3_OUTPUT_1_6;
    }
    else if(fabs(voltage-1.7)<=0.001)
    {
        buf[0]=SX126X_DIO3_OUTPUT_1_7;
    }
    else if(fabs(voltage-1.8)<=0.001)
    {
        buf[0]=SX126X_DIO3_OUTPUT_1_8;
    }
    else if(fabs(voltage-2.2)<=0.001)
    {
        buf[0]=SX126X_DIO3_OUTPUT_2_2;
    }
    else if(fabs(voltage-2.4)<=0.001)
    {
        buf[0]=SX126X_DIO3_OUTPUT_2_4;
    }
    else if(fabs(voltage-2.7)<=0.001)
    {
        buf[0]=SX126X_DIO3_OUTPUT_2_7;
    }
    else if(fabs(voltage-3.0)<=0.001)
    {
        buf[0]=SX126X_DIO3_OUTPUT_3_0;
    }
    else
    {
        buf[0]=SX126X_DIO3_OUTPUT_3_3;
    }

    uint32_t delayValue=(uint32_t) ((float) delay/15.625);
    buf[1]=(uint8_t) ((delayValue>>16) & 0xFF);
    buf[2]=(uint8_t) ((delayValue>>8) & 0xFF);
    buf[3]=(uint8_t) (delayValue&0xFF);

    SX126x_WriteCommand(SX126X_CMD_SET_DIO3_AS_TCXO_CTRL, buf, 4); // 0x97
}

void SX126x_Calibrate(uint8_t calibParam)
{
    uint8_t data=calibParam;
    SX126x_WriteCommand(SX126X_CMD_CALIBRATE, &data, 1); // 0x89
}

void SetDio2AsRfSwitchCtrl(uint8_t enable)
{
    uint8_t data=enable;
    SX126x_WriteCommand(SX126X_CMD_SET_DIO2_AS_RF_SWITCH_CTRL, &data, 1); // 0x9D
}

void SX126x_SetRfFrequency(uint32_t frequency)
{
    uint8_t buf[4];
    uint32_t freq=0;

    SX126x_CalibrateImage(frequency);

    freq=(uint32_t) ((double) frequency/(double) FREQ_STEP);
    buf[0]=(uint8_t) ((freq>>24) & 0xFF);
    buf[1]=(uint8_t) ((freq>>16) & 0xFF);
    buf[2]=(uint8_t) ((freq>>8) & 0xFF);
    buf[3]=(uint8_t) (freq&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_RF_FREQUENCY, buf, 4); // 0x86
}

void SX126x_CalibrateImage(uint32_t frequency)
{
    uint8_t calFreq[2];

    if(frequency>900000000)
    {
        calFreq[0]=0xE1;
        calFreq[1]=0xE9;
    }
    else if(frequency>850000000)
    {
        calFreq[0]=0xD7;
        calFreq[1]=0xDB;
    }
    else if(frequency>770000000)
    {
        calFreq[0]=0xC1;
        calFreq[1]=0xC5;
    }
    else if(frequency>460000000)
    {
        calFreq[0]=0x75;
        calFreq[1]=0x81;
    }
    else if(frequency>425000000)
    {
        calFreq[0]=0x6B;
        calFreq[1]=0x6F;
    }
    SX126x_WriteCommand(SX126X_CMD_CALIBRATE_IMAGE, calFreq, 2); // 0x98
}

void SX126x_SetRegulatorMode(uint8_t mode)
{
    uint8_t data=mode;
    SX126x_WriteCommand(SX126X_CMD_SET_REGULATOR_MODE, &data, 1); // 0x96
}

void SX126x_SetBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress)
{
    uint8_t buf[2];

    buf[0]=txBaseAddress;
    buf[1]=rxBaseAddress;
    SX126x_WriteCommand(SX126X_CMD_SET_BUFFER_BASE_ADDRESS, buf, 2); // 0x8F
}

void SX126x_SetPowerConfig(int8_t power, uint8_t rampTime)
{
    uint8_t buf[2];

    if(power>22)
    {
        power=22;
    }
    else if(power < -3)
    {
        power= -3;
    }

    buf[0]=(uint8_t) power;
    buf[1]=(uint8_t) rampTime;
    SX126x_WriteCommand(SX126X_CMD_SET_TX_PARAMS, buf, 2); // 0x8E
}

void SX126x_SetPaConfig(uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel, uint8_t paLut)
{
    uint8_t buf[4];

    buf[0]=paDutyCycle;
    buf[1]=hpMax;
    buf[2]=deviceSel;
    buf[3]=paLut;
    SX126x_WriteCommand(SX126X_CMD_SET_PA_CONFIG, buf, 4); // 0x95
}

void SX126x_SetOvercurrentProtection(float currentLimit)
{
    if((currentLimit>=0.0) && (currentLimit<=140.0))
    {
        uint8_t buf[1];
        buf[0]=(uint8_t) (currentLimit/2.5);
        SX126x_WriteRegister(SX126X_REG_OCP_CONFIGURATION, buf, 1); // 0x08E7
    }
}

void SX126x_SetSyncWord(int16_t sync)
{
    uint8_t buf[2];

    buf[0]=(uint8_t) ((sync>>8) & 0x00FF);
    buf[1]=(uint8_t) (sync&0x00FF);
    SX126x_WriteRegister(SX126X_REG_LORA_SYNC_WORD_MSB, buf, 2); // 0x0740
}

void SX126x_SetDioIrqParams
(uint16_t IrqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask)
{
    uint8_t buf[8];

    buf[0]=(uint8_t) ((IrqMask>>8) & 0x00FF);
    buf[1]=(uint8_t) (IrqMask&0x00FF);
    buf[2]=(uint8_t) ((dio1Mask>>8) & 0x00FF);
    buf[3]=(uint8_t) (dio1Mask&0x00FF);
    buf[4]=(uint8_t) ((dio2Mask>>8) & 0x00FF);
    buf[5]=(uint8_t) (dio2Mask&0x00FF);
    buf[6]=(uint8_t) ((dio3Mask>>8) & 0x00FF);
    buf[7]=(uint8_t) (dio3Mask&0x00FF);
    SX126x_WriteCommand(SX126X_CMD_SET_DIO_IRQ_PARAMS, buf, 8); // 0x08
}

void SX126x_SetStopRxTimerOnPreambleDetect(bool enable)
{
    __dbsu("\r\n[SX126x] SX126x_SetStopRxTimerOnPreambleDetect enable=", enable);
    //uint8_t data = (uint8_t)enable;
    uint8_t data=0;
    if(enable) data=1;
    SX126x_WriteCommand(SX126X_CMD_STOP_TIMER_ON_PREAMBLE, &data, 1); // 0x9F
}

void SX126x_SetLoRaSymbNumTimeout(uint8_t SymbNum)
{
    uint8_t data=SymbNum;
    SX126x_WriteCommand(SX126X_CMD_SET_LORA_SYMB_NUM_TIMEOUT, &data, 1); // 0xA0
}

void SX126x_SetPacketType(uint8_t packetType)
{
    uint8_t data=packetType;
    SX126x_WriteCommand(SX126X_CMD_SET_PACKET_TYPE, &data, 1); // 0x01
}

void SX126x_SetModulationParams(uint8_t spreadingFactor, uint8_t bandwidth, uint8_t codingRate, uint8_t lowDataRateOptimize)
{
    uint8_t data[4];
    //currently only LoRa supported
    data[0]=spreadingFactor;
    data[1]=bandwidth;
    data[2]=codingRate;
    data[3]=lowDataRateOptimize;
    SX126x_WriteCommand(SX126X_CMD_SET_MODULATION_PARAMS, data, 4); // 0x8B
}

void SX126x_SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, uint8_t cadExitMode, uint32_t cadTimeout)
{
    uint8_t data[7];
    data[0]=cadSymbolNum;
    data[1]=cadDetPeak;
    data[2]=cadDetMin;
    data[3]=cadExitMode;
    data[4]=(uint8_t) ((cadTimeout>>16) & 0xFF);
    data[5]=(uint8_t) ((cadTimeout>>8) & 0xFF);
    data[6]=(uint8_t) (cadTimeout&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_CAD_PARAMS, data, 7); // 0x88
}

void SX126x_SetCad(void)
{
    uint8_t data=0;
    SX126x_WriteCommand(SX126X_CMD_SET_CAD, &data, 0); // 0xC5
}

uint16_t SX126x_GetIrqStatus(void)
{
    uint8_t data[3];
    uint16_t out;

    SX126x_ReadCommand(SX126X_CMD_GET_IRQ_STATUS, data, 3); // 0x12
    out=data[1];
    out<<=8;
    out|=data[2];

    return out;
}

void SX126x_ClearIrqStatus(uint16_t Irq)
{
    uint8_t buf[2];

    buf[0]=(uint8_t) (((uint16_t) Irq>>8) & 0x00FF);
    buf[1]=(uint8_t) ((uint16_t) Irq&0x00FF);
    SX126x_WriteCommand(SX126X_CMD_CLEAR_IRQ_STATUS, buf, 2); // 0x02
}

void SX126x_SetRx(uint32_t timeout_ms)
{
    if(debugPrint)
    {
        __dbsu("\r\n[SX126x] SetRx timeout(ms)=", timeout_ms);
    }
    SX126x_SetStandby(SX126X_STANDBY_RC);
    SX126x_SetRxEnable();
    uint8_t buf[3];
    buf[0]=(uint8_t) ((timeout_ms>>16) & 0xFF);
    buf[1]=(uint8_t) ((timeout_ms>>8) & 0xFF);
    buf[2]=(uint8_t) (timeout_ms&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_RX, buf, 3); // 0x82

    for(int retry=0; retry<10; retry++)
    {
        if((SX126x_GetStatus() & 0x70)==0x50) break;
        delay(1);
    }
    if((SX126x_GetStatus() & 0x70)!=0x50)
    {
        __dbs("\r\n[SX126x] SX126x_SetRx Illegal Status");
        LoRaError(ERR_INVALID_SETRX_STATE);
    }
}

void SX126x_SetRxEnable(void)
{
    if(debugPrint)
    {
        __dbsu("\r\n[SX126x] SX126x_SetRxEnable:SX126x_TXEN=", SX126x_TXEN);
        __dbsu(" SX126x_RXEN=", SX126x_RXEN);
    }
    if((SX126x_TXEN!= -1) && (SX126x_RXEN!= -1))
    {
        sx126x_gpio_set_level(SX126x_RXEN, HIGH);
        sx126x_gpio_set_level(SX126x_TXEN, LOW);
    }
}

void SX126x_SetTx(uint32_t timeout_msInMs)
{
    if(debugPrint)
    {
        __dbsu("\r\n[SX126x] ----- SX126x_SetTx timeoutInMs=", timeout_msInMs);
    }
    SX126x_SetStandby(SX126X_STANDBY_RC);
    SX126x_SetTxEnable();
    uint8_t buf[3];
    uint32_t tout=timeout_msInMs;
    if(timeout_msInMs!=0)
    {
        float timeout_msInUs=(float)(timeout_msInMs*1000);
        tout=(uint32_t) (timeout_msInUs/0.015625);
    }
    if(debugPrint)
    {
        __dbsu("\r\n[SX126x] SX126x_SetTx timeout_msInMs=", timeout_msInMs);
        __dbsu(" tout=", tout);
    }
    buf[0]=(uint8_t) ((tout>>16) & 0xFF);
    buf[1]=(uint8_t) ((tout>>8) & 0xFF);
    buf[2]=(uint8_t) (tout&0xFF);
    SX126x_WriteCommand(SX126X_CMD_SET_TX, buf, 3); // 0x83

    for(int retry=0; retry<10; retry++)
    {
        if((SX126x_GetStatus() & 0x70)==0x60) break;
        system_wait();
    }
    if((SX126x_GetStatus() & 0x70)!=0x60)
    {
        __dbs("\r\n[SX126x] SX126x_SetTx Illegal Status");
        LoRaError(ERR_INVALID_SETTX_STATE);
    }
}

void SX126x_SetTxEnable(void)
{
    if(debugPrint)
    {
        __dbsu("\r\n[SX126x] SX126x_SetTxEnable:SX126x_TXEN=", SX126x_TXEN);
        __dbsu(" SX126x_RXEN=", SX126x_RXEN);
    }
    if((SX126x_TXEN!= -1) && (SX126x_RXEN!= -1))
    {
        sx126x_gpio_set_level(SX126x_RXEN, LOW);
        sx126x_gpio_set_level(SX126x_TXEN, HIGH);
    }
}

int SX126x_GetPacketLost(void)
{
    return txLost;
}

uint8_t SX126x_GetRssiInst(void)
{
    uint8_t buf[2];
    SX126x_ReadCommand(SX126X_CMD_GET_RSSI_INST, buf, 2); // 0x15
    return buf[1];
}

void SX126x_GetRxBufferStatus(uint8_t *payloadLength, uint8_t *rxStartBufferPointer)
{
    uint8_t buf[3];
    SX126x_ReadCommand(SX126X_CMD_GET_RX_BUFFER_STATUS, buf, 3); // 0x13
    *payloadLength=buf[1];
    *rxStartBufferPointer=buf[2];
}

void SX126x_WaitForIdleBegin(uint32_t timeout_ms, char *text)
{
    // ensure BUSY is low (state meachine ready)
    bool stop=false;
    for(int retry=0; retry<10; retry++)
    {
        if(retry==9) stop=true;
        bool ret=SX126x_WaitForIdle(BUSY_WAIT, text, stop);
        if(ret==true) break;
        __dbsi("\r\n[SX126x] SX126x_WaitForIdle fail retry=", retry);
        system_wait();
    }
}

bool SX126x_WaitForIdle(uint32_t timeout_ms, char *text, bool stop)
{
    bool ret=true;
    uint32_t start=sx126x_get_system_tick_ms();
    //delayMicroseconds(1);
    while(sx126x_get_system_tick_ms()-start<timeout_ms)
    {
        if(sx126x_gpio_get_level(SX126x_BUSY)==0) break;
        //delayMicroseconds(1);
        // Give up CPU execution rights
        system_wait();
    }

    if(sx126x_gpio_get_level(SX126x_BUSY))
    {
        if(stop)
        {
            __dbss("\r\n[SX126x] SX126x_WaitForIdle Timeout text=", text);
            __dbsu(" timeout_ms=", timeout_ms);
            __dbsu(" start=", start);
            LoRaError(ERR_IDLE_TIMEOUT);
        }
        else
        {
            __dbss("\r\n[SX126x] SX126x_WaitForIdle Timeout text=", text);
            __dbsu(" timeout_ms=", timeout_ms);
            __dbsu(" start=", start);
            ret=false;
        }
    }
    return ret;
}

uint8_t SX126x_ReadBuffer(uint8_t *rxData, int16_t rxDataLen)
{
    uint8_t offset=0;
    uint8_t payloadLength=0;
    SX126x_GetRxBufferStatus(&payloadLength, &offset);
    if(payloadLength>rxDataLen)
    {
        __dbsu("\r\n[SX126x] SX126x_ReadBuffer rxDataLen too small. payloadLength=", payloadLength);
        __dbsi(" rxDataLen=", rxDataLen);

        return 0;
    }

    // ensure BUSY is low (state meachine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start SX126x_ReadBuffer", true);

    // start transfer
    uint8_t *buf;
    buf=malloc(payloadLength+3);
    if(buf!=NULL)
    {
        buf[0]=SX126X_CMD_READ_BUFFER; // 0x1E
        buf[1]=offset; // offset in rx fifo
        buf[2]=SX126X_CMD_NOP;
        memset(&buf[3], SX126X_CMD_NOP, payloadLength);
        sx126x_spi_exchange(buf, buf, payloadLength+3);
        memcpy(rxData, &buf[3], payloadLength);
        free(buf);
    }
    else
    {
        __dbs("\r\n[SX126x] SX126x_ReadBuffer malloc fail");
        payloadLength=0;
    }

    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end SX126x_ReadBuffer", false);

    return payloadLength;
}

void SX126x_WriteBuffer(uint8_t *txData, int16_t txDataLen)
{
    uint8_t buf[LORA_MSG_LEN_MAX+2];

    if(txDataLen>LORA_MSG_LEN_MAX)
    {
        __dbs("\r\n[SX126x] SX126x_WriteBuffer out of memory");
        return;
    }

    // ensure BUSY is low (state meachine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start SX126x_WriteBuffer", true);

    // start transfer
    buf[0]=SX126X_CMD_WRITE_BUFFER; // 0x0E
    buf[1]=0; // offset in tx fifo
    memcpy(&buf[2], txData, (size_t) txDataLen);
    spi_write_byte(buf, (size_t) (txDataLen+2));

    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end SX126x_WriteBuffer", false);
}

void SX126x_WriteRegister(uint16_t reg, uint8_t* data, uint8_t numBytes)
{
    // ensure BUSY is low (state meachine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start SX126x_WriteRegister", true);

    if(debugPrint)
    {
        __dbs("\r\n[SX126x] SX126x_WriteRegister: REG=");
        __dbh2(reg);

        for(uint8_t n=0; n<numBytes; n++)
        {
            __dbs("\r\n[SX126x] DataOut:");
            __dbh2(data[n]);
        }
    }

    // start transfer
    uint8_t buf[16];
    buf[0]=SX126X_CMD_WRITE_REGISTER;
    buf[1]=(reg&0xFF00)>>8;
    buf[2]=reg&0xff;
    memcpy(&buf[3], data, numBytes);
    spi_write_byte(buf, 3+numBytes);

    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end SX126x_WriteRegister", false);
}

void SX126x_ReadRegister(uint16_t reg, uint8_t* data, uint8_t numBytes)
{
    // ensure BUSY is low (state meachine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start SX126x_ReadRegister", true);

    if(debugPrint)
    {
        __dbs("\r\n[SX126x] SX126x_ReadRegister: REG=");
        __dbh2(reg);
    }

    // start transfer
    uint8_t buf[16];
    memset(buf, SX126X_CMD_NOP, sizeof (buf));
    buf[0]=SX126X_CMD_READ_REGISTER;
    buf[1]=(reg&0xFF00)>>8;
    buf[2]=reg&0xff;
    sx126x_spi_exchange(buf, buf, 4+numBytes);
    memcpy(data, &buf[4], numBytes);
    if(debugPrint)
    {
        for(uint8_t n=0; n<numBytes; n++)
        {
            __dbs("\r\n[SX126x] DataIn:");
            __dbh2(data[n]);
        }
    }

    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end SX126x_ReadRegister", false);
}

// SX126x_WriteCommand with retry

void SX126x_WriteCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes)
{
    uint8_t status;
    for(int retry=1; retry<10; retry++)
    {
        status=SX126x_WriteCommand2(cmd, data, numBytes);
        __dbs("\r\n[SX126x] status=");
        __dbh2(status);

        if(status==0) break;
        __dbs("\r\n[SX126x] SX126x_WriteCommand2 status=");
        __dbh2(status);
        __dbsi(" retry=", retry);
    }
    if(status!=0)
    {
        __dbs("\r\n[SX126x] SPI Transaction error:");
        __dbh2(status);
        LoRaError(ERR_SPI_TRANSACTION);
    }
}

uint8_t SX126x_WriteCommand2(uint8_t cmd, uint8_t* data, uint8_t numBytes)
{
    // ensure BUSY is low (state meachine ready)
    SX126x_WaitForIdle(BUSY_WAIT, "start SX126x_WriteCommand2", true);

    if(debugPrint)
    {
        __dbs("\r\n[SX126x] SX126x_WriteCommand: CMD=");
        __dbh2(cmd);
    }

    // start transfer
    uint8_t buf[16];
    buf[0]=cmd;
    memcpy(&buf[1], data, numBytes);
    sx126x_spi_exchange(buf, buf, numBytes+1);

    uint8_t status=0;
    uint8_t cmd_status=buf[1] & 0xe;

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
            // default: break; // success
    }

    // wait for BUSY to go low
    SX126x_WaitForIdle(BUSY_WAIT, "end SX126x_WriteCommand2", false);
    return status;
}

void SX126x_ReadCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes)
{
    // ensure BUSY is low (state meachine ready)
    SX126x_WaitForIdleBegin(BUSY_WAIT, "start SX126x_ReadCommand");

    if(debugPrint)
    {
        __dbs("\r\n[SX126x] SX126x_ReadCommand: CMD=");
        __dbh2(cmd);
    }

    // start transfer
    uint8_t buf[16];
    memset(buf, SX126X_CMD_NOP, sizeof (buf));
    buf[0]=cmd;
    sx126x_spi_exchange(buf, buf, 1+numBytes);
    if(data!=NULL&&numBytes)
        memcpy(data, &buf[1], numBytes);

    // wait for BUSY to go low
    system_wait();
    SX126x_WaitForIdle(BUSY_WAIT, "end SX126x_ReadCommand", false);
}
