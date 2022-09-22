#include "QWIIC.h"
#include "libcomp.h"

int8_t qwiic_last_state=QWIIC_ERR;

void QWIIC_WriteNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz);
void QWIIC_ReadNBytes(uint8_t SlvAddr, uint8_t *pD, uint8_t sz);