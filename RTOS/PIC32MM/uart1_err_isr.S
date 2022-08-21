#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _UART1_ERR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _UART1_ERR_Handler
.extern xISRStackTop
.global _UART1_ERR
.set noreorder
.set noat
.ent _UART1_ERR
_UART1_ERR:
portSAVE_CONTEXT
jal _UART1_ERR_Handler
nop
portRESTORE_CONTEXT
.end _UART1_ERR
