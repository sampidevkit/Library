#ifndef QWIIC_H
#define QWIIC_H

#include <stdint.h>
#include <stdbool.h>

#define QWIIC_OK    (0)
#define QWIIC_BUSY  (1)
#define QWIIC_ERR   (-1)

extern int8_t qwiic_last_state;

#define QWIIC_GetLastState()    qwiic_last_state

void QWIIC_WriteNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz);
void QWIIC_ReadNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz);

#endif