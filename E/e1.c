#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#define ABC_MASK (GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_5)
#define DEFG_MASK (GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_0 | GPIO_PIN_7)
#define TENS_DIGIT (1 << 2)
#define UNITS_DIGIT (0 << 2)

void write_to_disp(unsigned char t, unsigned char digit)
{
    // 7  6  5  4  3  2  1  0
    // _  g  f  e  d  c  b  a
    //    N7 N0 N5 N2 K5 K6 KA
    unsigned char bits[8];
    int i;

    for (i = 0; i < 8; i++)
        bits[i] = (t & (1 << i)) >> i;

    // Port K
    // _ B C A _ _ _ _
    // 7 6 5 4 3 2 1 0
    bits[0] <<= 4;  // a
    bits[1] <<= 6;  // b
    bits[2] <<= 5;  // c
    unsigned char abc = bits[0] | bits[1] | bits[2];


    // Port N
    // G _ E _ _ D _ F
    // 7 6 5 4 3 2 1 0
    bits[3] <<= 2;  // d
    bits[4] <<= 5;  // e
    bits[5] <<= 0;  // f
    bits[6] <<= 7;  // g
    unsigned char defg = bits[3] | bits[4] | bits[5] | bits[6];


    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, digit);
    GPIOPinWrite(GPIO_PORTK_BASE, ABC_MASK, abc);
    GPIOPinWrite(GPIO_PORTN_BASE, DEFG_MASK, defg);
}

int
main(void)
{
    // index -> segments
    const unsigned char ledt[] = {0x3F,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7F,0x67,0x79};

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    // Wejcie CAT
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_2);

    // Wejscia AA, AB, AC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, ABC_MASK);

    // Wejscia AD, AE, AF, AG
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, DEFG_MASK);

    // Do wyswietlenia bedzie 47

    while(1)
    {
        write_to_disp(ledt[4], TENS_DIGIT);
        SysCtlDelay(SysCtlClockGet() / 3 / 1000 * 5);

        write_to_disp(ledt[7], UNITS_DIGIT);
        SysCtlDelay(SysCtlClockGet() / 3 / 1000 * 5);
    }
}
