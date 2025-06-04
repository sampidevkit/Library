#include "bld_extmem.h"
#include "misc/util.h"

static uint8_t BldInfo[BLD_EXTMEM_INFO_LEN];
static uint32_t DataAddr=BLD_EXTMEM_DATA_ADDR;

bld_stt_t BLD_ExtMem_Init(void)
{
    bld_stt_t bldState=BLD_STATE_UPTODATE;

    if(BLD_ExtMem_Driver_Init())
    {
        bldState=BLD_ExtMem_ReadState();
        DataAddr=BLD_EXTMEM_DATA_ADDR;
    }

    return bldState;
}

int BLD_ExtMem_ReadData(void)
{
    if(DataAddr<=BLD_EXTMEM_END)
        return BLD_ExtMem_Driver_Read(DataAddr++);

    return EOF;
}

void BLD_ExtMem_SetDataAddr(uint32_t addr)
{
    if(addr<=BLD_EXTMEM_END)
        DataAddr=addr;
}

void BLD_ExtMem_WriteData(uint8_t b)
{
    uint32_t i;

    if(DataAddr==BLD_EXTMEM_DATA_ADDR) // check blank
    {
        do
        {
            if(BLD_ExtMem_Driver_Read(DataAddr)!=0xFF)
            {
                i=BLD_EXTMEM_DATA_LEN/BLD_EXTMEM_SECTOR_LEN; // get number of sector
                DataAddr=BLD_EXTMEM_DATA_ADDR;

                while(i>0)
                {
                    BLD_ExtMem_Driver_Erase(DataAddr);
                    DataAddr+=BLD_EXTMEM_SECTOR_LEN;
                    i--;
                }

                break;
            }
            else
                DataAddr++;

        }
        while(DataAddr<=BLD_EXTMEM_END);

        DataAddr=BLD_EXTMEM_DATA_ADDR;
    }

    if(DataAddr<=BLD_EXTMEM_END)
        BLD_ExtMem_Driver_Write(DataAddr++, b);
}

bld_stt_t BLD_ExtMem_ReadState(void)
{
    uint32_t i;

    for(i=0; i<BLD_EXTMEM_INFO_LEN; i++)
        BldInfo[i]=BLD_ExtMem_Driver_Read(BLD_EXTMEM_INFO_ADDR+i);

    if(BldInfo[INFO_STATE_OFFSET]!=BLD_STATE_FIRST_RUN)
    {
        if(BldInfo[INFO_STATE_OFFSET]!=BLD_STATE_NEWFW)
        {
            if(BldInfo[INFO_STATE_OFFSET]!=BLD_STATE_DOWNLOADING)
                BldInfo[INFO_STATE_OFFSET]=BLD_STATE_UPTODATE;
        }
    }

    return (bld_stt_t) BldInfo[INFO_STATE_OFFSET];
}

void BLD_ExtMem_WriteState(bld_stt_t stt)
{
    if(stt!=BldInfo[INFO_STATE_OFFSET])
    {
        uint32_t i;

        if(stt>BldInfo[INFO_STATE_OFFSET])
            BLD_ExtMem_Driver_Erase(BLD_EXTMEM_INFO_ADDR);

        BldInfo[INFO_STATE_OFFSET]=stt;

        for(i=0; i<BLD_EXTMEM_INFO_LEN; i++)
            BLD_ExtMem_Driver_Write(BLD_EXTMEM_INFO_ADDR+i, BldInfo[i]);
    }
}

uint16_t BLD_ExtMem_ReadServerPort(void)
{
    uint16_t x=BldInfo[INFO_PORT_OFFSET];

    x<<=8;
    x|=BldInfo[INFO_PORT_OFFSET+1];

    return x;
}

void BLD_ExtMem_WriteServerPort(uint16_t cnt)
{
    uint32_t i;

    if(BldInfo[INFO_PORT_OFFSET]==(uint8_t) (cnt>>8))
    {
        if(BldInfo[INFO_PORT_OFFSET+1]==(uint8_t) (cnt))
            return;
    }

    BLD_ExtMem_Driver_Erase(BLD_EXTMEM_INFO_ADDR);
    BldInfo[INFO_PORT_OFFSET]=(uint8_t) (cnt>>8);
    BldInfo[INFO_PORT_OFFSET+1]=(uint8_t) (cnt);

    for(i=0; i<BLD_EXTMEM_INFO_LEN; i++)
        BLD_ExtMem_Driver_Write(BLD_EXTMEM_INFO_ADDR+i, BldInfo[i]);
}

