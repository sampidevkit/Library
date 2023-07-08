#ifndef UARTMULTIFLEXER_H
#define UARTMULTIFLEXER_H

#include "LibDef.h"
#include "UartMultiflexer_Cfg.h"

typedef struct PORT_CXT
{
    bool (*isRxReady)(void);
    bool (*isTxReady)(void);
    uint8_t (*ReadByte)(void);
    void (*WriteByte)(uint8_t);
    
    struct PORT_CXT *Next;
} port_cxt_t;

#if(NUM_OF_PORTS>0)
extern const port_cxt_t PortCxt[NUM_OF_PORTS];
#endif

#endif