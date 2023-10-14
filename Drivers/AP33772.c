#include "AP33772.h"

#ifdef USE_AP33772_DEBUG
#include "Common/debug.h"
#else
#define __dbc(c)                
#define __dbs(str)              
#define __dbi(x)                
#define __dbh(x, dg)            
#define __dbh2(x)               
#define __dbhs(str, len)        
#define __dbdata(str, len)      

#define __dbsi(str, x)          
#define __dbsh(str, x)          
#define __dbsc(str, c)          
#define __dbss(str1, str2)      
#define __dbstime(str, tcxt)    
#define __dbsdate(str, tcxt)    
#define __dbsdata(str, pd, len) 
#endif

uint8_t readBuf[READ_BUFF_LENGTH]={0};
uint8_t writeBuf[WRITE_BUFF_LENGTH]={0};
uint8_t numPDO=0; // source PDO number
uint8_t indexPDO=0; // PDO index, start from index 0
int reqPpsVolt=0; // requested PPS voltage, unit:20mV
uint8_t existPPS=0; // PPS flag for setVoltage()
int8_t PPSindex=0;

AP33772_STATUS_T ap33772_status={0};
EVENT_FLAG_T event_flag={0};
RDO_DATA_T rdoData={0};
PDO_DATA_T pdoData[7]={0};

void AP33772_Init(void) // <editor-fold defaultstate="collapsed" desc="Initialize">
{
    uint8_t i=0;

    // Reset chip
    writeBuf[i++]=CMD_RDO;
    writeBuf[i++]=0x00;
    writeBuf[i++]=0x00;
    writeBuf[i++]=0x00;
    writeBuf[i++]=0x00;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);

    i=0;
    writeBuf[i++]=CMD_STATUS;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
    AP33772_I2C_Read(AP33772_ADDRESS, readBuf, 1); // CMD: Read Status
    ap33772_status.readStatus=readBuf[0];

    if(ap33772_status.isOvp)
        event_flag.ovp=1;
    if(ap33772_status.isOcp)
        event_flag.ocp=1;

    if(ap33772_status.isReady) // negotiation finished
    {
        if(ap33772_status.isNewpdo) // new PDO
        {
            if(ap33772_status.isSuccess) // negotiation success
                event_flag.newNegoSuccess=1;
            else
                event_flag.newNegoFail=1;
        }
        else
        {
            if(ap33772_status.isSuccess)
                event_flag.negoSuccess=1;
            else
                event_flag.negoFail=1;
        }
    }

    AP33772_DelayMs(10);

    // If negotiation is good, let's load in some PDOs from charger
    if(event_flag.newNegoSuccess)
    {
        event_flag.newNegoSuccess=0;

        i=0;
        writeBuf[i++]=CMD_PDONUM;
        AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
        AP33772_I2C_Read(AP33772_ADDRESS, readBuf, 1); // CMD: Read PDO Number
        numPDO=readBuf[0];

        i=0;
        writeBuf[i++]=CMD_SRCPDO;
        AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
        AP33772_I2C_Read(AP33772_ADDRESS, readBuf, SRCPDO_LENGTH); // CMD: Read PDOs

        // copy PDOs to pdoData[]
        for(i=0; i<numPDO; i++)
        {
            pdoData[i].byte0=readBuf[i<<2]; // i*4
            pdoData[i].byte1=readBuf[(i<<2)+1]; // i*4+1
            pdoData[i].byte2=readBuf[(i<<2)+2]; // i*4+2
            pdoData[i].byte3=readBuf[(i<<2)+3]; // i*4+3

            if((pdoData[i].byte3&0xF0)==0xC0) // PPS profile found
            {
                PPSindex=i; // Store index
                existPPS=1; // Turn on flag
            }
        }
    }
} // </editor-fold>

