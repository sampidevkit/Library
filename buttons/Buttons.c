#include "Buttons.h"

public bool Button_Is_Pressed(button_t *pBt) // <editor-fold defaultstate="collapsed" desc="Check pressed button">
{
    switch(pBt->BtTask)
    {
        case BT_CHECK_NOISE:
            if(pBt->Mode<COUNT_MODE_EDGE_L2H)
            {
                if(Tick_Timer_Is_Over_Ms(pBt->TickTimer, pBt->DetectTime))
                {
                    pBt->BtTask=BT_PRESSED;
                    if(pBt->Mode==TIME_MODE_EDGE_H2L)
                        return 1;
                }
                else
                    pBt->BtTask=BT_CHECK_AGAIN;
            }
            else
            {
                if(Tick_Counter_Is_Over(&pBt->TickCounter, pBt->DetectTime))
                {
                    pBt->BtTask=BT_PRESSED;
                    if(pBt->Mode==COUNT_MODE_EDGE_H2L)
                        return 1;
                }
                else
                    pBt->BtTask=BT_CHECK_AGAIN;
            }
            break;

        case BT_CHECK_AGAIN:
            if(pBt->SignalFunc()==0)
                pBt->BtTask=BT_CHECK_NOISE;
            else
            {
                Tick_Timer_Reset(pBt->TickTimer);
                Tick_Counter_Reset(pBt->TickCounter);
                pBt->BtTask=BT_UNPRESSED;
            }
            break;

        case BT_PRESSED:
            if(pBt->SignalFunc()==1)
            {
                pBt->BtTask=BT_UNPRESSED;
                if((pBt->Mode==TIME_MODE_EDGE_L2H)||(pBt->Mode==COUNT_MODE_EDGE_L2H))
                    return 1;
            }
            break;

        case BT_UNPRESSED:
        default:
            if(pBt->SignalFunc()==0)
            {
                Tick_Timer_Reset(pBt->TickTimer);
                Tick_Counter_Reset(pBt->TickCounter);
                pBt->BtTask=BT_CHECK_NOISE;
            }
            break;
    }

    return 0;
} // </editor-fold>

public void Button_Reset(button_t *pBt) // <editor-fold defaultstate="collapsed" desc="Button reset">
{
    pBt->BtTask=BT_UNPRESSED;
    Tick_Timer_Reset(pBt->TickTimer);
    Tick_Counter_Reset(pBt->TickCounter);
} // </editor-fold>

public void Button_Init(button_t *pBt, signal_handler_t SignalFunc,
                        detect_mode_t Mode, uint16_t DetectTime) // <editor-fold defaultstate="collapsed" desc="Button Time Detect Initialize">
{
    pBt->Mode=Mode;
    pBt->BtTask=BT_UNPRESSED;
    pBt->DetectTime=DetectTime;
    pBt->SignalFunc=SignalFunc;

    if(Mode<COUNT_MODE_EDGE_L2H) // Timer detection
        Tick_Timer_Reset(pBt->TickTimer);
    else // Counter detection
        Tick_Counter_Reset(pBt->TickCounter);
} // </editor-fold>