#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _ISR_RTCC_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _ISR_RTCC_Handler
.extern xISRStackTop
.global _ISR_RTCC
.set noreorder
.set noat
.ent _ISR_RTCC
_ISR_RTCC:
portSAVE_CONTEXT
jal _ISR_RTCC_Handler
nop
portRESTORE_CONTEXT
.end _ISR_RTCC
