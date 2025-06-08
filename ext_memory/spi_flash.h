#ifndef SPI_FLASH_H
#define	SPI_FLASH_H

#include "common/libdef.h"
#include "project_cfg.h"

#if defined USE_SST25VF020B 
#define MEMORY_CAPACITY         0x040000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x25
#define SPI_FLASH_DEVICE_ID2    0x8C
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25VF040B
#define MEMORY_CAPACITY         0x080000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x25
#define SPI_FLASH_DEVICE_ID2    0x8D
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25VF080B
#define MEMORY_CAPACITY         0x100000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x25
#define SPI_FLASH_DEVICE_ID2    0x8E
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25VF016B
#define MEMORY_CAPACITY         0x200000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x25
#define SPI_FLASH_DEVICE_ID2    0x41
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25VF032B
#define MEMORY_CAPACITY         0x400000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x01
#define SPI_FLASH_DEVICE_ID2    0x4A
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25PF020B
#define MEMORY_CAPACITY         0x040000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x25
#define SPI_FLASH_DEVICE_ID2    0x8C
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25PF040B
#define MEMORY_CAPACITY         0x080000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x25
#define SPI_FLASH_DEVICE_ID2    0x8D
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25PF080B
#define MEMORY_CAPACITY         0x100000
#define SPI_FLASH_JDECID        0xBF
#define SPI_FLASH_DEVICE_ID1    0x25
#define SPI_FLASH_DEVICE_ID2    0x8E
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_SST25PF040C
#define MEMORY_CAPACITY         0x080000
#define SPI_FLASH_JDECID        0x62
#define SPI_FLASH_DEVICE_ID1    0x06
#define SPI_FLASH_DEVICE_ID2    0x13
#define SPI_FLASH_AAI_WORD_PROG 0xAD // AAI command
#elif defined USE_AT25SF161B
#define MEMORY_CAPACITY         0x200000
#define SPI_FLASH_JDECID        0x1F
#define SPI_FLASH_DEVICE_ID1    0x86
#define SPI_FLASH_DEVICE_ID2    0x01
#else
#error "Not support your device"
#endif
/* ******************************************************* EXTERNAL FUNCTIONS */
void SPI_Flash_PWREN_Enable(void);
void SPI_Flash_PWREN_Disable(void);
bool SPI_Flash_Open(void);
void SPI_Flash_Close(void);
void SPI_Flash_NCS_SetLow(void);
void SPI_Flash_NCS_SetHigh(void);
uint8_t SPI_Flash_Exchange8bit(uint8_t b);
void SPI_Flash_LongWait_Task(void);
/* ********************************************************* PUBLIC FUNCTIONS */
// address= 0x00000 to MAX_MEM_ADDR
// range= see in FLASH_PROTECT_RANGE
// len=1 to 256, if over 256 bytes, this function will not execute

#define SPI_FLASH_RSLT_OK    0
#define SPI_FLASH_RSLT_ERR   (-1)
#define SPI_FLASH_RSLT_BUSY  1

public int8_t SPI_Flash_Init(void); // Call this function first (SPI must be initialized before)

public void SPI_Flash_Deinit(void); // Call last
public uint32_t SPI_Flash_Get_JDECID(void); // Get JEDECID
public void SPI_Flash_Protect(uint8_t BPval); // Protect memory zone

public void SPI_Flash_Make_Address(uint32_t address);
public uint32_t SPI_Flash_GetSectorBeginAddress(uint32_t Addr);
public uint32_t SPI_Flash_GetSectorIndex(uint32_t Addr);
public void SPI_Flash_Wait_Busy(void);
public void SPI_Flash_Write_Status(uint8_t SttRegIdx, uint8_t stt);
public uint8_t SPI_Flash_Read_Status(uint8_t SttRegIdx);
public void SPI_Flash_Disable_Write(void);
public void SPI_Flash_Enable_Write(void);

public bool SPI_Flash_Chip_Erase(void); // Erase full-memory
public void SPI_Flash_Sector_Erase(uint32_t BAddr); // Erase a sector

public uint8_t SPI_Flash_Read_Byte(uint32_t BAddr); // Read 1 byte
public void SPI_Flash_Read_nByte(uint32_t BAddr, uint16_t len, uint8_t *buffer); // Read n byte(s)

public void SPI_Flash_Write_Byte(uint32_t Addr, uint8_t data); // Write 1 byte
public void SPI_Flash_Write_nByte(uint32_t BAddr, uint16_t len, uint8_t *data); // Write n byte(s)

#endif