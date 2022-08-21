#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CCP8_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CCP8_ISR_Handler
.extern xISRStackTop
.global CCP8_ISR
.set noreorder
.set noat
.ent CCP8_ISR
CCP8_ISR:
portSAVE_CONTEXT
jal CCP8_ISR_Handler
nop
portRESTORE_CONTEXT
.end CCP8_ISR
