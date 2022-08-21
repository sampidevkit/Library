#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CORE_TIMER_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CORE_TIMER_ISR_Handler
.extern xISRStackTop
.global _CORE_TIMER_ISR
.set noreorder
.set noat
.ent _CORE_TIMER_ISR
_CORE_TIMER_ISR:
portSAVE_CONTEXT
jal _CORE_TIMER_ISR_Handler
nop
portRESTORE_CONTEXT
.end _CORE_TIMER_ISR
