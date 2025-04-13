#ifndef KIT_USB_DEVICE_H
#define	KIT_USB_DEVICE_H

#include "Common/LibDef.h"
#include "project_cfg.h"

public void KIT_USB_Device_Init(void);
public void KIT_USB_Device_Deinit(void);
public void KIT_USB_Device_Tasks(void *);

#endif