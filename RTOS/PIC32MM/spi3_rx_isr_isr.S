#include <p32xxxx.h>
#include <sys/asm.h>
#include "ISR_Support.h"
/* Copy this line to your C source
#ifdef USE_RTOS
;
void SPI3_RX_ISR_Handler(void);
#endif
******************************** */
.set nomips16
.set noreorder
.extern SPI3_RX_ISR_Handler
.extern xISRStackTop
.global SPI3_RX_ISR
.set noreorder
.set noat
.ent SPI3_RX_ISR
SPI3_RX_ISR:
portSAVE_CONTEXT
jal SPI3_RX_ISR_Handler
nop
portRESTORE_CONTEXT
.end SPI3_RX_ISR
