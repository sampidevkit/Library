#ifndef BOOTLOADER_H
#define	BOOTLOADER_H

#include "common/libdef.h"
#include "project_cfg.h"

#ifndef BLD_Trigger_GetState
#define BLD_Trigger_GetState()   (1)
#warning "No trigger signal for bootloader"
#endif

#ifndef BLD_BUFFER_SIZE
#define BLD_BUFFER_SIZE         48
#endif

public void BootLoader_Initialize(void);
public void BootLoader_Deinitialize(void);
public void BootLoader_Tasks(void);

#endif