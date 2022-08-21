#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCT9_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCT9_ISR_Handler
.extern xISRStackTop
.global CCT9_ISR
.set noreorder
.set noat
.ent CCT9_ISR
CCT9_ISR:
portSAVE_CONTEXT
jal CCT9_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCT9_ISR
