#ifndef UARTMULTIFLEXER_H
#define UARTMULTIFLEXER_H

#include "LibDef.h"
#include "UartMultiflexer_Cfg.h"

typedef struct PORT_CXT
{
    bool (*isRxReady)(void);
    bool (*isTxReady)(void);
    bool (*isTxDone)(void);
    uint8_t (*ReadByte)(void);
    void (*WriteByte)(uint8_t);
} port_cxt_t;

#if(NUM_OF_DOWNSTREAM_PORTS>0)
extern const port_cxt_t DownStreamPortCxt[NUM_OF_DOWNSTREAM_PORTS];
#endif

#endif