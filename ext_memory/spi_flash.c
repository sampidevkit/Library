#include "spi_flash.h"
#include "system/system_tick.h"

#ifdef USE_SPI_FLASH_DEBUG
#include "Common/debug.h"
#else
#define __dbs(...)
#define __dbsh(...)
#define __dbh2(...)
#define __dbdata(...)
#endif

// System CMD
#define SPI_FLASH_READ_JDECID        0x9F
#define SPI_FLASH_WRITE_STATUS_EN    0x50
#define SPI_FLASH_WRITE_STATUS1      0x01
#define SPI_FLASH_WRITE_STATUS2      0x31
#define SPI_FLASH_WRITE_STATUS3      0x11
// Read CMD
#define SPI_FLASH_FAST_READ_DATA     0x0B
#define SPI_FLASH_READ_STATUS1       0x05
#define SPI_FLASH_READ_STATUS2       0x35
#define SPI_FLASH_READ_STATUS3       0x15
// Erase CMD
#define SPI_FLASH_SECTOR_ERASE       0x20
#define SPI_FLASH_BULK_ERASE         0xC7
// Write CMD
#define SPI_FLASH_WRITE_EN           0x06
#define SPI_FLASH_WRITE_DIS          0x04
#define SPI_FLASH_BYTE_PROG          0x02 // Page program

// Default values
#define SPI_FLASH_PROTECT_LOCK       0x80
#define SPI_FLASH_PROTECT_UNLOCK     0x00

static uint8_t spiData[8];

/* [Chip select On/Off=1/0][data buffer][write length][read length][Chip select On/Off=1/0] */
private void SPI_Xfer(bool enCS, uint8_t *data, uint16_t lenIn, uint16_t lenOut, bool dnCS) // <editor-fold defaultstate="collapsed" desc="SPI xfer data">
{
    uint16_t i;

    if(enCS==1)
        SPI_Flash_NCS_SetLow();

    for(i=0; i<lenIn; i++)
        SPI_Flash_Exchange8bit(data[i]);

    for(i=0; i<lenOut; i++)
        data[i]=SPI_Flash_Exchange8bit(0xFF);

    if(dnCS==1)
        SPI_Flash_NCS_SetHigh();
} // </editor-fold>

public uint32_t SPI_Flash_GetSectorIndex(uint32_t Addr) // <editor-fold defaultstate="collapsed" desc="Get index of a sector">
{
    return (Addr>>12);
} // </editor-fold>

public uint32_t SPI_Flash_GetSectorBeginAddress(uint32_t Addr) // <editor-fold defaultstate="collapsed" desc="Get begin address of a sector">
{
    return (Addr&0xFFFFF000);
} // </editor-fold>

public void SPI_Flash_Enable_Write(void)
{
    spiData[0]=SPI_FLASH_WRITE_EN;
    SPI_Xfer(1, spiData, 1, 0, 1);
}

public void SPI_Flash_Disable_Write(void)
{
    spiData[0]=SPI_FLASH_WRITE_DIS;
    SPI_Xfer(1, spiData, 1, 0, 1);
}

public uint8_t SPI_Flash_Read_Status(uint8_t SttRegIdx)
{
    if(SttRegIdx==0)
        spiData[0]=SPI_FLASH_READ_STATUS1;
    else if(SttRegIdx==1)
        spiData[0]=SPI_FLASH_READ_STATUS2;
    else
        spiData[0]=SPI_FLASH_READ_STATUS3;

    SPI_Xfer(1, spiData, 1, 1, 1);

    return spiData[0];
}

public void SPI_Flash_Write_Status(uint8_t SttRegIdx, uint8_t stt)
{
    spiData[0]=SPI_FLASH_WRITE_STATUS_EN;
    SPI_Xfer(1, spiData, 1, 0, 1);

    if(SttRegIdx==0)
        spiData[0]=SPI_FLASH_WRITE_STATUS1;
    else if(SttRegIdx==1)
        spiData[0]=SPI_FLASH_WRITE_STATUS2;
    else
        spiData[0]=SPI_FLASH_WRITE_STATUS3;

    spiData[1]=stt;
    SPI_Xfer(1, spiData, 2, 0, 1);
}

