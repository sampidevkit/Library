#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _ISR_HLVD_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _ISR_HLVD_Handler
.extern xISRStackTop
.global _ISR_HLVD
.set noreorder
.set noat
.ent _ISR_HLVD
_ISR_HLVD:
portSAVE_CONTEXT
jal _ISR_HLVD_Handler
nop
portRESTORE_CONTEXT
.end _ISR_HLVD
