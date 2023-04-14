#ifndef ATCMD_H
#define ATCMD_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "AtCmd_Cfg.h"

typedef struct {
    uint8_t *pData;
    int Len;
    int Size;
} buff_t;

#ifndef ATCMD_BUFFER_SIZE
#define ATCMD_BUFFER_SIZE   128
#ifndef DISABLE_LIBRARY_WARNING
#warning "Default AtCmdData size is 128"
#endif
#endif

extern uint8_t AtCmdData[ATCMD_BUFFER_SIZE];
extern buff_t AtCmdRxBuff;
// Response constants
extern const char RES_OK[];

void ATCMD_Init(void);
void ATCMD_Delay(uint16_t delayMs);
void ATCMD_SendRaw(const uint8_t *pD, int sz);
int8_t ATCMD_GetRaw(uint8_t *pD, int *pSz, uint16_t firstWait, uint16_t lastWait);
int8_t ATCMD_SendGetDat(const char *pTx, char *pRx, uint16_t firstWait, uint16_t lastWait);
int8_t ATCMD_SendGetAck(const char *pTx, const char *pAck, uint16_t firstWait, uint16_t lastWait, uint8_t tryCount);
int8_t ATCMD_Test(uint8_t tryCount);

#define ATCMD_GetRxBuffer(idx)  AtCmdRxBuff.pData[idx]
#define ATCMD_GetRxSize()       AtCmdRxBuff.Size
#define ATCMD_GetRxLen()        AtCmdRxBuff.Len

#endif