public void SPI_Flash_Wait_Busy(void)
{
    while((SPI_Flash_Read_Status(0)&0x01)>0) // check RDY bit
        SPI_Flash_LongWait_Task();
}

public void SPI_Flash_Make_Address(uint32_t address)
{
    spiData[1]=(uint8_t) (address>>16);
    spiData[2]=(uint8_t) (address>>8);
    spiData[3]=(uint8_t) address;
}

public bool SPI_Flash_Chip_Erase(void)
{
    static bool busy=0;

    if(busy==0)
    {
        SPI_Flash_Enable_Write();
        spiData[0]=SPI_FLASH_BULK_ERASE;
        SPI_Xfer(1, spiData, 1, 0, 1);
        busy=1;
    }
    else if((SPI_Flash_Read_Status(0)&0x03)==0) // check WEL & RDY bits
    {
        busy=0;
        SPI_Flash_Disable_Write();
    }

    return !busy;
}

public void SPI_Flash_Sector_Erase(uint32_t BAddr)
{
    SPI_Flash_Enable_Write();
    spiData[0]=SPI_FLASH_SECTOR_ERASE;
    SPI_Flash_Make_Address(SPI_Flash_GetSectorBeginAddress(BAddr));
    SPI_Xfer(1, spiData, 4, 0, 1);
    SPI_Flash_Wait_Busy();
    SPI_Flash_Disable_Write();
}

public void SPI_Flash_Read_nByte(uint32_t BAddr, uint16_t len, uint8_t *buffer)
{
    SPI_Flash_Wait_Busy();
    spiData[0]=SPI_FLASH_FAST_READ_DATA;
    SPI_Flash_Make_Address(BAddr);
    spiData[4]=0x00;
    SPI_Xfer(1, spiData, 5, 0, 0);
    SPI_Xfer(0, buffer, 0, len, 1);
}

public uint8_t SPI_Flash_Read_Byte(uint32_t BAddr)
{
    uint8_t data;

    SPI_Flash_Wait_Busy();
    spiData[0]=SPI_FLASH_FAST_READ_DATA;
    SPI_Flash_Make_Address(BAddr);
    spiData[4]=0x00;
    SPI_Xfer(1, spiData, 5, 0, 0);
    SPI_Xfer(0, &data, 0, 1, 1);

    return data;
}

public void SPI_Flash_Write_Byte(uint32_t Addr, uint8_t data)
{
    SPI_Flash_Wait_Busy();
    SPI_Flash_Enable_Write();
    spiData[0]=SPI_FLASH_BYTE_PROG;
    SPI_Flash_Make_Address(Addr);
    spiData[4]=data;
    SPI_Xfer(1, spiData, 5, 0, 1);
    SPI_Flash_Wait_Busy();
    SPI_Flash_Disable_Write();
}

