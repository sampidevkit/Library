#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void USB_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern USB_ISR_Handler
.extern xISRStackTop
.global USB_ISR
.set noreorder
.set noat
.ent USB_ISR
USB_ISR:
portSAVE_CONTEXT
jal USB_ISR_Handler
nop
portRESTORE_CONTEXT
.end USB_ISR
