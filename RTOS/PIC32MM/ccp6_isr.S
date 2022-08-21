#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP6_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP6_ISR_Handler
.extern xISRStackTop
.global CCP6_ISR
.set noreorder
.set noat
.ent CCP6_ISR
CCP6_ISR:
portSAVE_CONTEXT
jal CCP6_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP6_ISR
