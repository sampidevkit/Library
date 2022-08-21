#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _UART1_TX_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _UART1_TX_Handler
.extern xISRStackTop
.global _UART1_TX
.set noreorder
.set noat
.ent _UART1_TX
_UART1_TX:
portSAVE_CONTEXT
jal _UART1_TX_Handler
nop
portRESTORE_CONTEXT
.end _UART1_TX
