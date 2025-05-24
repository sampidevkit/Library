#ifndef INTEL_HEX_H
#define	INTEL_HEX_H

#include "common/libdef.h"
#include "project_cfg.h"

#ifndef HEX_DATA_SIZE
#define HEX_DATA_SIZE   16
#warning "Default value of HEX_DATA_SIZE is 16"
#endif

// Data type of Intel hex
#define IHEX_RECTYPE_DAT 0x00 // Data
#define IHEX_RECTYPE_EOF 0x01 // End Of File
#define IHEX_RECTYPE_ESA 0x02 // Extended Segment Address
#define IHEX_RECTYPE_SSA 0x03 // Start Segment Address
#define IHEX_RECTYPE_ELA 0x04 // Extended Linear Address
#define IHEX_RECTYPE_SLA 0x05 // Start Linear Address

#define IHEX_DONE        0b00000000
#define IHEX_BUSY        0b10000000
#define IHEX_ERROR       0b01000000

typedef __PACKED_STRUCT
{
    uint32_t Value;
    uint32_t ExtSeg;
    uint32_t ExtLin;
}
hex_addr_t;

typedef __PACKED_STRUCT{
    uint8_t ByteCount;
    hex_addr_t Address;
    uint8_t RecordType;
    uint8_t Data[HEX_DATA_SIZE];
    uint8_t Checksum;
}
hex_t;

typedef unsigned long _paddr_t;

/* ****************************************************** EXTRANAL PROTOTYPES */
public void IHEX_ErrorLogWrite(uint16_t line);
    
#define IHEX_NVM_UNLOCK                 0
#define IHEX_NVM_LOCK                   1
#define IHEX_NVM_INTERNAL_STATE_RESET   2 // Keep previous access mode, reset internal process state only
public void IHEX_NVM_Lock(uint8_t Opt);
public uint8_t IHEX_NVM_Write(uint32_t addr, uint8_t *pData, uint8_t len);
/* ************************************************************************** */
public void IHEX_Init(bool testEna);
public int8_t IHEX_Decode(int8_t c);

#endif
