#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCT8_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCT8_ISR_Handler
.extern xISRStackTop
.global CCT8_ISR
.set noreorder
.set noat
.ent CCT8_ISR
CCT8_ISR:
portSAVE_CONTEXT
jal CCT8_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCT8_ISR
