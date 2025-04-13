#include "../memmap.h"
#include "memory/flash.h"

#if(__PIC32_FLASH_SIZE==256)

#if defined(__32MM0256GPM028_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM028";
#elif defined(__32MM0256GPM036_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM036";
#elif defined(__32MM0256GPM048_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM048";
#elif defined(__32MM0256GPM064_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM064";
#else
#error "Target MCU is not supported"
#endif

// Bootloader size is fixed in 20480 bytes (20kB)
// Application size is 262144-20480=241664 bytes (236kB)
const uint32_t FLASH_PAGE_SIZE=FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS; // (bytes)
const uint32_t APP_RESET_ADDRESS=0x9D000000;
const uint32_t APP_BEGIN_ADDRESS=0x1D000000;
const uint32_t APP_END_ADDRESS=0x1D03AFFF;
const uint32_t APP_SIZE_IN_PAGE=118; // (pages)
const uint32_t TARGET_PACK_BYTE_MASK=0xFFFFF800; // 2048 bytes/page
const uint32_t TARGET_PACK_IDX_MASK=0x7FF; // 0 to 2047   

#elif(__PIC32_FLASH_SIZE==128)

#if defined(__32MM0128GPM028_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM028";
#elif defined(__32MM0128GPM036_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM036";
#elif defined(__32MM0128GPM048_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM048";
#elif defined(__32MM0128GPM064_H)
const char BLD_MCU_NAME[]="PIC32MM0256GPM064";
#else
#error "Target MCU is not supported"
#endif

// Bootloader size is fixed in 20480 bytes (20kB)
// Application size is 131072-20480=110592 bytes (108kB)
const uint32_t FLASH_PAGE_SIZE=FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS; // (bytes)
const uint32_t APP_RESET_ADDRESS=0x9D000000;
const uint32_t APP_BEGIN_ADDRESS=0x1D000000;
const uint32_t APP_END_ADDRESS=0x1D01AFFF;
const uint32_t APP_SIZE_IN_PAGE=54; // (pages)
const uint32_t TARGET_PACK_BYTE_MASK=0xFFFFF800; // 2048 bytes/page
const uint32_t TARGET_PACK_IDX_MASK=0x7FF; // 0 to 2047   

#elif(__PIC32_FLASH_SIZE==64)

#if defined(__32MM0064GPM028_H)
const char BLD_MCU_NAME[]="PIC32MM0064GPM028";
#elif defined(__32MM0064GPM036_H)
const char BLD_MCU_NAME[]="PIC32MM0064GPM036";
#elif defined(__32MM0064GPM048_H)
const char BLD_MCU_NAME[]="PIC32MM0064GPM048";
#elif defined(__32MM0064GPM064_H)
const char BLD_MCU_NAME[]="PIC32MM0064GPM064";
#else
#error "Target MCU is not supported"
#endif

// Bootloader size is fixed in 20480 bytes (20kB)
// Application size is 65536-20480=45056 bytes (44kB)
const uint32_t FLASH_PAGE_SIZE=FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS; // (bytes)
const uint32_t APP_RESET_ADDRESS=0x9D000000;
const uint32_t APP_BEGIN_ADDRESS=0x1D000000;
const uint32_t APP_END_ADDRESS=0x1D00AFFF;
const uint32_t APP_SIZE_IN_PAGE=22; // (pages)
const uint32_t TARGET_PACK_BYTE_MASK=0xFFFFF800; // 2048 bytes/page
const uint32_t TARGET_PACK_IDX_MASK=0x7FF; // 0 to 2047   
#else
#error "Target MCU is not supported"
#endif