public void SPI_Flash_Write_nByte(uint32_t BAddr, uint16_t len, uint8_t *data)
{
#ifdef SPI_FLASH_AAI_WORD_PROG // Auto Address Increment (AAI) Word-Program
    uint16_t i;

    SPI_Flash_Wait_Busy();
    SPI_Flash_Enable_Write();
    spiData[0]=SPI_FLASH_AAI_WORD_PROG;
    SPI_Flash_Make_Address(BAddr);
    spiData[4]=data[0];
    spiData[5]=data[1];
    SPI_Xfer(1, spiData, 6, 0, 1);
    SPI_Flash_Wait_Busy();

    for(i=2; i<len; i+=2)
    {
        spiData[0]=SPI_FLASH_AAI_WORD_PROG;
        spiData[1]=data[i];
        spiData[2]=data[i+1];
        SPI_Xfer(1, spiData, 3, 0, 1);
        SPI_Flash_Wait_Busy();
    }

    SPI_Flash_Disable_Write();
#else // Quad Page Program
    uint16_t i=0;

LOOP:
    SPI_Flash_Wait_Busy();
    SPI_Flash_Enable_Write();
    spiData[0]=SPI_FLASH_BYTE_PROG;
    SPI_Flash_Make_Address(BAddr);
    __dbsh("\nWrite @", BAddr);
    __dbs(": ");
    SPI_Xfer(1, spiData, 4, 0, 0);

    if(len>256)
    {
        SPI_Xfer(0, &data[i], 256, 0, 1);
        __dbdata(&data[i], 256);
        BAddr+=256;
        i+=256;
        len-=256;
        SPI_Flash_Wait_Busy();
        SPI_Flash_Disable_Write();

        goto LOOP;
    }
    else
    {
        SPI_Xfer(0, &data[i], len, 0, 1);
        __dbdata(&data[i], len);
        SPI_Flash_Wait_Busy();
        SPI_Flash_Disable_Write();
    }
#endif
}

public void SPI_Flash_Protect(uint8_t BPval)
{
    uint16_t i=0;

    SPI_Flash_Enable_Write();
    spiData[i++]=SPI_FLASH_WRITE_STATUS1;
    spiData[i++]=SPI_FLASH_PROTECT_UNLOCK;
    SPI_Xfer(1, spiData, i, 0, 1);
    SPI_Flash_Wait_Busy();

    i=0;
    SPI_Flash_Enable_Write();
    spiData[i++]=SPI_FLASH_WRITE_STATUS1;
    spiData[i++]=(BPval<<2)|SPI_FLASH_PROTECT_LOCK;
    SPI_Xfer(1, spiData, i, 0, 1);
    SPI_Flash_Wait_Busy();
}

public uint32_t SPI_Flash_Get_JDECID(void)
{
    uint32_t ID;

    spiData[0]=SPI_FLASH_READ_JDECID;
    SPI_Xfer(1, spiData, 1, 3, 1);
    ID=spiData[0];
    ID<<=8;
    ID|=spiData[1];
    ID<<=8;
    ID|=spiData[2];

    return ID;
}

public int8_t SPI_Flash_Init(void)
{
    uint8_t tryTimes=0;

    SPI_Flash_PWREN_Enable();
    SPI_Flash_NCS_SetHigh();

    if(SPI_Flash_Open()==0)
    {
        SPI_Flash_PWREN_Disable();
        SPI_Flash_NCS_SetLow();

        return SPI_FLASH_RSLT_BUSY;
    }

LOOP:
    spiData[0]=SPI_FLASH_READ_JDECID;
    SPI_Xfer(1, spiData, 1, 3, 1);

    if((spiData[0]!=SPI_FLASH_JDECID)||(spiData[1]!=SPI_FLASH_DEVICE_ID1)||(spiData[2]!=SPI_FLASH_DEVICE_ID2))// SPI_Flash JDEC ID is always 0xBF25xx
    {
        if(++tryTimes==0)
        {
            __dbs("\nSPI_Flash ID: ");
            __dbh2(spiData[0]);
            __dbh2(spiData[1]);
            __dbh2(spiData[2]);

            return SPI_FLASH_RSLT_ERR; // Try 256 times after return error
        }

        goto LOOP;
    }

    SPI_Flash_Write_Status(0, 0); // BP0,1,2,3: not write protect, read/writable

    return SPI_FLASH_RSLT_OK;
}

public void SPI_Flash_Deinit(void)
{
    SPI_Flash_Wait_Busy();
    SPI_Flash_Close();
    SPI_Flash_PWREN_Disable();
    SPI_Flash_NCS_SetLow();
}