uint8_t *BLD_ExtMem_ReadServerName(void)
{
    return &BldInfo[INFO_HOST_OFFSET];
}

void BLD_ExtMem_WriteServerName(const uint8_t *pServerName)
{
    uint32_t i;

    if(str_cmp((char *) &BldInfo[INFO_HOST_OFFSET], (char *)pServerName))
        return;

    BLD_ExtMem_Driver_Erase(BLD_EXTMEM_INFO_ADDR);

    for(i=0; i<INFO_HOST_LEN; i++)
        BldInfo[INFO_HOST_OFFSET+i]=*pServerName++;

    for(i=0; i<BLD_EXTMEM_INFO_LEN; i++)
        BLD_ExtMem_Driver_Write(BLD_EXTMEM_INFO_ADDR+i, BldInfo[i]);
}

uint8_t *BLD_ExtMem_ReadUserName(void)
{
    return &BldInfo[INFO_USER_OFFSET];
}

void BLD_ExtMem_WriteUserName(const uint8_t *pUserName)
{
    uint32_t i;

    if(str_cmp((char *) &BldInfo[INFO_USER_OFFSET], (char *)pUserName))
        return;

    BLD_ExtMem_Driver_Erase(BLD_EXTMEM_INFO_ADDR);

    for(i=0; i<INFO_USER_LEN; i++)
        BldInfo[INFO_USER_OFFSET+i]=*pUserName++;

    for(i=0; i<BLD_EXTMEM_INFO_LEN; i++)
        BLD_ExtMem_Driver_Write(BLD_EXTMEM_INFO_ADDR+i, BldInfo[i]);
}

uint8_t *BLD_ExtMem_ReadPassword(void)
{
    return &BldInfo[INFO_PASSWORD_OFFSET];
}

void BLD_ExtMem_WritePassword(const uint8_t *pPassword)
{
    uint32_t i;

    if(str_cmp((char *) &BldInfo[INFO_PASSWORD_OFFSET], (char *)pPassword))
        return;

    BLD_ExtMem_Driver_Erase(BLD_EXTMEM_INFO_ADDR);

    for(i=0; i<INFO_PASSWORD_LEN; i++)
        BldInfo[INFO_PASSWORD_OFFSET+i]=*pPassword++;

    for(i=0; i<BLD_EXTMEM_INFO_LEN; i++)
        BLD_ExtMem_Driver_Write(BLD_EXTMEM_INFO_ADDR+i, BldInfo[i]);
}

uint16_t BLD_ExtMem_ReadDownloadedPackage(void)
{
    uint16_t i, x, last=0;

    for(i=0; i<INFO_DOWNLOADED_LEN; i+=2)
    {
        x=BldInfo[i];
        x<<=8;
        x|=BldInfo[i+1];
        x^=0xFFFF;

        if(x==0)
            break;
        else
            last=x;
    }

    return last;
}

void BLD_ExtMem_WriteDownloadedPackage(uint16_t cnt)
{
    uint32_t i;

    if(cnt==BLD_ExtMem_ReadDownloadedPackage())
        return;

    for(i=0; i<INFO_DOWNLOADED_LEN; i+=2) // find the last stored address
    {
        uint16_t x=BldInfo[i];

        x<<=8;
        x|=BldInfo[i+1];
        x^=0xFFFF;

        if(x==0)
            break;
    }

    if(i>=INFO_RFU_OFFSET)
    {
        BLD_ExtMem_Driver_Erase(BLD_EXTMEM_INFO_ADDR);
        BldInfo[INFO_DOWNLOADED_OFFSET]=(uint8_t) (cnt>>8);
        BldInfo[INFO_DOWNLOADED_OFFSET+1]=(uint8_t) (cnt);

        for(i=0; i<BLD_EXTMEM_INFO_LEN; i++)
            BLD_ExtMem_Driver_Write(BLD_EXTMEM_INFO_ADDR+i, BldInfo[i]);
    }
    else
    {
        i+=(BLD_EXTMEM_INFO_ADDR+INFO_DOWNLOADED_OFFSET);
        BLD_ExtMem_Driver_Write(i, (uint8_t) (cnt>>8));
        BLD_ExtMem_Driver_Write(i+1, (uint8_t) (cnt));
    }
}
