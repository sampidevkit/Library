#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCT3_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCT3_ISR_Handler
.extern xISRStackTop
.global CCT3_ISR
.set noreorder
.set noat
.ent CCT3_ISR
CCT3_ISR:
portSAVE_CONTEXT
jal CCT3_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCT3_ISR
