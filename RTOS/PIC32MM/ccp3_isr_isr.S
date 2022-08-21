#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP3_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP3_ISR_Handler
.extern xISRStackTop
.global CCP3_ISR
.set noreorder
.set noat
.ent CCP3_ISR
CCP3_ISR:
portSAVE_CONTEXT
jal CCP3_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP3_ISR
