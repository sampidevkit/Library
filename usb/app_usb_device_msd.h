#ifndef KIT_USB_DEVICE_MSD_H
#define KIT_USB_DEVICE_MSD_H

#include "Common/LibDef.h"
#include "project_cfg.h"

public void KIT_USB_Device_MSD_Init(void);
public void KIT_USB_Device_MSD_Deinit(void);
public void KIT_USB_Device_MSD_Tasks(void *);
public void KIT_USB_Device_MSD_EventInit(void);

#endif