#include <stdint.h>
#include <stdbool.h>
//#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
//#include "inc/hw_types.h"
//#include "driverlib/debug.h"
//#include "driverlib/fpu.h"
//#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
//#include "driverlib/timer.h"
//#include "driverlib/rom.h"
//#include "grlib/grlib.h"
//#include "drivers/cfal96x64x16.h"

int
main(void)
{
    // 16 MHz / 16 = 1 MHz = 0.001ms (period)
    // 500 ms / 0.001ms = 500000
    SysCtlClockSet(SYSCTL_SYSDIV_16 | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

    // Ustawienie pinu PB5 (dioda D0) jako wyjscie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);

    // Ustawienie pinu PB5 (dioda D0) jako wejscie
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);

//    char value = 0x01;

    //
    // Loop forever while the timers run.
    //
    while(1)
    {
        GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);
        int32_t val = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5); // Czytanie z PB5 (dioda)

        SysCtlDelay(SysCtlClockGet() / 3 / 1000 * 500);

        val = (val & 0xff) >> 5;
        val = ~val & 0x01;

        GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, val << 5);
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, val << 2);
        //SysCtlDelay(SysCtlClockGet() / 3 / 1000 * 500);
    }
}
