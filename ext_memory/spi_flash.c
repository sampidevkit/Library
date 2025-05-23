#include "spi_flash.h"

#if(0)
#include <stdio.h>

#define debug(...)   printf(__VA_ARGS__)
#else
#define debug(...)
#endif

#define SPI_Flash_Make_Address(address) do{iCmd[1]=(uint8_t) (address>>16); iCmd[2]=(uint8_t) (address>>8); iCmd[3]=(uint8_t) address;}while(0)

private uint8_t iCmd[6];

/* [Chip select On/Off=1/0][data buffer][write length][read length][Chip select On/Off=1/0] */
private void SPI_Xfer(bool enCS, uint8_t *data, uint8_t lenIn, uint8_t lenOut, bool dnCS)
{
    uint8_t i;

    if(enCS)
    {
        SPI_Flash_Open();
        SPI_Flash_NCS_SetLow();
    }

    for(i=0; i<lenIn; i++)
        SPI_Flash_Exchange8bit(data[i]);

    for(i=0; i<lenOut; i++)
        data[i]=SPI_Flash_Exchange8bit(0xFF);

    if(dnCS)
    {
        SPI_Flash_NCS_SetHigh();
        SPI_Flash_Close();
    }
}

private void SPI_Flash_Enable_Write(void)// Private function
{
    iCmd[0]=SST_WRITE_EN;
    SPI_Xfer(1, iCmd, 1, 0, 1);
}

private void SPI_Flash_Disable_Write(void)// Private function
{
    iCmd[0]=SST_WRITE_DIS;
    SPI_Xfer(1, iCmd, 1, 0, 1);
}

private void SPI_Flash_Poll_WR_Status(void)// Private function
{
    iCmd[0]=SST_POLL_WR_STATUS;
    SPI_Xfer(1, iCmd, 1, 0, 1);
}

private uint8_t SPI_Flash_Read_Status(void)// Private function
{
    iCmd[0]=SST_READ_STATUS;
    SPI_Xfer(1, iCmd, 1, 1, 1);

    return iCmd[0];
}

private void SPI_Flash_Write_Status(uint8_t stt)// Private function
{
    iCmd[0]=SST_WRITE_STATUS_EN;
    SPI_Xfer(1, iCmd, 1, 0, 1);
    iCmd[0]=SST_WRITE_STATUS;
    iCmd[1]=stt;
    SPI_Xfer(1, iCmd, 2, 0, 1);
}

private void SPI_Flash_Check_Busy(void)// Private function
{
    while(SPI_Flash_Read_Status()&0x01);
}

public void SPI_Flash_Chip_Erase(void)// Public function
{
    SPI_Flash_Enable_Write();
    iCmd[0]=SST_BULK_ERASE;
    SPI_Xfer(1, iCmd, 1, 0, 1);
    SPI_Flash_Check_Busy();
}

public void SPI_Flash_Sector_Erase(uint32_t BAddr)// Public function
{
    SPI_Flash_Enable_Write();
    iCmd[0]=SST_SECTOR_ERASE;
    SPI_Flash_Make_Address(BAddr);
    SPI_Xfer(1, iCmd, 4, 0, 1);
    SPI_Flash_Check_Busy();
}

public void SPI_Flash_Read_nByte(uint32_t BAddr, uint32_t len, uint8_t *buffer)// Public function
{
    SPI_Flash_Check_Busy();
    iCmd[0]=SST_FAST_READ_DATA;
    SPI_Flash_Make_Address(BAddr);
    iCmd[4]=0x00;
    SPI_Xfer(1, iCmd, 5, 0, 0);
    SPI_Xfer(0, buffer, 0, len, 1);
}

public void SPI_Flash_Write_Byte(uint32_t Addr, uint8_t data)// Public function
{
    SPI_Flash_Check_Busy();
    SPI_Flash_Enable_Write();
    iCmd[0]=SST_BYTE_PROG;
    SPI_Flash_Make_Address(Addr);
    iCmd[4]=data;
    SPI_Xfer(1, iCmd, 5, 0, 1);
    SPI_Flash_Disable_Write();
}

public void SPI_Flash_Write_nByte(uint32_t BAddr, uint16_t len, const uint8_t *data)// Public function
{
    uint16_t i;
#if(SST_USE_AAI)
    // Auto Address Increment (AAI) Word-Program
    SPI_Flash_Check_Busy();
    SPI_Flash_Enable_Write();
    iCmd[0]=SST_WORD_PROG;
    SPI_Flash_Make_Address(BAddr);
    iCmd[4]=data[0];
    iCmd[5]=data[1];
    SPI_Xfer(1, iCmd, 6, 0, 1);
    SPI_Flash_Check_Busy();

    for(i=2; i<len; i+=2)
    {
        iCmd[0]=SST_WORD_PROG;
        iCmd[1]=data[i];
        iCmd[2]=data[i+1];
        SPI_Xfer(1, iCmd, 3, 0, 1);
        SPI_Flash_Check_Busy();
    }

    SPI_Flash_Disable_Write();
#else
    for(i=0; i<len; i++)
        SPI_Flash_Write_Byte(BAddr+i, *data++);
#endif
}

public void SPI_Flash_Protect(sst_protect_range_t range)// Private function
{
    SPI_Flash_Enable_Write();
    iCmd[0]=SST_WRITE_STATUS;
    iCmd[1]=FLASH_PROTECT_UNLOCK;
    SPI_Xfer(1, iCmd, 2, 0, 1);
    SPI_Flash_Enable_Write();
    iCmd[0]=range|FLASH_PROTECT_LOCK;
    SPI_Xfer(1, iCmd, 1, 0, 1);
}

public bool SPI_Flash_Init(void)// Public function
{
    uint8_t tryTimes=0;

LOOP:
    iCmd[0]=SST_READ_JDECID;
    SPI_Xfer(1, iCmd, 1, 3, 1);

    if((iCmd[0]!=JEDEC_ID_BYTE0)||(iCmd[1]!=JEDEC_ID_BYTE1)||(iCmd[2]!=JEDEC_ID_BYTE2))// SST JDEC ID is always 0xBF25xx
    {
        debug("\n%02X%02X%02X", iCmd[0], iCmd[1], iCmd[2]);

        if(++tryTimes==0)
            return 0; // Try 256 times after return error

        goto LOOP;
    }

    SPI_Flash_Write_Status(0);

    return 1;
}