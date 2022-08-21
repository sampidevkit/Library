#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP4_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP4_ISR_Handler
.extern xISRStackTop
.global CCP4_ISR
.set noreorder
.set noat
.ent CCP4_ISR
CCP4_ISR:
portSAVE_CONTEXT
jal CCP4_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP4_ISR
