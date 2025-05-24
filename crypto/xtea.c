#include "xtea.h"

#ifdef USE_XTEA_DEBUG
#include "Common/debug.h"
#else
#define __db(...)
#define __dbhex(...)
#endif

private uint8_t i;

#if(XTEA_KEY_LEN>8)
private int8_t j;
private uint8_t *pk, tmp[XTEA_DATA_LEN];
#endif

private uint32_t v0, v1, sum;

private void _XTEA_Enc(void *pDest, const void *pRaw, const void *pKey, uint8_t Round) // <editor-fold defaultstate="collapsed" desc="Standard encryption">
{
    v0=((const uint32_t*) pRaw)[0];
    v1=((const uint32_t*) pRaw)[1];
    sum=0;

    for(i=0; i<Round; i++)
    {
        v0+=((v1<<4 ^ v1>>5)+v1) ^ (sum+((const uint32_t*) pKey)[sum&3]);
        sum+=XTEA_DELTA;
        v1+=((v0<<4 ^ v0>>5)+v0) ^ (sum+((const uint32_t*) pKey)[(sum>>11)&3]);
    }

    ((uint32_t*) pDest)[0]=v0;
    ((uint32_t*) pDest)[1]=v1;
} // </editor-fold>

private void _XTEA_Dec(void *pDest, const void *pEnc, const void *pKey, uint8_t Round) // <editor-fold defaultstate="collapsed" desc="Standard decryption">
{
    v0=((const uint32_t*) pEnc)[0];
    v1=((const uint32_t*) pEnc)[1];
    sum=Round; //0xC6EF3720;
    sum*=XTEA_DELTA;

    for(i=0; i<Round; i++)
    {
        v1-=((v0<<4 ^ v0>>5)+v0) ^ (sum+((const uint32_t*) pKey)[(sum>>11)&3]);
        sum-=XTEA_DELTA;
        v0-=((v1<<4 ^ v1>>5)+v1) ^ (sum+((const uint32_t*) pKey)[sum&3]);
    }

    ((uint32_t*) pDest)[0]=v0;
    ((uint32_t*) pDest)[1]=v1;
} // </editor-fold>

public void XTEA_Encrypt(void *pDest, const void *pRaw, const void *pKey, uint8_t Round) // <editor-fold defaultstate="collapsed" desc="XTEA encrypt">
{
#if(XTEA_KEY_LEN>8)
    j=0;
    pk=(uint8_t*) pKey;
    memcpy(tmp, pRaw, XTEA_DATA_LEN);

    while(j<XTEA_KEY_LEN)
    {
        __db("\n\nE %d: ", j);
        __dbhex(&pk[j], 8);
        __db("\nDat: ");
        __dbhex(tmp, 8);

        _XTEA_Enc(pDest, tmp, (void*) &pk[j], Round);
        memcpy(tmp, pDest, XTEA_DATA_LEN);
        j+=8;

        __db("\nEnc: ");
        __dbhex((uint8_t *) pDest, 8);
    }
#else
    __db("\n\nE:");
    __dbhex((uint8_t *) pKey, 8);
    __db("\nDat: ");
    __dbhex((uint8_t *) pRaw, 8);

    _XTEA_Enc(pDest, pRaw, pKey, Round);

    __db("\nEnc: ");
    __dbhex((uint8_t *) pDest, 8);
#endif
} // </editor-fold>

public void XTEA_Decrypt(void *pDest, const void *pEnc, const void *pKey, uint8_t Round) // <editor-fold defaultstate="collapsed" desc="XTEA decrypt">
{
#if(XTEA_KEY_LEN>8)
    j=XTEA_KEY_LEN-8;
    pk=(uint8_t*) pKey;
    memcpy(tmp, pEnc, XTEA_DATA_LEN);

    while(j>=0)
    {
        __db("\n\nD %d: ", j);
        __dbhex(&pk[j], 8);
        __db("\nDat: ");
        __dbhex(tmp, 8);

        _XTEA_Dec(pDest, tmp, (void*) &pk[j], Round);
        memcpy(tmp, pDest, XTEA_DATA_LEN);
        j-=8;

        __db("\nRaw: ");
        __dbhex((uint8_t *) pDest, 8);
    }
#else
    __db("\n\nD:");
    __dbhex((uint8_t *) pKey, 8);
    __db("\nDat: ");
    __dbhex((uint8_t *) pEnc, 8);

    _XTEA_Dec(pDest, pEnc, pKey, Round);

    __db("\nRaw: ");
    __dbhex((uint8_t *) pDest, 8);
#endif
} // </editor-fold>