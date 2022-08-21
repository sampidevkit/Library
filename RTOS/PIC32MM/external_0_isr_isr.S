#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _EXTERNAL_0_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _EXTERNAL_0_ISR_Handler
.extern xISRStackTop
.global _EXTERNAL_0_ISR
.set noreorder
.set noat
.ent _EXTERNAL_0_ISR
_EXTERNAL_0_ISR:
portSAVE_CONTEXT
jal _EXTERNAL_0_ISR_Handler
nop
portRESTORE_CONTEXT
.end _EXTERNAL_0_ISR
