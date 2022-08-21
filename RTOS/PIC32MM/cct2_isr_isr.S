#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCT2_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCT2_ISR_Handler
.extern xISRStackTop
.global CCT2_ISR
.set noreorder
.set noat
.ent CCT2_ISR
CCT2_ISR:
portSAVE_CONTEXT
jal CCT2_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCT2_ISR
