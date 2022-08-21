#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CLC1Interrupt_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CLC1Interrupt_Handler
.extern xISRStackTop
.global _CLC1Interrupt
.set noreorder
.set noat
.ent _CLC1Interrupt
_CLC1Interrupt:
portSAVE_CONTEXT
jal _CLC1Interrupt_Handler
nop
portRESTORE_CONTEXT
.end _CLC1Interrupt
