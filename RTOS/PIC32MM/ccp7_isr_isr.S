#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP7_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP7_ISR_Handler
.extern xISRStackTop
.global CCP7_ISR
.set noreorder
.set noat
.ent CCP7_ISR
CCP7_ISR:
portSAVE_CONTEXT
jal CCP7_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP7_ISR
