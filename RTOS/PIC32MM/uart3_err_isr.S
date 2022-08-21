#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _UART3_ERR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _UART3_ERR_Handler
.extern xISRStackTop
.global _UART3_ERR
.set noreorder
.set noat
.ent _UART3_ERR
_UART3_ERR:
portSAVE_CONTEXT
jal _UART3_ERR_Handler
nop
portRESTORE_CONTEXT
.end _UART3_ERR
