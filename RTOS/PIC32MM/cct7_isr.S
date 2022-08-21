#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCT7_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCT7_ISR_Handler
.extern xISRStackTop
.global CCT7_ISR
.set noreorder
.set noat
.ent CCT7_ISR
CCT7_ISR:
portSAVE_CONTEXT
jal CCT7_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCT7_ISR
