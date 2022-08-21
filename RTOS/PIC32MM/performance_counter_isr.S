#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void PERFORMANCE_COUNTER_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern PERFORMANCE_COUNTER_ISR_Handler
.extern xISRStackTop
.global PERFORMANCE_COUNTER_ISR
.set noreorder
.set noat
.ent PERFORMANCE_COUNTER_ISR
PERFORMANCE_COUNTER_ISR:
portSAVE_CONTEXT
jal PERFORMANCE_COUNTER_ISR_Handler
nop
portRESTORE_CONTEXT
.end PERFORMANCE_COUNTER_ISR
