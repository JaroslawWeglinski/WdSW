#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"

int
main(void)
{
    // 16 MHz / 16 = 1 MHz = 0.001ms (period)
    // 500 ms / 0.001ms = 500000
    SysCtlClockSet(SYSCTL_SYSDIV_16 | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Ustawienie pinu PB5 (dioda D0) jako wyjscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);

    //
    // Loop forever while the timers run.
    //
    while(1)
    {
        int32_t val = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5); // Czytanie z PB5 (dioda)

        val = (val & 0xff) >> 5;
        val = ~val & 0x01;

        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, val << 5);
        ROM_SysCtlDelay(SysCtlClockGet() / 3 / 1000 * 500);
    }
}
