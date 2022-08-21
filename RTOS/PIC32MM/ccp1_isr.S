#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP1_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP1_ISR_Handler
.extern xISRStackTop
.global CCP1_ISR
.set noreorder
.set noat
.ent CCP1_ISR
CCP1_ISR:
portSAVE_CONTEXT
jal CCP1_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP1_ISR
