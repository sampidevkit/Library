#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void DMA3_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern DMA3_ISR_Handler
.extern xISRStackTop
.global DMA3_ISR
.set noreorder
.set noat
.ent DMA3_ISR
DMA3_ISR:
portSAVE_CONTEXT
jal DMA3_ISR_Handler
nop
portRESTORE_CONTEXT
.end DMA3_ISR
