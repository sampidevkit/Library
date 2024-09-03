#include "ButtonApi.h"
#include "libcomp.h"
/* ********************************************************************** SW1 */
#ifdef SW1_GetValue

private button_t BtSw1;

#define Sw1_Init()  Button_Init(&BtSw1, SW1_GetValue, SW1_TIME_MODE_EDGE_H2L, SW1_TIMEOUT)
#define Sw1_Get()   Button_Is_Pressed(&BtSw1)
#else
#define Sw1_Init()
#define Sw1_Get()   0
#endif

public bool Sw1_Is_Pressed(void)
{
    return Sw1_Get();
}
/* ********************************************************************** SW2 */
#ifdef SW2_GetValue
private button_t BtSw2;

#define Sw2_Init()  Button_Init(&BtSw2, SW2_GetValue, SW2_TIME_MODE_EDGE_H2L, SW2_TIMEOUT)
#define Sw2_Get()   Button_Is_Pressed(&BtSw2)
#else
#define Sw2_Init()
#define Sw2_Get()   0
#endif

public bool Sw2_Is_Pressed(void)
{
    return Sw2_Get();
}
/* ********************************************************************** SW3 */
#ifdef SW3_GetValue
private button_t BtSw3;

#define Sw3_Init()  Button_Init(&BtSw3, SW3_GetValue, SW3_TIME_MODE_EDGE_H2L, SW3_TIMEOUT)
#define Sw3_Get()   Button_Is_Pressed(&BtSw3)
#else
#define Sw3_Init()
#define Sw3_Get()   0
#endif

public bool Sw3_Is_Pressed(void)
{
    return Sw3_Get();
}
/* ********************************************************************** SW4 */
#ifdef SW4_GetValue
private button_t BtSw4;

#define Sw4_Init()  Button_Init(&BtSw4, SW4_GetValue, SW4_TIME_MODE_EDGE_H2L, SW4_TIMEOUT)
#define Sw4_Get()   Button_Is_Pressed(&BtSw4)
#else
#define Sw4_Init()
#define Sw4_Get()   0
#endif

public bool Sw4_Is_Pressed(void)
{
    return Sw4_Get();
}
/* ********************************************************************** SW5 */
#ifdef SW5_GetValue
private button_t BtSw5;

#define Sw5_Init()  Button_Init(&BtSw5, SW5_GetValue, SW5_TIME_MODE_EDGE_H2L, SW5_TIMEOUT)
#define Sw5_Get()   Button_Is_Pressed(&BtSw5)
#else
#define Sw5_Init()
#define Sw5_Get()   0
#endif

public bool Sw5_Is_Pressed(void)
{
    return Sw5_Get();
}
/* ********************************************************************** SW6 */
#ifdef SW6_GetValue
private button_t BtSw6;

#define Sw6_Init()  Button_Init(&BtSw6, SW6_GetValue, SW6_TIME_MODE_EDGE_H2L, SW6_TIMEOUT)
#define Sw6_Get()   Button_Is_Pressed(&BtSw6)
#else
#define Sw6_Init()
#define Sw6_Get()   0
#endif

public bool Sw6_Is_Pressed(void)
{
    return Sw6_Get();
}
/* ********************************************************************** SW7 */
#ifdef SW7_GetValue
private button_t BtSw7;

#define Sw7_Init()  Button_Init(&BtSw7, SW7_GetValue, SW7_TIME_MODE_EDGE_H2L, SW7_TIMEOUT)
#define Sw7_Get()   Button_Is_Pressed(&BtSw7)
#else
#define Sw7_Init()
#define Sw7_Get()   0
#endif

public bool Sw7_Is_Pressed(void)
{
    return Sw7_Get();
}
/* ********************************************************************** SW8 */
#ifdef SW8_GetValue
private button_t BtSw8;

#define Sw8_Init()  Button_Init(&BtSw8, SW8_GetValue, SW8_TIME_MODE_EDGE_H2L, SW8_TIMEOUT)
#define Sw8_Get()   Button_Is_Pressed(&BtSw8)
#else
#define Sw8_Init()
#define Sw8_Get()   0
#endif

public bool Sw8_Is_Pressed(void)
{
    return Sw8_Get();
}
/* ****************************************************************** SW Init */
public void ButtonApi_Init(void)
{
    Sw1_Init();
    Sw2_Init();
    Sw3_Init();
    Sw4_Init();
    Sw5_Init();
    Sw6_Init();
    Sw7_Init();
    Sw8_Init();
}