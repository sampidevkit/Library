#include "../flash_access.h"
#include "../memmap.h"
#include "memory/flash.h"

typedef struct
{
    uint32_t Address;
    uint32_t Word0;
    uint32_t Word1;
} flash_t;

private bool flash_initialized=0;

private uint32_t Flash_Access_Convert4x8to32(const uint8_t *pArr) // <editor-fold defaultstate="collapsed" desc="Convert 32bit to 4x8bit">
{
    uint32_t Value=pArr[3];

    Value<<=8;
    Value|=pArr[2];
    Value<<=8;
    Value|=pArr[1];
    Value<<=8;
    Value|=pArr[0];

    return Value;
} // </editor-fold>

private bool Flash_Access_IsWritable(uint32_t Addr) // <editor-fold defaultstate="collapsed" desc="Check writable address">
{
    if((Addr>=APP_BEGIN_ADDRESS)&&(Addr<=APP_END_ADDRESS))
        return 1;

    return 0;
} // </editor-fold>

private bool Flash_Access_FullErase(void) // <editor-fold defaultstate="collapsed" desc="Full media memory erasing">
{
    uint32_t Addr=APP_BEGIN_ADDRESS;
    uint32_t NumOfPage=APP_SIZE_IN_PAGE;

    while(NumOfPage>0)
    {
        NumOfPage--;

        if(!FLASH_ErasePage(Addr))
            return 0; // error

        Addr+=FLASH_PAGE_SIZE;
    }

    return 1;
} // </editor-fold>

private bool Flash_Access_Init(void) // <editor-fold defaultstate="collapsed" desc="Init memory">
{
    if(flash_initialized==0)
    {
        FLASH_Unlock(FLASH_UNLOCK_KEY);
        flash_initialized=Flash_Access_FullErase();
    }

    return flash_initialized;
} // </editor-fold>

private void Flash_Access_Denit(void) // <editor-fold defaultstate="collapsed" desc="Deinit memory">
{
    FLASH_Lock();
    flash_initialized=0;
} // </editor-fold>

private bool Flash_Access_DWordPack(flash_t *pBldNvm) // <editor-fold defaultstate="collapsed" desc="Write double word then checksum">
{
    bool rslt=0;

    if(Flash_Access_Init())
    {
        if((pBldNvm->Word0!=0xFFFFFFFF)||(pBldNvm->Word1!=0xFFFFFFFF))
        {
            if(Flash_Access_IsWritable(pBldNvm->Address))
            {
                if(FLASH_WriteDoubleWord(pBldNvm->Address, pBldNvm->Word0, pBldNvm->Word1))
                    rslt=1;
            }
            else
                rslt=1;

            pBldNvm->Word0=0xFFFFFFFF; // add blanked data
            pBldNvm->Word1=0xFFFFFFFF; // add blanked data
        }
        else
            rslt=1;
    }

    return rslt;
} // </editor-fold>

public int8_t Flash_Access_Write(uint32_t Addr, const uint8_t *pData, int Len) // <editor-fold defaultstate="collapsed" desc="Write hex data to flash">
{
    // copy only the bytes from the current data packet up to the boundary of a row
    int8_t rslt=PROC_DONE;
    private flash_t flash={0, 0xFFFFFFFF, 0xFFFFFFFF};

    // Check the last data
    if(Len==0)
    {
        if(!Flash_Access_DWordPack(&flash))
            rslt=PROC_ERR;

        goto EXIT;
    }

    while(Len>0)
    {
        // Check boundary of DWord
        if((Addr&0xFFFFFFF8)!=flash.Address)
        {
            if(flash.Address!=0) // Skip in the first time
            {
                if(!Flash_Access_DWordPack(&flash))
                {
                    rslt=PROC_ERR;
                    goto EXIT;
                }
            }

            flash.Address=(Addr&0xFFFFFFF8); // update new row address
        }
        // Check current address is of Word0 or Word1
        if((Addr&7)==0) // Word 0 address
            flash.Word0=Flash_Access_Convert4x8to32(pData);
        else // if((Addr&4)==4) // Word 1 address
            flash.Word1=Flash_Access_Convert4x8to32(pData);

        pData+=4;
        Addr+=4;
        Len-=4;
    }

    return PROC_DONE;

EXIT:
    flash.Address=0; // Reset the 1st start address
    Flash_Access_Denit();

    return rslt;
} // </editor-fold>

public void Flash_Access_Read(uint32_t Addr, void *pData, int Len) // <editor-fold defaultstate="collapsed" desc="Read flash memory">
{
    int i;
    uint32_t *pD=(uint32_t *) pData;

    Addr&=4;
    
    for(i=0; i<Len; i++)
    {
        *pD=FLASH_ReadWord(Addr+i);
        pD++;
    }
} // </editor-fold>
