#ifndef FLASH_ACCESS_H_
#define FLASH_ACCESS_H_

#include <stdint.h>

#ifndef SELF_PROGRAM_TEST
#include "avr/pgmspace.h"
#include "flash_access.h"
#define FLASH_ReadWord(addr, dummy) (((uint16_t)pgm_read_byte_far(addr+1)<<8)|pgm_read_byte_far(addr))
#else
#ifndef FLASH_ReadWord
#define FLASH_ReadWord(addr, w0)    w0
#endif
#endif

#ifndef BOOTEND
#define BOOTEND                     (0x1FFF) // Bootsize=8192 bytes
#endif

#ifndef APP_BEGIN_ADDR
#define APP_BEGIN_ADDR              (BOOTEND+1)
#endif

#ifndef APP_END_ADDR
#define APP_END_ADDR                FLASHEND
#endif

int8_t SelfProgram(uint32_t Addr, const uint8_t *pData, int Len);
/* Jump in program memory at a specific address */
extern void pgm_jmp_far(uint32_t addrr);

/* Write a word in program memory at a specific address*/
extern void pgm_word_write(uint32_t addrr, uint16_t data);

#endif