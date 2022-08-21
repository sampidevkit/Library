#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void NVM_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern NVM_ISR_Handler
.extern xISRStackTop
.global NVM_ISR
.set noreorder
.set noat
.ent NVM_ISR
NVM_ISR:
portSAVE_CONTEXT
jal NVM_ISR_Handler
nop
portRESTORE_CONTEXT
.end NVM_ISR
