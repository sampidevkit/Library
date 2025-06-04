#ifndef BLD_EXTMEM_H
#define	BLD_EXTMEM_H

#include "common/libdef.h"
#include "project_cfg.h"

#define BLD_EXTMEM_CAPACITY     (BLD_EXTMEM_END-BLD_EXTMEM_BEGIN+1)
#define BLD_EXTMEM_INFO_ADDR    BLD_EXTMEM_BEGIN
#define BLD_EXTMEM_INFO_LEN     BLD_EXTMEM_SECTOR_LEN
#define BLD_EXTMEM_RESUME_ADDR  BLD_EXTMEM_BEGIN
#define BLD_EXTMEM_RESUME_LEN   BLD_EXTMEM_SECTOR_LEN
#define BLD_EXTMEM_DATA_ADDR    (BLD_EXTMEM_INFO_ADDR+BLD_EXTMEM_INFO_LEN)
#define BLD_EXTMEM_DATA_LEN     (BLD_EXTMEM_CAPACITY-BLD_EXTMEM_DATA_ADDR)
// Sub-address of BLD_EXTMEM_INFO zone
#define INFO_STATE_OFFSET       0
#define INFO_STATE_LEN          1
#define INFO_PORT_OFFSET        (INFO_STATE_OFFSET+INFO_STATE_LEN)
#define INFO_PORT_LEN           2
#define INFO_HOST_OFFSET        (INFO_PORT_OFFSET+INFO_PORT_LEN)
#define INFO_HOST_LEN           128
#define INFO_USER_OFFSET        (INFO_HOST_OFFSET+INFO_HOST_LEN)
#define INFO_USER_LEN           32
#define INFO_PASSWORD_OFFSET    (INFO_USER_OFFSET+INFO_USER_LEN)
#define INFO_PASSWORD_LEN       32
#define INFO_DOWNLOADED_OFFSET  (INFO_PASSWORD_OFFSET+INFO_PASSWORD_LEN)
#define INFO_DOWNLOADED_LEN     3900
#define INFO_RFU_OFFSET         (INFO_DOWNLOADED_OFFSET+INFO_DOWNLOADED_LEN)
#define INFO_RFU_LEN            1
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

bld_stt_t BLD_ExtMem_Init(void);
int BLD_ExtMem_ReadData(void);
void BLD_ExtMem_SetDataAddr(uint32_t addr);
void BLD_ExtMem_WriteData(uint8_t b);
bld_stt_t BLD_ExtMem_ReadState(void);
void BLD_ExtMem_WriteState(bld_stt_t stt);
uint16_t BLD_ExtMem_ReadServerPort(void);
void BLD_ExtMem_WriteServerPort(uint16_t cnt);
uint8_t *BLD_ExtMem_ReadServerName(void);
void BLD_ExtMem_WriteServerName(const uint8_t *pServerName);
uint8_t *BLD_ExtMem_ReadUserName(void);
void BLD_ExtMem_WriteUserName(const uint8_t *pUserName);
uint8_t *BLD_ExtMem_ReadPassword(void);
void BLD_ExtMem_WritePassword(const uint8_t *pPassword);
uint16_t BLD_ExtMem_ReadDownloadedPackage(void);
void BLD_ExtMem_WriteDownloadedPackage(uint16_t cnt);

#endif
