#ifndef QWIIC_H
#define QWIIC_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    QWIIC_OK = 0,
    QWIIC_BUSY = 1,
    QWIIC_ERR = (-1)
} qwiic_stt_t;

extern qwiic_stt_t qwiic_last_state;

#define QWIIC_GetLastState() qwiic_last_state

void QWIIC_WriteNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz);
void QWIIC_ReadNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz);

#endif