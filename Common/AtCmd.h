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

extern buff_t *pAtCmdRxBkBuff;
// Response constants
extern const char RES_OK[];

void ATCMD_Init(buff_t *pRxBuffer);
void ATCMD_Delay(uint16_t delayMs);
void ATCMD_SendRaw(const uint8_t *pD, int sz);
int8_t ATCMD_GetRaw(uint8_t *pD, int *pSz, uint16_t firstWait, uint16_t lastWait);
int8_t ATCMD_SendGetDat(const char *pTx, char *pRx, uint16_t firstWait, uint16_t lastWait);
int8_t ATCMD_SendGetAck(const char *pTx, const char *pAck, uint16_t firstWait, uint16_t lastWait, uint8_t tryCount);

#define ATCMD_Test(x)               ATCMD_SendGetAck("AT\r", RES_OK, 250, 250, x)
#define ATCMD_RxBuffer_Is_Ready()   (pAtCmdRxBkBuff!=NULL)
#define ATCMD_SetRxBuffer(buf)      pAtCmdRxBkBuff=buf
#define ATCMD_GetRxBuffer(idx)      pAtCmdRxBkBuff->pData[idx]
#define ATCMD_GetRxBufferSize()     pAtCmdRxBkBuff->Len

#endif