#ifndef BUTTONAPI_H
#define	BUTTONAPI_H

#include "Common/LibDef.h"

#ifdef COMMON_LIB_CFG
#include "Common_Lib_Cfg.h"
#else
#include "Buttons_Cfg.h"
#endif

public void ButtonApi_Init(void);
public bool Sw1_Is_Pressed(void);
public bool Sw2_Is_Pressed(void);
public bool Sw3_Is_Pressed(void);
public bool Sw4_Is_Pressed(void);
public bool Sw5_Is_Pressed(void);
public bool Sw6_Is_Pressed(void);
public bool Sw7_Is_Pressed(void);
public bool Sw8_Is_Pressed(void);

#endif