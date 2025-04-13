#ifndef MEMMAP_H
#define MEMMAP_H

#include <xc.h>
#include <stdint.h>

extern const char BLD_MCU_NAME[];

#ifdef __XC32__
extern const uint32_t FLASH_PAGE_SIZE;
extern const uint32_t APP_RESET_ADDRESS;
extern const uint32_t APP_BEGIN_ADDRESS;
extern const uint32_t APP_END_ADDRESS;
extern const uint32_t APP_SIZE_IN_PAGE;
extern const uint32_t TARGET_PACK_BYTE_MASK;
extern const uint32_t TARGET_PACK_IDX_MASK;
#else
extern const uint16_t FLASH_PAGE_SIZE;
extern const uint16_t APP_RESET_ADDRESS;
extern const uint16_t APP_BEGIN_ADDRESS;
extern const uint16_t APP_END_ADDRESS;
extern const uint16_t APP_SIZE_IN_PAGE;
extern const uint16_t TARGET_PACK_BYTE_MASK;
extern const uint16_t TARGET_PACK_IDX_MASK;
#endif
#endif