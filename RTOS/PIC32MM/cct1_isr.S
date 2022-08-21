#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCT1_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCT1_ISR_Handler
.extern xISRStackTop
.global CCT1_ISR
.set noreorder
.set noat
.ent CCT1_ISR
CCT1_ISR:
portSAVE_CONTEXT
jal CCT1_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCT1_ISR
