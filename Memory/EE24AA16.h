#ifndef EE24AA16_H
#define EE24AA16_H

#include <stdint.h>
#include <stdbool.h>
#include "EE24AA16_Cfg.h"

#ifndef EE24AA16_PIN_A0
#define EE24AA16_PIN_A0 0
#endif

#ifndef EE24AA16_PIN_A1
#define EE24AA16_PIN_A1 0
#endif

#ifndef EE24AA16_PIN_A2
#define EE24AA16_PIN_A2 0
#endif

#define EE24AA16_SLAVE_ADDR (0b01010000|(EE24AA16_PIN_A2<<2)|(EE24AA16_PIN_A2<<1)|EE24AA16_PIN_A0)
#define EE24AA16_SIZE       2048 // bytes

#ifndef 

#ifndef EE24AA16_GetState
#define EE24AA16_GetState() 0
#endif

void EE24AA16_WriteBytes(uint8_t address, uint8_t data);
uint8_t EE24AA16_ReadBytes(uint8_t address);

#endif