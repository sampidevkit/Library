#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _CHANGE_NOTICE_A_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _CHANGE_NOTICE_A_Handler
.extern xISRStackTop
.global _CHANGE_NOTICE_A
.set noreorder
.set noat
.ent _CHANGE_NOTICE_A
_CHANGE_NOTICE_A:
portSAVE_CONTEXT
jal _CHANGE_NOTICE_A_Handler
nop
portRESTORE_CONTEXT
.end _CHANGE_NOTICE_A
