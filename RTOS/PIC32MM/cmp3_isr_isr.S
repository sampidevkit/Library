#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CMP3_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CMP3_ISR_Handler
.extern xISRStackTop
.global _CMP3_ISR
.set noreorder
.set noat
.ent _CMP3_ISR
_CMP3_ISR:
portSAVE_CONTEXT
jal _CMP3_ISR_Handler
nop
portRESTORE_CONTEXT
.end _CMP3_ISR
