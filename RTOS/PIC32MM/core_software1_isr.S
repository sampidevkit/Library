#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void CORE_SOFTWARE1_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern CORE_SOFTWARE1_Handler
.extern xISRStackTop
.global CORE_SOFTWARE1
.set noreorder
.set noat
.ent CORE_SOFTWARE1
CORE_SOFTWARE1:
portSAVE_CONTEXT
jal CORE_SOFTWARE1_Handler
nop
portRESTORE_CONTEXT
.end CORE_SOFTWARE1
