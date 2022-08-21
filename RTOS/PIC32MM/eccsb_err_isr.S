#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void ECCSB_ERR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern ECCSB_ERR_Handler
.extern xISRStackTop
.global ECCSB_ERR
.set noreorder
.set noat
.ent ECCSB_ERR
ECCSB_ERR:
portSAVE_CONTEXT
jal ECCSB_ERR_Handler
nop
portRESTORE_CONTEXT
.end ECCSB_ERR
