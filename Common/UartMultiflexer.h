#ifndef UARTMULTIFLEXER_H
#define UARTMULTIFLEXER_H

#include "LibDef.h"
#include "UartMultiflexer_Cfg.h"

typedef struct PORT_CXT {
    bool (*isRxReady)(void);
    bool (*isTxReady)(void);
    bool (*isTxDone)(void);
    uint8_t(*ReadByte)(void);
    void (*WriteByte)(uint8_t);
} port_cxt_t;

#ifndef MAX_TX_DATA_PER_PORT
#define MAX_TX_DATA_PER_PORT 8
#endif

extern const port_cxt_t DownStreamPort[NUM_OF_DOWNSTREAM_PORTS]; // Defined in UartMultiflexer_Cfg.c

void UartMux_Tasks(void *);

#endif