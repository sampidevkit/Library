#ifndef XTEA_H
#define XTEA_H

#include "Common/LibDef.h"
#include "Project_Cfg.h"

#define XTEA_DATA_LEN   8

#ifndef XTEA_KEY_LEN
#define XTEA_KEY_LEN    8
#endif

#if((XTEA_KEY_LEN%8)!=0)
#error "XTEA_KEY_LEN should be a multiple of 8"
#endif

#ifndef XTEA_DELTA
#define XTEA_DELTA      0x9E3779B9
#endif

public void XTEA_Encrypt(void *pDest, const void *pRaw, const void *pKey, uint8_t Round);
public void XTEA_Decrypt(void *pDest, const void *pEnc, const void *pKey, uint8_t Round);

#endif