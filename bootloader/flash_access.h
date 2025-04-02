#ifndef BLD_NVM_PIC32_H
#define	BLD_NVM_PIC32_H

#include "common/libdef.h"
#include "project_cfg.h"

public int8_t BLD_Nvm_Write(uint32_t Addr, const uint8_t *pData, int Len);

#endif