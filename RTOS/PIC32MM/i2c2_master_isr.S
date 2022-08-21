#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void _I2C2_MASTER_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern _I2C2_MASTER_Handler
.extern xISRStackTop
.global _I2C2_MASTER
.set noreorder
.set noat
.ent _I2C2_MASTER
_I2C2_MASTER:
portSAVE_CONTEXT
jal _I2C2_MASTER_Handler
nop
portRESTORE_CONTEXT
.end _I2C2_MASTER
