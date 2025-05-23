#ifndef SPI_FLASH_H
#define	SPI_FLASH_H

#include "common/libdef.h"
#include "project_cfg.h"

/* **********************************************Please to read these documents:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/S71417_03.pdf
 * http://ww1.microchip.com/downloads/en/DeviceDoc/20005051C.pdf
 ******************************************************************************/

// MAX_MEM_ADDR=(Capacity in Mbit *1024^2)/8 - 1 (address start at 0)
#define SECTOR_LENGTH   4096
#define PAGE_LENGTH     256
#define PAGE_IN_SECTOR  16

#if(SST25VF020B)
#define MAX_MEM_ADDR    0x3FFFF  
#define JEDEC_ID_BYTE0  0xBF
#define JEDEC_ID_BYTE1  0x25
#define JEDEC_ID_BYTE2  0x8C
#elif(SST25PF040B)
#define MAX_MEM_ADDR    0x7FFFF     
#define JEDEC_ID_BYTE0  0xBF
#define JEDEC_ID_BYTE1  0x25
#define JEDEC_ID_BYTE2  0x8D
#elif(SST25VF016B)
#define MAX_MEM_ADDR    0x1FFFFF 
#define JEDEC_ID_BYTE0  0xBF
#define JEDEC_ID_BYTE1  0x25
#define JEDEC_ID_BYTE2  0x41
#else

#endif

/*********************************************************************COMMANDS*/
#define SST_READ_DATA        0x03
#define SST_FAST_READ_DATA   0x0B
#define SST_SECTOR_ERASE     0x20
#define SST_SECTOR_32_ERASE  0x52
#define SST_SECTOR_64_ERASE  0xD8
#define SST_BULK_ERASE       0xC7
#define SST_BYTE_PROG        0x02
#define SST_WORD_PROG        0xAD
#define SST_READ_STATUS      0x05
#define SST_WRITE_STATUS_EN  0x50
#define SST_WRITE_STATUS     0x01
#define SST_WRITE_EN         0x06
#define SST_WRITE_DIS        0x04
#define SST_READ_JDECID      0x9F
#define SST_HW_WR_STATUS     0x70
#define SST_POLL_WR_STATUS   0x80
#define FLASH_PROTECT_LOCK   0x80
#define FLASH_PROTECT_UNLOCK 0x00

typedef enum SST_PROTECT_RANGE {
    FLASH_PROTECT_NONE = 0,
    FLASH_PROTECT_64KB = 1,
    FLASH_PROTECT_128KB = 2,
    FLASH_PROTECT_256KB = 3,
    FLASH_PRTOECT_512KB = 4,
    FLASH_PROTECT_1MB = 5,
    FLASH_PROTECT_2MB = 6,
    FLASH_PROTECT_ALL = 7
} sst_protect_range_t;
/* ******************************************************* EXTERNAL FUNCTIONS */
void SPI_Flash_Open(void);
void SPI_Flash_NCS_SetLow(void);
uint8_t SPI_Flash_Exchange8bit(uint8_t b);
void SPI_Flash_NCS_SetHigh(void);
void SPI_Flash_Close(void);
/* ********************************************************* PUBLIC FUNCTIONS */
// address= 0x00000 to MAX_MEM_ADDR
// range= see in FLASH_PROTECT_RANGE
// len=1 to 256, if over 256 bytes, this function will not execute

public void SPI_Flash_Chip_Erase(void); // Erase full-memory
public void SPI_Flash_Sector_Erase(uint32_t BAddr); // Erase a sector
public void SPI_Flash_Read_nByte(uint32_t BAddr, uint32_t len, uint8_t *buffer); // Read n byte(s)
public void SPI_Flash_Write_nByte(uint32_t BAddr, uint16_t len, const uint8_t *data); // Write n byte(s)
public bool SPI_Flash_Init(void); // Call this function first (SPI must be initialized before)

#endif