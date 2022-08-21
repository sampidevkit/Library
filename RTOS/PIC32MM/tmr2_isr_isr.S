#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void TMR2_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern TMR2_ISR_Handler
.extern xISRStackTop
.global TMR2_ISR
.set noreorder
.set noat
.ent TMR2_ISR
TMR2_ISR:
portSAVE_CONTEXT
jal TMR2_ISR_Handler
nop
portRESTORE_CONTEXT
.end TMR2_ISR
