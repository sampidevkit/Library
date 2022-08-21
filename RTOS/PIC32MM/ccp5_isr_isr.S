#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP5_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP5_ISR_Handler
.extern xISRStackTop
.global CCP5_ISR
.set noreorder
.set noat
.ent CCP5_ISR
CCP5_ISR:
portSAVE_CONTEXT
jal CCP5_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP5_ISR
