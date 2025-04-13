#ifndef BOOTLOADER_H
#define	BOOTLOADER_H

#include "common/libdef.h"
#include "project_cfg.h"

#ifndef BLD_BUFFER_SIZE
#define BLD_BUFFER_SIZE         48
#endif

/* **************************************************************** PROTOTYPE */
void BLD_Comm_Init(void);
void BLD_SystemReboot(void);
bool BLD_Trigger_GetState(void);
bool BLD_CheckCodeProtect(void);
/* ************************************************************************** */
public void BootLoader_Initialize(void);
public void BootLoader_Deinitialize(void);
public void BootLoader_Tasks(void);

#endif