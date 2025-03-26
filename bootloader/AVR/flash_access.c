#include <avr/wdt.h>
#include "self_program.h"

#ifdef SELF_PROGRAM_DEBUG
#include "Common/debug.h"
#undef  __dbsh
#define __dbsh(str, x)  do{debug((const char *)(str), 0); debug_hex(x, 0x84);}while(0)
#else
#undef __dbs
#undef __dbi
#undef __dbh
#undef __dbh2
#undef __dbsi
#undef __dbsh

#define __dbs(str)
#define __dbi(x)
#define __dbh(x)
#define __dbh2(x)
#define __dbsi(str, x)
#define __dbsh(str, x)
#endif

#ifndef PROGMEM_PAGE_SIZE // uint: byte
#define PROGMEM_PAGE_SIZE               (512U)
#warning "Default PROGMEM_PAGE_SIZE=512 bytes"
#endif

#define FLASH_PACK_DATA_SIZE            (PROGMEM_PAGE_SIZE>>1U) // unit: Word
#define FLASH_DATA_INDEX_IN_PAGE        (FLASH_PACK_DATA_SIZE-1) // word index
#define FLASH_ERASE_PAGE_MASK           (~FLASH_DATA_INDEX_IN_PAGE)

#ifndef FLASH_GetErasePageAddress
#define FLASH_GetErasePageAddress(addr) (addr&FLASH_ERASE_PAGE_MASK) // uint: Word
#endif

#ifndef BLD_SELFPROG_LED_SetHigh
#define BLD_SELFPROG_LED_SetHigh()
#endif

#ifndef BLD_SELFPROG_LED_SetLow
#define BLD_SELFPROG_LED_SetLow()
#endif

#ifdef SELF_PROGRAM_TEST
#warning "This section is used in debug mode only"

#define FLASH_EraseFlashPage(addr)

static void pgm_word_write(uint32_t addr, uint16_t w)
{
    __dbsh("\nWR: ", addr);
    __dbsh(": ", w);
}
#else

static void FLASH_EraseFlashPage(uint32_t addr)
{
    __dbsh("\nErase ", addr);
    wdt_reset();
    /* Wait for completion of previous write */
    while(NVMCTRL.STATUS&(NVMCTRL_EEBUSY_bm|NVMCTRL_FBUSY_bm));
    /* Clear the current command */
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
    /* Erase the flash page */
    /* Send erase command */
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_FLPER_gc);
    /* Dummy write to start erase operation */
    pgm_word_write(addr, 0x00);
    /* Wait for completion of previous write */
    while(NVMCTRL.STATUS&(NVMCTRL_EEBUSY_bm|NVMCTRL_FBUSY_bm));
    /* Clear the current command */
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
    /* Enable flash Write Mode */
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_FLWR_gc);
}
#endif

static struct
{
    uint16_t idx; // index in page
    uint32_t addr; // page address
    uint16_t data[FLASH_PACK_DATA_SIZE]; // page data
} BldNvmPack;

static bool bld_nvm_initialized=0;

static void BLD_Nvm_Deinit(void) // <editor-fold defaultstate="collapsed" desc="Deinit memory">
{
    //__dbss("\n--> ", __FUNCTION__);
    bld_nvm_initialized=0;
    BLD_Deinit();
} // </editor-fold>

static int8_t BLD_Nvm_RowCommit(void) // <editor-fold defaultstate="collapsed" desc="Write 1 row">
{
    uint16_t i,dW;
    uint32_t Addr;
    int8_t rslt=RESULT_DONE;

    //__dbss("\n--> ", __FUNCTION__);

    BLD_SELFPROG_LED_SetHigh();
    BldNvmPack.addr=FLASH_GetErasePageAddress(BldNvmPack.addr);
    //__dbsh(" addr=", BldNvmPack.addr);

    Addr=BldNvmPack.addr<<1;
    FLASH_EraseFlashPage(Addr);// Use byte address

    //__dbs("\nCheck data");

    for(i=0; i<FLASH_PACK_DATA_SIZE; i++) // check blank data
    {
        if(BldNvmPack.data[i]!=0xFFFF)
            break;
    }

    if(i<FLASH_PACK_DATA_SIZE)
    {
        __dbsh("\nWrPg WD=", BldNvmPack.addr);

        for(i=0; i<FLASH_PACK_DATA_SIZE; i++)
        {
            /* Program flash word with desired value */
            pgm_word_write(Addr, BldNvmPack.data[i]);
            // Verify data
            dW=FLASH_ReadWord(Addr, BldNvmPack.data[i]);

            if(dW!=BldNvmPack.data[i])
            {
                rslt=RESULT_ERR;
                __dbsh("\nRErr ", BldNvmPack.addr);
                __dbsh(": ", BldNvmPack.data[i]);
                __dbsh("# ", dW);
            }

            BldNvmPack.addr++;
            Addr=BldNvmPack.addr<<1;
        }

        BldNvmPack.addr--; // keep address in present page
    }
    else
    {
        __dbs(": blanked");
    }

    BLD_SELFPROG_LED_SetLow();

    return rslt;
} // </editor-fold>

