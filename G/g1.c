#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "driverlib/adc.h"

uint32_t g_ui32Flags;

const unsigned int timerStartValue = 2048;
volatile unsigned int pressCount;
volatile unsigned int displayFlag = 0;
char str[64];

// Graphics context used to show text on the CSTN display.
tContext g_sContext;

void
Timer0IntHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Get and reload timer value, then set display flag.
    //
    pressCount = timerStartValue - TimerValueGet(TIMER5_BASE, TIMER_A);
    TimerLoadSet(TIMER5_BASE, TIMER_A, timerStartValue);
    displayFlag = 1;
}

void
Timer1IntHandler(void) {}

int
main(void)
{
    tRectangle sRect;

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //
    // Setup display.
    //
    CFAL96x64x16Init();

    // Initialize the graphics context and find the middle X coordinate.
    GrContextInit(&g_sContext, &g_sCFAL96x64x16);

    // Fill the top part of the screen with blue to create the banner.
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&g_sContext) - 1;
    sRect.i16YMax = 9;
    GrContextForegroundSet(&g_sContext, ClrDarkBlue);
    GrRectFill(&g_sContext, &sRect);

    // Change foreground for white text.
    GrContextForegroundSet(&g_sContext, ClrWhite);

    // Put the application name in the middle of the banner.
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    GrStringDrawCentered(&g_sContext, "Segment G", -1, GrContextDpyWidthGet(&g_sContext) / 2, 4, 0);

    // Initialize timer status display.
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    GrStringDraw(&g_sContext, "Timer 5:", -1, 16, 26, 0);


    //
    // Setup timers.
    //

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);

    // Setup PM2 to be a timer.
    GPIOPinTypeTimer(GPIO_PORTM_BASE, GPIO_PIN_2);
    GPIOPinConfigure(GPIO_PM2_T5CCP0);
    // pull-up
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configure timer 5 to capture button presses.
    TimerConfigure(TIMER5_BASE, TIMER_CFG_A_CAP_COUNT | TIMER_CFG_SPLIT_PAIR);
    TimerControlEvent(TIMER5_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
    TimerLoadSet(TIMER5_BASE, TIMER_A, timerStartValue);

    // Configure the periodic timer.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() * 4 - 1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Enable processor interrupts.
    IntMasterEnable();

    // Enable the timers.
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER5_BASE, TIMER_A);

    // Loop forever while the timers run.
    while(1)
    {
        // Display only when prompted
        if (displayFlag == 1)
        {
            displayFlag = 0;
            sprintf(str, "%d", pressCount);
            GrStringDraw(&g_sContext, "   ", -1, 68, 26, 1);
            GrStringDraw(&g_sContext, str, -1, 68, 26, 1);
        }
    }
}
