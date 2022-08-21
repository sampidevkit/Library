#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CMP2_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CMP2_ISR_Handler
.extern xISRStackTop
.global _CMP2_ISR
.set noreorder
.set noat
.ent _CMP2_ISR
_CMP2_ISR:
portSAVE_CONTEXT
jal _CMP2_ISR_Handler
nop
portRESTORE_CONTEXT
.end _CMP2_ISR
