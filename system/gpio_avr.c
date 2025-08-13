#include "Gpio.h"
#include "misc/util.h"

void Gpio_Set(volatile uint32_t *baseReg, gpio_pin_t pin, uint8_t val)
{
    if((baseReg<(volatile uint32_t *)&ADC0.CTRLA)||(baseReg>(volatile uint32_t *)&ADC0.CTRLF))
    {
        if(val==0) // Clear
            baseReg+=1; // Set
        else if(val==1)
            baseReg+=2;
        else // Invert
            baseReg+=3;
    }

    if(pin<=PIN_A7) // Port A
    {
        baseReg+=0;
        pin-=PIN_A0;
    }
    else if(pin<=PIN_B7) // Port B
    {
        baseReg+=0x20;
        pin-=PIN_B0;
    }
    else if(pin<=PIN_C7) // Port C
    {
        baseReg+=0x40;
        pin-=PIN_C0;
    }
    else if(pin<=PIN_D7)// Port D
    {
        baseReg+=0x60;
        pin-=PIN_D0;
    }
    else if(pin<=PIN_E7)// Port E
    {
        baseReg+=0x80;
        pin-=PIN_D0;
    }
    else if(pin<=PIN_F6)// Port F
    {
        baseReg+=0xA0;
        pin-=PIN_D0;
    }
    else if(pin<=PIN_G7)// Port G
    {
        baseReg+=0xC0;
        pin-=PIN_D0;
    }
    else
        return;

    *baseReg=(1<<pin);
}

bool Gpio_Get(volatile uint32_t *baseReg, gpio_pin_t pin)
{
    if(pin<=PIN_A7) // Port A
    {
        baseReg+=0;
        pin-=PIN_A0;
    }
    else if(pin<=PIN_B7) // Port B
    {
        baseReg+=0x20;
        pin-=PIN_B0;
    }
    else if(pin<=PIN_C7) // Port C
    {
        baseReg+=0x40;
        pin-=PIN_C0;
    }
    else if(pin<=PIN_D7)// Port D
    {
        baseReg+=0x60;
        pin-=PIN_D0;
    }
    else if(pin<=PIN_E7)// Port E
    {
        baseReg+=0x80;
        pin-=PIN_D0;
    }
    else if(pin<=PIN_F6)// Port F
    {
        baseReg+=0xA0;
        pin-=PIN_D0;
    }
    else if(pin<=PIN_G7)// Port G
    {
        baseReg+=0xC0;
        pin-=PIN_D0;
    }
    else
        return 0;

    return BitTest((*baseReg), pin);
}

void Gpio_GetPinName(char *pStr, gpio_pin_t pin)
{
    if(pin<=PIN_A7) // Port A
    {
        *pStr='A';
        pin-=PIN_A0;
    }
    else if(pin<=PIN_B7) // Port B
    {
        *pStr='B';
        pin-=PIN_B0;
    }
    else if(pin<=PIN_C7) // Port C
    {
        *pStr='C';
        pin-=PIN_C0;
    }
    else if(pin<=PIN_D7)// Port D
    {
        *pStr='D';
        pin-=PIN_D0;
    }
    else if(pin<=PIN_E7)// Port E
    {
        *pStr='E';
        pin-=PIN_D0;
    }
    else if(pin<=PIN_F6)// Port F
    {
        *pStr='F';
        pin-=PIN_D0;
    }
    else if(pin<=PIN_G7)// Port G
    {
        *pStr='G';
        pin-=PIN_D0;
    }
    else
        return;

    pStr++;
    u32str(pin, pStr);
}
