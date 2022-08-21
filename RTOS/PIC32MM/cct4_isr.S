#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCT4_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCT4_ISR_Handler
.extern xISRStackTop
.global CCT4_ISR
.set noreorder
.set noat
.ent CCT4_ISR
CCT4_ISR:
portSAVE_CONTEXT
jal CCT4_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCT4_ISR
