#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void DMA1_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern DMA1_ISR_Handler
.extern xISRStackTop
.global DMA1_ISR
.set noreorder
.set noat
.ent DMA1_ISR
DMA1_ISR:
portSAVE_CONTEXT
jal DMA1_ISR_Handler
nop
portRESTORE_CONTEXT
.end DMA1_ISR
