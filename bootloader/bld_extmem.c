#include "bld_extmem.h"

bld_info_t BldInfo;
static uint32_t DataAddr=BLD_EXTMEM_DATA_ADDR;

bld_stt_t BLD_ExtMem_Init(void)
{
    if(BLD_ExtMem_Driver_Init())
    {


        DataAddr=BLD_EXTMEM_DATA_ADDR;
    }
}

uint8_t BLD_ExtMem_ReadData(void)
{
    if(DataAddr<=BLD_EXTMEM_END)
        return BLD_ExtMem_Driver_Read(DataAddr++);

    return 0xFF;
}

void BLD_ExtMem_WriteData(uint8_t b)
{
    if(DataAddr<=BLD_EXTMEM_END)
        BLD_ExtMem_Driver_Write(DataAddr++, b);
}

bld_stt_t BLD_ExtMem_ReadState(void)
{
        uint8_t *pD=(uint8_t *)&BldInfo;
        
        for(DataAddr=BLD_EXTMEM_INFO_ADDR; DataAddr<(BLD_EXTMEM_INFO_ADDR+BLD_EXTMEM_INFO_LEN); DataAddr++)
        {
            *pD=BLD_ExtMem_Driver_Read(DataAddr);
            pD++;
        }
}

void BLD_ExtMem_WriteState(bld_stt_t stt)
{
    
    if(BLD_STATE_DOWNLOADING==stt)
    {
        for(DataAddr=BLD_EXTMEM_BEGIN; DataAddr<(BLD_EXTMEM_BEGIN+1); DataAddr+=BLD_EXTMEM_SECTOR_LEN)
            BLD_ExtMem_Driver_Erase(DataAddr);
    }

    return BLD_ExtMem_Driver_Write(BLD_EXTMEM_INFO_ADDR, stt);
}