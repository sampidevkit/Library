#ifndef BLD_EXTMEM_H
#define	BLD_EXTMEM_H

#include "Common/LibDef.h"

#ifdef COMMON_LIB_CFG
#include "Common_Lib_Cfg.h"
#else
#include "BLD_ExtMem_Cfg.h"
#endif

#define BLD_EXTMEM_CAPACITY     (BLD_EXTMEM_END-BLD_EXTMEM_BEGIN+1)
#define BLD_EXTMEM_INFO_ADDR    BLD_EXTMEM_BEGIN
#define BLD_EXTMEM_INFO_LEN     BLD_EXTMEM_SECTOR_LEN
#define BLD_EXTMEM_RESUME_ADDR  BLD_EXTMEM_BEGIN
#define BLD_EXTMEM_RESUME_LEN   BLD_EXTMEM_SECTOR_LEN
#define BLD_EXTMEM_DATA_ADDR    (BLD_EXTMEM_INFO_ADDR+BLD_EXTMEM_INFO_LEN)
#define BLD_EXTMEM_DATA_LEN     (BLD_EXTMEM_CAPACITY-BLD_EXTMEM_DATA_ADDR)

/* ************************************************************** Driver APIs */
bool BLD_ExtMem_Driver_Init(void);
uint8_t BLD_ExtMem_Driver_Read(uint32_t addr);
void BLD_ExtMem_Driver_Erase(uint32_t addr);
void BLD_ExtMem_Driver_Write(uint32_t addr, uint8_t data);

/* ********************************************************* Application APIs */
typedef enum {
    BLD_STATE_UPTODATE = 0xFF,
    BLD_STATE_DOWNLOADING = 0xFE,
    BLD_STATE_NEWFW = 0xFC,
    BLD_STATE_FIRST_RUN = 0xF8
} bld_stt_t;

typedef struct
{
    bld_stt_t State; // state of bootloader
    uint16_t Downloaded; // counter of downloaded part 
    uint16_t Port; // port of host
    char Host[128]; // host name
    char User[32]; // user name to access the host
    char Password[32]; // password to access the host
} bld_info_t;

extern bld_info_t BldInfo;

bld_stt_t BLD_ExtMem_Init(void);
uint8_t BLD_ExtMem_ReadData(void);
void BLD_ExtMem_WriteData(uint8_t b);
bld_stt_t BLD_ExtMem_ReadState(void);
void BLD_ExtMem_WriteState(bld_stt_t stt);

#endif
