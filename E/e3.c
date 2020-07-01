#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#define ABC_MASK (GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_5)
#define DEFG_MASK (GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_0 | GPIO_PIN_7)
#define TENS_DIGIT (1 << 2)
#define UNITS_DIGIT (0 << 2)

const unsigned char obchody[7] = {0x5C,0x7C,0x58,0x74,0x5C,0x5E,0x6E};
unsigned char abc[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char defg[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void preprocess_digit(int ind)
{
    unsigned char t = obchody[ind];
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
    abc[ind] = bits[0] | bits[1] | bits[2];


    // Port N
    // G _ E _ _ D _ F
    // 7 6 5 4 3 2 1 0
    bits[3] <<= 2;  // d
    bits[4] <<= 5;  // e
    bits[5] <<= 0;  // f
    bits[6] <<= 7;  // g
    defg[ind] = bits[3] | bits[4] | bits[5] | bits[6];
}

int
main(void)
{
    // index -> segments

    int ind;
    for (ind = 0; ind < 7; ind++)
        preprocess_digit(ind);

    int i = 0;

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

    int j;
    while(1)
    {
        // SysCtlClockGet() = 1s in clock cycles
        // SysCtlDelay(SysCtlClockGet() / 3); -> sleeps for 1s
        // SysCtlDelay(SysCtlClockGet() / 3 / 200); -> sleeps for 1/200s

        //
        // 9 cycles * 2 = 18

        int persec = SysCtlClockGet() / (SysCtlClockGet() / 300 + 18);

        for (j = 0; j < persec / 6; j++)
        {
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, TENS_DIGIT);  // 3 cykle
            GPIOPinWrite(GPIO_PORTK_BASE, ABC_MASK, abc[i]);  // 3 cykle
            GPIOPinWrite(GPIO_PORTN_BASE, DEFG_MASK, defg[i]);  // 3 cykle -> 9 cykli w sumie
            SysCtlDelay(SysCtlClockGet() / 3 / 200);

            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, UNITS_DIGIT);
            GPIOPinWrite(GPIO_PORTK_BASE, ABC_MASK, abc[(i+1)%7]);
            GPIOPinWrite(GPIO_PORTN_BASE, DEFG_MASK, defg[(i+1)%7]);
            SysCtlDelay(SysCtlClockGet() / 3 / 200);
        }
        i = (i+1) % 7;
    }
}
