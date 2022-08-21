#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void ADC1_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern ADC1_ISR_Handler
.extern xISRStackTop
.global ADC1_ISR
.set noreorder
.set noat
.ent ADC1_ISR
ADC1_ISR:
portSAVE_CONTEXT
jal ADC1_ISR_Handler
nop
portRESTORE_CONTEXT
.end ADC1_ISR
