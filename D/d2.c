#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

#define PWM_FREQUENCY 1000
unsigned long zmo=0;
unsigned int ulLoad;
unsigned long ulClock;
unsigned long ulPWMClock;

void
Timer0IntHandler(void)
{
    // Clear the timer interrupt.
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    int32_t val = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5); // Czytanie z PB5 (dioda)

    val = (val & 0xff) >> 5;
    val = ~val & 0x01;

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, val << 5);
}

void
Timer1IntHandler(void)
{
    // Clear the timer interrupt.
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    if (zmo > 32) zmo=0;
    ulLoad = zmo*((ulPWMClock / PWM_FREQUENCY) - 1)/100;
    zmo += 4;

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4, ulLoad);
}

int
main(void)
{
    long ulPeriod;
    // 16 MHz / 16 = 1 MHz = 0.001ms (period)
    // 500 ms / 0.001ms = 500000
    SysCtlClockSet(SYSCTL_SYSDIV_16 | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Ustawienie pinu PB5 (dioda D0) jako wyjscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);

    // Ustawienie pinu PN3 (dioda D2) jako wyjscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3 | GPIO_PIN_4);

    //
    // Wlaczanie i konfiguracja liczników
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // timer okresowy z 32bit. rej. liczący w dół
    ulPeriod = SysCtlClockGet() / 2; // ilosc “tickow” na stan diody led
    TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod - 1); // załaduj wartość do timera

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC); // timer okresowy z 32bit. rej. liczący w dół
    ulPeriod = SysCtlClockGet(); // ilosc “tickow” na stan diody led
    TimerLoadSet(TIMER1_BASE, TIMER_A, ulPeriod - 1); // załaduj wartość do timera


    IntMasterEnable(); // w module timera i NVIC

    IntEnable(INT_TIMER0A); // zezwól na pracę przerwań od timera
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A); // odpal timer

    IntEnable(INT_TIMER1A); // zezwól na pracę przerwań od timera
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE, TIMER_A); // odpal timer

    SysCtlPWMClockSet(SYSCTL_PWMDIV_2); // clock divider for the PWM , this gives 500 kHz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // enable the PWM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // enable the PWM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);  // enable the N Port

    // Połacz  GPION_PIN_3 i GPION_PIN_4 do PWM
    GPIOPinTypePWM(GPIO_PORTN_BASE, GPIO_PIN_3);
    GPIOPinTypePWM(GPIO_PORTN_BASE, GPIO_PIN_4);
    GPIOPinConfigure(GPIO_PN3_M0PWM7);
    GPIOPinConfigure(GPIO_PN4_M1PWM4);


    ulPWMClock = SysCtlClockGet(); //divide PWMs clock by 16

    ulLoad = (ulPWMClock / PWM_FREQUENCY) - 1;

    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, ulLoad);
    PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_3);

    // Ustawianie szerokosci impulsu na 24%
    ulLoad = 24*((ulPWMClock / PWM_FREQUENCY) - 1)/100;
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, ulLoad);

    //
    // Dioda D3
    //

    ulLoad = (ulPWMClock / PWM_FREQUENCY) - 1;

    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ulLoad);
    PWMOutputState(PWM1_BASE, PWM_OUT_4_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);

    //
    // Loop forever while the timers run.
    //
    while(1) {}
}
