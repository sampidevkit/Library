#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CLC4Interrupt_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CLC4Interrupt_Handler
.extern xISRStackTop
.global _CLC4Interrupt
.set noreorder
.set noat
.ent _CLC4Interrupt
_CLC4Interrupt:
portSAVE_CONTEXT
jal _CLC4Interrupt_Handler
nop
portRESTORE_CONTEXT
.end _CLC4Interrupt
