#include <stdbool.h>
#include <stdint.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

int
main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Ustawienie pinu PG2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

    //GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x00); // Zgaszenie diody
    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x04); // Zapalenie diody
    while(1) {}
}
