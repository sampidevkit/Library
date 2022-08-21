#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CLC2Interrupt_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CLC2Interrupt_Handler
.extern xISRStackTop
.global _CLC2Interrupt
.set noreorder
.set noat
.ent _CLC2Interrupt
_CLC2Interrupt:
portSAVE_CONTEXT
jal _CLC2Interrupt_Handler
nop
portRESTORE_CONTEXT
.end _CLC2Interrupt
