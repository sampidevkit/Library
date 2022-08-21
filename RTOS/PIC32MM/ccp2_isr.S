#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP2_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP2_ISR_Handler
.extern xISRStackTop
.global CCP2_ISR
.set noreorder
.set noat
.ent CCP2_ISR
CCP2_ISR:
portSAVE_CONTEXT
jal CCP2_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP2_ISR