void AP33772_setVoltage(int targetVoltage) // <editor-fold defaultstate="collapsed" desc="Set VBUS voltage, targetVoltage in mV">
{
    /*
    Step 1: Check if PPS can satify request voltage
    Step 2: Scan PDOs to see what is the lower closest voltage to request
    Step 3: Compare found PDOs votlage and PPS max voltage
     */
    uint8_t i, tempIndex=0;

    if(existPPS)
    {
        if((pdoData[PPSindex].pps.maxVoltage*100>=targetVoltage) && (pdoData[PPSindex].pps.minVoltage*100<=targetVoltage)) // PPS exist, voltage satify
        {
            indexPDO=PPSindex;
            reqPpsVolt=targetVoltage/20; // Unit in 20mV/LBS
            rdoData.pps.objPosition=PPSindex+1; // index 1
            rdoData.pps.opCurrent=pdoData[PPSindex].pps.maxCurrent;
            rdoData.pps.voltage=reqPpsVolt;
            AP33772_writeRDO();
            return;
        }
    }
    else
    {
        // Step 2: Scan PDOs to see what is the lower closest voltage to request
        for(i=0; i<numPDO-existPPS; i++)
        {
            if(pdoData[i].fixed.voltage*50<=targetVoltage)
                tempIndex=i;
        }
        // Step 3: Compare found PDOs votlage and PPS max voltage
        if(pdoData[tempIndex].fixed.voltage*50>pdoData[PPSindex].pps.maxVoltage*100)
        {
            indexPDO=tempIndex;
            rdoData.fixed.objPosition=tempIndex+1; // Index 0 to Index 1
            rdoData.fixed.maxCurrent=pdoData[indexPDO].fixed.maxCurrent;
            rdoData.fixed.opCurrent=pdoData[indexPDO].fixed.maxCurrent;
            AP33772_writeRDO();
            return;
        }
        else // If PPS voltage larger or equal to Fixed PDO
        {
            indexPDO=PPSindex;
            reqPpsVolt=pdoData[PPSindex].pps.maxVoltage*5; // Convert unit: 100mV -> 20mV
            rdoData.pps.objPosition=PPSindex+1; // index 1
            rdoData.pps.opCurrent=pdoData[PPSindex].pps.maxCurrent;
            rdoData.pps.voltage=reqPpsVolt;
            AP33772_writeRDO();
            return;
        }
    }
} // </editor-fold>

void AP33772_setMaxCurrent(int targetMaxCurrent) // <editor-fold defaultstate="collapsed" desc="Set max current before tripping at wall plug, targetMaxCurrent in mA">
{
    /*
    Step 1: Check if current profile is PPS, check if max current is lower than request
        If yes, set new max current
        If no, report fault
    Step 2: If profile is PDO, check if max current is lower than request
        If yes, set new max current
        If no, report fault
     */

    if(indexPDO==PPSindex)
    {
        if(targetMaxCurrent<=pdoData[PPSindex].pps.maxCurrent*50)
        {
            rdoData.pps.objPosition=PPSindex+1; // index 1
            rdoData.pps.opCurrent=targetMaxCurrent/50; // 50mA/LBS
            rdoData.pps.voltage=reqPpsVolt;
            AP33772_writeRDO();
        }
    }
    else if(targetMaxCurrent<=pdoData[indexPDO].fixed.maxCurrent*10)
    {
        rdoData.fixed.objPosition=indexPDO+1; // Index 0 to Index 1
        rdoData.fixed.maxCurrent=targetMaxCurrent/10; // 10mA/LBS
        rdoData.fixed.opCurrent=targetMaxCurrent/10; // 10mA/LBS
        AP33772_writeRDO();
    }
} // </editor-fold>

void AP33772_setNTC(int TR25, int TR50, int TR75, int TR100) // <editor-fold defaultstate="collapsed" desc="Set resistance value of 10K NTC">
{
    /**
     * @brief Set resistance value of 10K NTC at 25C, 50C, 75C and 100C.
     *          Default is 10000, 4161, 1928, 974Ohm
     * @param TR25, TR50, TR75, TR100 unit in Ohm
     * @attention Blocking function due to long I2C write, min blocking time 15ms
     */
    uint8_t i=0;

    writeBuf[i++]=CMD_TR25;
    writeBuf[i++]=TR25&0xff;
    writeBuf[i++]=(TR25>>8) & 0xff;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);

    AP33772_DelayMs(5);

    i=0;
    writeBuf[i++]=CMD_TR50;
    writeBuf[i++]=TR50&0xff;
    writeBuf[i++]=(TR50>>8) & 0xff;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);

    AP33772_DelayMs(5);

    i=0;
    writeBuf[i++]=CMD_TR75;
    writeBuf[i++]=TR75&0xff;
    writeBuf[i++]=(TR75>>8) & 0xff;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);

    AP33772_DelayMs(5);

    i=0;
    writeBuf[i++]=CMD_TR100;
    writeBuf[i++]=TR100&0xff;
    writeBuf[i++]=(TR100>>8) & 0xff;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
} // </editor-fold>

void AP33772_setDeratingTemp(int temperature) // <editor-fold defaultstate="collapsed" desc="Set target temperature (C) when output power through USB-C is reduced, temperature (unit in Celcius)">
{
    // Default is 120 C
    uint8_t i=0;

    writeBuf[i++]=CMD_DRTHR;
    writeBuf[i++]=temperature;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
} // </editor-fold>

