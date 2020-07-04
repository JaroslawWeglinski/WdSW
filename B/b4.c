#include <stdbool.h>
#include <stdint.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

int
main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Ustawienie pinu PG2 jako wyjscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

    // Ustawienie pinu PM2 jako wejscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_0);

    // Ustawienie pull-up dla PM0
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x00); // Zgaszenie diody

    while(1)
    {
        // Czytanie z PM0 (przycisk SW1) i negowanie wartosci
        bool value = ~GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0) & 0x01;
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, value << 2); // Pisanie na PG2 (user LED)
    }
}
