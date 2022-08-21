#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void DMA2_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern DMA2_ISR_Handler
.extern xISRStackTop
.global DMA2_ISR
.set noreorder
.set noat
.ent DMA2_ISR
DMA2_ISR:
portSAVE_CONTEXT
jal DMA2_ISR_Handler
nop
portRESTORE_CONTEXT
.end DMA2_ISR