void AP33772_setOTPThreshold(int temperature) // <editor-fold defaultstate="collapsed" desc="Set target temperature (C) when over temperature, temperature (unit in Celcius)">
{
    uint8_t i=0;

    writeBuf[i++]=CMD_OTPTHR;
    writeBuf[i++]=temperature;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
} // </editor-fold>

void AP33772_setMask(AP33772_MASK flag) // <editor-fold defaultstate="collapsed" desc="Set mask">
{
    uint8_t i=0;
    // First read in what is currently in the MASK
    writeBuf[i++]=CMD_MASK;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
    AP33772_I2C_Read(AP33772_ADDRESS, readBuf, 1);
    AP33772_DelayMs(10); // Short break between read/write
    i=0;
    writeBuf[i++]=CMD_MASK;
    writeBuf[i++]=flag|readBuf[0]; // WRONG!, will not be able to turn thing off
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
} // </editor-fold>

void AP33772_writeRDO(void) // <editor-fold defaultstate="collapsed" desc="Write the desire power profile back to the power source">
{
    uint8_t i=0;

    writeBuf[i++]=CMD_RDO;
    writeBuf[i++]=rdoData.byte0;
    writeBuf[i++]=rdoData.byte1;
    writeBuf[i++]=rdoData.byte2;
    writeBuf[i++]=rdoData.byte3;

    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i); // CMD: Write RDO
} // </editor-fold>

int AP33772_readVoltage(void) // <editor-fold defaultstate="collapsed" desc="Read VBUS voltage in mV">
{
    uint8_t i=0;

    writeBuf[i++]=CMD_VOLTAGE;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
    AP33772_I2C_Read(AP33772_ADDRESS, readBuf, 1);

    return ((int) readBuf[0]*80); // I2C read return 80mV/LSB
} // </editor-fold>

int AP33772_readCurrent(void) // <editor-fold defaultstate="collapsed" desc="Read VBUS current in mA">
{
    uint8_t i=0;

    writeBuf[i++]=CMD_CURRENT;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
    AP33772_I2C_Read(AP33772_ADDRESS, readBuf, 1);

    return ((int) readBuf[0]<<4); // readBuf[0] * 16, I2C read return 24mA/LSB
} // </editor-fold>

int AP33772_readTemp(void) // <editor-fold defaultstate="collapsed" desc="Read NTC temperature in C degree">
{
    uint8_t i=0;

    writeBuf[i++]=CMD_TEMP;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
    AP33772_I2C_Read(AP33772_ADDRESS, readBuf, 1);

    return readBuf[0]; // I2C read return 1C/LSB
} // </editor-fold>

void AP33772_reset(void) // <editor-fold defaultstate="collapsed" desc="Hard reset the power supply. Will temporary cause power outage">
{
    uint8_t i=0;

    writeBuf[i++]=CMD_RDO;
    writeBuf[i++]=0x00;
    writeBuf[i++]=0x00;
    writeBuf[i++]=0x00;
    writeBuf[i++]=0x00;
    AP33772_I2C_Write(AP33772_ADDRESS, writeBuf, i);
} // </editor-fold>

void AP33772_printPDO(void) // <editor-fold defaultstate="collapsed" desc="Debug code to quickly check power supply profile PDOs">
{
    uint8_t i;

    __dbsi("\nSource PDO Number= ", numPDO);

    for(i=0; i<numPDO; i++)
    {
        if((pdoData[i].byte3&0xF0)==0xC0) // PPS PDO
        {
            __dbsi("\nPDO[", i+1); // PDO position start from 1
            __dbsi("]-PPS: ", pdoData[i].pps.minVoltage/10); // pdoData[i].pps.minVoltage*100/1000
            __dbsi("V ~ ", pdoData[i].pps.maxVoltage/10); // pdoData[i].pps.maxVoltage*100/1000
            __dbsi("V @ ", pdoData[i].pps.maxCurrent/20); // pdoData[i].pps.maxCurrent*50/1000
            __dbs("A");
        }
        else if((pdoData[i].byte3&0xC0)==0x00) // Fixed PDO
        {
            __dbsi("\nPDO[", i+1);
            __dbsi("]-Fixed: ", pdoData[i].fixed.voltage/20); // pdoData[i].fixed.voltage*50/1000
            __dbsi("V @ ", pdoData[i].fixed.maxCurrent/100); // pdoData[i].fixed.maxCurrent*10/1000
            __dbs("A");
        }
    }

    __dbs("\n===============================================\n");
} // </editor-fold>