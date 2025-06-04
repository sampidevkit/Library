#ifndef FLASH_ACCESS_H
#define	FLASH_ACCESS_H

#include "common/libdef.h"
#include "project_cfg.h"

public int8_t Flash_Access_Write(uint32_t Addr, const uint8_t *pData, int Len);
public void Flash_Access_Read(uint32_t Addr, void *pData, int Len);
    
#endif
    