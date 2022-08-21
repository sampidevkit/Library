#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CLC3Interrupt_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CLC3Interrupt_Handler
.extern xISRStackTop
.global _CLC3Interrupt
.set noreorder
.set noat
.ent _CLC3Interrupt
_CLC3Interrupt:
portSAVE_CONTEXT
jal _CLC3Interrupt_Handler
nop
portRESTORE_CONTEXT
.end _CLC3Interrupt
