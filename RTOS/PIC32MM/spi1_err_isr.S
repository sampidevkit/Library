#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void SPI1_ERR_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern SPI1_ERR_ISR_Handler
.extern xISRStackTop
.global SPI1_ERR_ISR
.set noreorder
.set noat
.ent SPI1_ERR_ISR
SPI1_ERR_ISR:
portSAVE_CONTEXT
jal SPI1_ERR_ISR_Handler
nop
portRESTORE_CONTEXT
.end SPI1_ERR_ISR
