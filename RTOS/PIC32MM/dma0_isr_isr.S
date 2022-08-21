#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void DMA0_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern DMA0_ISR_Handler
.extern xISRStackTop
.global DMA0_ISR
.set noreorder
.set noat
.ent DMA0_ISR
DMA0_ISR:
portSAVE_CONTEXT
jal DMA0_ISR_Handler
nop
portRESTORE_CONTEXT
.end DMA0_ISR
