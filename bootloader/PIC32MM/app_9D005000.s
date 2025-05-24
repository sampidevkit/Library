/*
    ** file: app_asm.s
*/
#include <xc.h>
    
    .text
    .set noreorder 
/*********************************************************************
* void appFunction(void)
* This function jump specified address
********************************************************************/
.global appFunction
.ent appFunction
appFunction:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    jals 0x9D005000
    nop
    
.end appFunction     
    