#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"


void
Timer0IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Zmien stan diody
    //
    int32_t val = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5); // Czytanie z PB5 (dioda)

    val = (val & 0xff) >> 5;
    val = ~val & 0x01;

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, val << 5);
}

void
Timer1IntHandler(void)
{}

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

    //
    // Wlaczanie i konfiguracja licznika
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // timer okresowy z 32bit. rej. liczący w dół
    ulPeriod = SysCtlClockGet() / 1000 * 500; // ilość “ticków” na stan diody led
    TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod - 1); // załaduj wartość do timera
    IntEnable(INT_TIMER0A); // zezwól na pracę przerwań od timera
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A); // odpal timer

    //
    // Loop forever while the timers run.
    //
    while(1) {}
}
