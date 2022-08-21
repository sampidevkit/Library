#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void TMR1_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern TMR1_ISR_Handler
.extern xISRStackTop
.global TMR1_ISR
.set noreorder
.set noat
.ent TMR1_ISR
TMR1_ISR:
portSAVE_CONTEXT
jal TMR1_ISR_Handler
nop
portRESTORE_CONTEXT
.end TMR1_ISR
