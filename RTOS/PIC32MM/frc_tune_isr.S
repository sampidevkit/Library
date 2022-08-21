#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void FRC_TUNE_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern FRC_TUNE_Handler
.extern xISRStackTop
.global FRC_TUNE
.set noreorder
.set noat
.ent FRC_TUNE
FRC_TUNE:
portSAVE_CONTEXT
jal FRC_TUNE_Handler
nop
portRESTORE_CONTEXT
.end FRC_TUNE
