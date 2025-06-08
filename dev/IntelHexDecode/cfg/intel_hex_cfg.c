#include "misc/intel_hex.h"

public uint8_t IHEX_NVM_Write(uint32_t addr, uint8_t *pData, uint8_t len)
{
    uint8_t i;

    printf("\r\n-->IHEX_NVM_Write %08X: ", addr);

    for(i=0; i<len; i++)
        printf("%02X ", *pData++);

    return IHEX_DONE;

}

public void IHEX_ErrorLogWrite(uint16_t line)
{
    printf("\r\n-->IHEX error line: %d", line);
}
