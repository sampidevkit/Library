#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP9_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP9_ISR_Handler
.extern xISRStackTop
.global CCP9_ISR
.set noreorder
.set noat
.ent CCP9_ISR
CCP9_ISR:
portSAVE_CONTEXT
jal CCP9_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP9_ISR