static void BLD_Nvm_CreateNewPack(void) // <editor-fold defaultstate="collapsed" desc="Preset page value">
{
    //__dbss("\n--> ", __FUNCTION__);
    //__dbs("\nClear buffer");

    for(BldNvmPack.idx=0; BldNvmPack.idx<FLASH_PACK_DATA_SIZE; BldNvmPack.idx++)
        BldNvmPack.data[BldNvmPack.idx]=0xFFFF;

    BldNvmPack.idx=0;
} // </editor-fold>

static int8_t BLD_Nvm_Pack(uint32_t Addr, const uint8_t *pData) // <editor-fold defaultstate="collapsed" desc="Add 2 bytes to buffer">
{
    int8_t rslt=RESULT_DONE;

    if(bld_nvm_initialized==0) // initialize
    {
        __dbsh("\nNVM init: ", FLASH_GetErasePageAddress(Addr));
        BLD_Nvm_CreateNewPack();
        BldNvmPack.addr=(APP_BEGIN_ADDR>>1);
        bld_nvm_initialized=1;
    }

    //__dbsh("\nAddr=", Addr);

    if(Addr>=BldNvmPack.addr)
    {
        if(pData!=NULL)
        {
            uint32_t prePgAddr=FLASH_GetErasePageAddress(Addr);
            uint32_t prvPgAddr=FLASH_GetErasePageAddress(BldNvmPack.addr);

            //__dbsh("\nPrv Pg=", prvPgAddr);
            //__dbsh(", Pre Pg=", prePgAddr);

            if(prePgAddr>prvPgAddr) // New page
            {
                __dbsh("\nNew commit @", BldNvmPack.addr);
                rslt=BLD_Nvm_RowCommit();
                BLD_Nvm_CreateNewPack();
            }

            BldNvmPack.addr=Addr;
            //__dbsh("\nAddr=", BldNvmPack.addr);
            //__dbsh(", PgAddr=", prePgAddr);

            BldNvmPack.idx=(uint16_t) (Addr&FLASH_DATA_INDEX_IN_PAGE);
            //__dbsi(", Data[", BldNvmPack.idx);
            BldNvmPack.data[BldNvmPack.idx]=pData[1];
            BldNvmPack.data[BldNvmPack.idx]<<=8;
            BldNvmPack.data[BldNvmPack.idx]|=pData[0];
            //__dbsh("]=", BldNvmPack.data[BldNvmPack.idx]);
        }
        else // write the last page
        {
            __dbsh("\nLast commit @", BldNvmPack.addr);
            rslt=BLD_Nvm_RowCommit();
            BldNvmPack.addr=FLASH_GetErasePageAddress(BldNvmPack.addr);
            Addr=BldNvmPack.addr<<1;
            BLD_SELFPROG_LED_SetHigh();

            do
            {
                Addr+=PROGMEM_PAGE_SIZE;
                FLASH_EraseFlashPage(Addr);
                __dbsh("\nErase: ", Addr);
            }
            while(Addr<(APP_END_ADDR-PROGMEM_PAGE_SIZE));

            BLD_SELFPROG_LED_SetLow();
        }
    }
    else
    {
        rslt=RESULT_ERR;
        __dbsh("\nAddr err: ", Addr);
        __dbsh(" < ", BldNvmPack.addr);
    }

    return rslt;
} // </editor-fold>

int8_t SelfProgram(uint32_t Addr, const uint8_t *pData, int Len) // <editor-fold defaultstate="collapsed" desc="Write hex data to flash">
{
    int i;
    bool odd=false;
    int8_t rslt=RESULT_DONE;

    //__dbss("\n--> ", __FUNCTION__);

    if((Len&1)==1) // data length error, it should be multiple of 2
    {
        odd=true;
        Len--;
        //        rslt=RESULT_ERR;
        //        __dbsi("\nLine=", __LINE__);
        //        goto EXIT;
    }
    else if(Len==0) // finish
    {
        rslt=BLD_Nvm_Pack(APP_END_ADDR, NULL);
        goto EXIT;
    }

    for(i=0; i<Len; i+=2)
    {
        if((Addr>=APP_BEGIN_ADDR)&&(Addr<APP_END_ADDR)) // Check valid address
        {
            rslt=BLD_Nvm_Pack(Addr>>1, &pData[i]);

            if(rslt==RESULT_ERR)
                goto EXIT;
        }

        Addr+=2;
    }

    if(odd==true)
    {
        uint8_t lastData[2];

        lastData[0]=pData[Len];
        lastData[1]=0xFF;
        __dbs("\nLast data: ");
        __dbh(lastData[0], 2);
        __dbc(' ');
        __dbh(lastData[1], 2);

        if((Addr>=APP_BEGIN_ADDR)&&(Addr<APP_END_ADDR)) // Check valid address
        {
            rslt=BLD_Nvm_Pack(Addr>>1, &lastData[0]);

            if(rslt==RESULT_ERR)
                goto EXIT;
        }
    }

    return RESULT_DONE;

EXIT:
    BLD_Nvm_Deinit();
    //__dbsi("\nRslt=", rslt);

    return rslt;
} // </editor-fold>
