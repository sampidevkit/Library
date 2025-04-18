#ifndef SX1268_TRANSCEIVER_H
#define SX1268_TRANSCEIVER_H

#include "common/libdef.h"
#include "project_cfg.h"

bool sx1268_transceiver_init(void);
bool sx1268_transceiver_is_rx_ready(void);
bool sx1268_transceiver_is_tx_ready(void);
bool sx1268_transceiver_is_tx_done(void);
void sx1268_transceiver_send(const uint8_t *pD, uint8_t len);
uint8_t sx1268_transceiver_receive(uint8_t *pD);
void sx1268_task(void);

#endif
