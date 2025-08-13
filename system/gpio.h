#ifndef GPIO_H
#define GPIO_H

#include <xc.h>
#include "common/libdef.h"
#include "project_cfg.h"

#define HIGH    1
#define LOW     0
#define TOGGLE  2
#define PULLENA 1
#define PULLDIS 0
#define ANALOG  1
#define DIGITAL 0

#ifdef __PIC32MM__
#define INPUT   1
#define OUTPUT  0
#elif defined(__AVR__)
#define INPUT   0
#define OUTPUT  1
#endif

typedef enum {
#ifdef __PIC32MM__
    PIN_A0 = 0,
    PIN_A1,
    PIN_A2,
    PIN_A3,
    PIN_A4,
    PIN_A5,
    PIN_A6,
    PIN_A7,
    PIN_A8,
    PIN_A9,
    PIN_A10,
    PIN_A11,
    PIN_A12,
    PIN_A13,
    PIN_A14,
    PIN_A15,

    PIN_B0,
    PIN_B1,
    PIN_B2,
    PIN_B3,
    PIN_B4,
    PIN_B5,
    PIN_B6,
    PIN_B7,
    PIN_B8,
    PIN_B9,
    PIN_B10,
    PIN_B11,
    PIN_B12,
    PIN_B13,
    PIN_B14,
    PIN_B15,

    PIN_C0,
    PIN_C1,
    PIN_C2,
    PIN_C3,
    PIN_C4,
    PIN_C5,
    PIN_C6,
    PIN_C7,
    PIN_C8,
    PIN_C9,
    PIN_C10,
    PIN_C11,
    PIN_C12,
    PIN_C13,
    PIN_C14,
    PIN_C15,

    PIN_D0,
    PIN_D1,
    PIN_D2,
    PIN_D3,
    PIN_D4,
#elif defined(__AVR__)
    PIN_A0 = 0,
    PIN_A1,
    PIN_A2,
    PIN_A3,
    PIN_A4,
    PIN_A5,
    PIN_A6,
    PIN_A7,

    PIN_B0,
    PIN_B1,
    PIN_B2,
    PIN_B3,
    PIN_B4,
    PIN_B5,
    PIN_B6,
    PIN_B7,

    PIN_C0,
    PIN_C1,
    PIN_C2,
    PIN_C3,
    PIN_C4,
    PIN_C5,
    PIN_C6,
    PIN_C7,

    PIN_D0,
    PIN_D1,
    PIN_D2,
    PIN_D3,
    PIN_D4,
    PIN_D5,
    PIN_D6,
    PIN_D7,

    PIN_E0,
    PIN_E1,
    PIN_E2,
    PIN_E3,
    PIN_E4,
    PIN_E5,
    PIN_E6,
    PIN_E7,

    PIN_F0,
    PIN_F1,
    PIN_F2,
    PIN_F3,
    PIN_F4,
    PIN_F5,
    PIN_F6,

    PIN_G0 = 49,
    PIN_G1,
    PIN_G2,
    PIN_G3,
    PIN_G4,
    PIN_G5,
    PIN_G6,
    PIN_G7,
#else
#warning "Please define GPIO PIN"
#endif
    NOT_USE
} gpio_pin_t;

bool Gpio_Get(volatile uint32_t *baseReg, gpio_pin_t pin);
void Gpio_Set(volatile uint32_t *baseReg, gpio_pin_t pin, uint8_t val);
void Gpio_GetPinName(char *pStr, gpio_pin_t pin);

#ifdef __PIC32__
#define Gpio_SetAnalogInputOn(pin)  Gpio_Set(&ANSELA, (gpio_pin_t)pin, ANALOG)
#define Gpio_SetAnalogInputOff(pin) Gpio_Set(&ANSELA, (gpio_pin_t)pin, DIGITAL)
#define Gpio_SetDigitalInput(pin)   Gpio_Set(&TRISA, (gpio_pin_t)pin, INPUT)
#define Gpio_SetDigitalOutput(pin)  Gpio_Set(&TRISA, (gpio_pin_t)pin, OUTPUT)
#define Gpio_PullupOn(pin)          Gpio_Set(&CNPUA, (gpio_pin_t)pin, PULLENA)
#define Gpio_PullupOff(pin)         Gpio_Set(&CNPUA, (gpio_pin_t)pin, PULLDIS)
#define Gpio_PulldownOn(pin)        Gpio_Set(&CNPDA, (gpio_pin_t)pin, PULLENA)
#define Gpio_PulldownOff(pin)       Gpio_Set(&CNPDA, (gpio_pin_t)pin, PULLDIS)
#define Gpio_GetState(pin)          Gpio_Get(&PORTA, (gpio_pin_t)pin)
#define Gpio_SetState(pin, stt)     Gpio_Set(&LATA, (gpio_pin_t)pin, stt)
#define Gpio_SetHigh(pin)           Gpio_Set(&LATA, (gpio_pin_t)pin, HIGH)
#define Gpio_SetLow(pin)            Gpio_Set(&LATA, (gpio_pin_t)pin, LOW)
#define Gpio_Toggle(pin)            Gpio_Set(&LATA, (gpio_pin_t)pin, TOGGLE)
#elif defined __AVR__
#define Gpio_SetAnalogInputOn(pin)  Gpio_Set((volatile uint32_t *)&ADC0.CTRLA, (gpio_pin_t)pin, ANALOG)
#define Gpio_SetAnalogInputOff(pin) Gpio_Set((volatile uint32_t *)&ADC0.CTRLA, (gpio_pin_t)pin, DIGITAL)
#define Gpio_SetDigitalInput(pin)   Gpio_Set((volatile uint32_t *)&PORTA.DIR, (gpio_pin_t)pin, INPUT)
#define Gpio_SetDigitalOutput(pin)  Gpio_Set((volatile uint32_t *)&PORTA.DIR, (gpio_pin_t)pin, OUTPUT)
#define Gpio_PullupOn(pin)          Gpio_Set((volatile uint32_t *)&PORTA.PINCTRLUPD, (gpio_pin_t)pin, PULLENA)
#define Gpio_PullupOff(pin)         Gpio_Set((volatile uint32_t *)&PORTA.PINCTRLUPD, (gpio_pin_t)pin, PULLDIS)
#define Gpio_PulldownOn(pin)        
#define Gpio_PulldownOff(pin)       
#define Gpio_GetState(pin)          Gpio_Get((volatile uint32_t *)&PORTA.IN, (gpio_pin_t)pin)
#define Gpio_SetState(pin, stt)     Gpio_Set((volatile uint32_t *)&PORTA.OUT, (gpio_pin_t)pin, stt)
#define Gpio_SetHigh(pin)           Gpio_Set((volatile uint32_t *)&PORTA.OUT, (gpio_pin_t)pin, HIGH)
#define Gpio_SetLow(pin)            Gpio_Set((volatile uint32_t *)&PORTA.OUT, (gpio_pin_t)pin, LOW)
#define Gpio_Toggle(pin)            Gpio_Set((volatile uint32_t *)&PORTA.OUT, (gpio_pin_t)pin, TOGGLE)
#endif
#endif