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

char str[64];
int pressCount;

uint32_t g_ui32Flags;

// Graphics context used to show text on the CSTN display.
tContext g_sContext;

// The interrupt handler for the first timer interrupt.
void
Timer0IntHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Toggle the flag for the first timer.
    HWREGBITW(&g_ui32Flags, 0) ^= 1;

    //
    // Get and display timer value.
    //

    pressCount = TimerValueGet(TIMER5_BASE, TIMER_A);
    //TimerLoadSet(TIMER5_BASE, TIMER_A, 0);

    sprintf(str, "%d", pressCount);

    //
    // Update the interrupt status on the display.
    //
    IntMasterDisable();
    GrStringDraw(&g_sContext, (HWREGBITW(&g_ui32Flags, 0) ? "1" : "0"), -1, 68, 26, 1);
    GrStringDraw(&g_sContext, str, -1, 8, 46, 1);
    //GrStringDraw(&g_sContext, (HWREGBITW(&g_ui32Flags, 1) ? "1" : "0"), -1, 68, 36, 1);
    IntMasterEnable();
}

void
Timer1IntHandler(void) {}

int
main(void)
{
    tRectangle sRect;

    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    FPULazyStackingEnable();

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
    GrStringDrawCentered(&g_sContext, "timers", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 4, 0);

    // Initialize timer status display.
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    GrStringDraw(&g_sContext, "Timer 0:", -1, 16, 26, 0);

    //
    // Setup timers.
    //

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Enable processor interrupts.
    IntMasterEnable();

    // Configure the periodic timer.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() * 4);

    // SW5 - PM4 - timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    // pull-up
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Setup PM2 to be a timer.
    GPIOPinConfigure(GPIO_PM2_T5CCP0);
    GPIOPinTypeTimer(GPIO_PORTM_BASE, GPIO_PIN_2);

    TimerDisable(TIMER5_BASE, TIMER_A);

    // Configure timer 5 to capture button presses.
    TimerConfigure(TIMER5_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT);
    TimerControlEvent(TIMER5_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);

    // Set initial timer load.
    TimerLoadSet(TIMER5_BASE, TIMER_A, 5432);

    // 4-sec timer.
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Enable the timers.
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER5_BASE, TIMER_A);

    // Loop forever while the timers run.
    while(1) {}
}
