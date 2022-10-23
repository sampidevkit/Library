#include "EE24AA16.h"
#include "libcomp.h"

void EE24AA16_WriteBytes(uint16_t address, uint8_t data)
{
    uint8_t buff[3]={(uint8_t) (address>>8), (uint8_t) address, data};

    EE24AA16_I2C_WriteNBytes(EE24AA16_SLAVE_ADDR, buff, 3);
}

uint8_t EE24AA16_ReadBytes(uint16_t address)
{
    uint8_t buff[3]={(uint8_t) (address>>8), (uint8_t) address, 0};

    EE24AA16_I2C_WriteNBytes(EE24AA16_SLAVE_ADDR, buff, 2);

    if(EE24AA16_GetState())
    return buff[2];
}