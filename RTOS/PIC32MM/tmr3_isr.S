#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void TMR3_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern TMR3_ISR_Handler
.extern xISRStackTop
.global TMR3_ISR
.set noreorder
.set noat
.ent TMR3_ISR
TMR3_ISR:
portSAVE_CONTEXT
jal TMR3_ISR_Handler
nop
portRESTORE_CONTEXT
.end TMR3_ISR
