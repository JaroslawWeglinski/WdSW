#include <stdbool.h>
#include <stdint.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

int
main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Ustawienie pinu PG2 jako wyjscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

    // Ustawienie pinu PM2 jako wejscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_2);

    // Ustawienie pull-up dla PM2
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x00); // Zgaszenie diody

    while(1)
    {
        int32_t value = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_2); // Czytanie z PM2 (przycisk)
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, value & 0xff); // Pisanie na PG2 (user LED)
    }
}
