#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void SPI2_ERR_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern SPI2_ERR_ISR_Handler
.extern xISRStackTop
.global SPI2_ERR_ISR
.set noreorder
.set noat
.ent SPI2_ERR_ISR
SPI2_ERR_ISR:
portSAVE_CONTEXT
jal SPI2_ERR_ISR_Handler
nop
portRESTORE_CONTEXT
.end SPI2_ERR_ISR
