//*****************************************************************************
//
// timers.c - Timers example.
//
// Copyright (c) 2011-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.0.1.11577 of the DK-TM4C123G Firmware Package.
//
//*****************************************************************************

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


#include"inc/hw_types.h"
#include"inc/hw_memmap.h"
#include"driverlib/fpu.h"
#include"driverlib/sysctl.h"
#include"driverlib/rom.h"
#include"driverlib/pin_map.h"
#include"grlib/grlib.h"
#include"drivers/cfal96x64x16.h"
#include"utils/uartstdio.h"
#include"driverlib/gpio.h"
#include"utils/ustdlib.h"
#include"driverlib/adc.h"
#include"inc/hw_ints.h"
#include"driverlib/interrupt.h"
#include"driverlib/timer.h"
#include"driverlib/pwm.h"
#include"driverlib/debug.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

char str[64];

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Timer (timers)</h1>
//!
//! This example application demonstrates the use of the timers to generate
//! periodic interrupts.  One timer is set up to interrupt once per second and
//! the other to interrupt twice per second; each interrupt handler will toggle
//! its own indicator on the display.
//
//*****************************************************************************

//*****************************************************************************
//
// Flags that contain the current value of the interrupt indicator as displayed
// on the CSTN display.
//
//*****************************************************************************
uint32_t g_ui32Flags;

//*****************************************************************************
//
// Graphics context used to show text on the CSTN display.
//
//*****************************************************************************
tContext g_sContext;

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

//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************
void
Timer0IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Toggle the flag for the first timer.
    //
    HWREGBITW(&g_ui32Flags, 0) ^= 1;

    //
    // Sprawdz stan licznika wcisniec
    //


    //
    // Update the interrupt status on the display.
    //
    IntMasterDisable();
    GrStringDraw(&g_sContext, (HWREGBITW(&g_ui32Flags, 0) ? "1" : "0"), -1, 68, 26, 1);
    GrStringDraw(&g_sContext, str, -1, 8, 46, 1);
    //GrStringDraw(&g_sContext, (HWREGBITW(&g_ui32Flags, 1) ? "1" : "0"), -1, 68, 36, 1);
    IntMasterEnable();
}

//*****************************************************************************
//
// The interrupt handler for the second timer interrupt.
//
//*****************************************************************************
void
Timer1IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Toggle the flag for the second timer.
    //
    HWREGBITW(&g_ui32Flags, 1) ^= 1;

    //
    // Update the interrupt status on the display.
    //
    IntMasterDisable();
    GrStringDraw(&g_sContext, (HWREGBITW(&g_ui32Flags, 1) ? "1" : "0"), -1, 68, 36, 1);
    IntMasterEnable();
}

//*****************************************************************************
//
// This example application demonstrates the use of the timers to generate
// periodic interrupts.
//
//*****************************************************************************
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
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Initialize the display driver.
    //
    CFAL96x64x16Init();

    //
    // Initialize the graphics context and find the middle X coordinate.
    //
    GrContextInit(&g_sContext, &g_sCFAL96x64x16);

    //
    // Fill the top part of the screen with blue to create the banner.
    //
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&g_sContext) - 1;
    sRect.i16YMax = 9;
    GrContextForegroundSet(&g_sContext, ClrDarkBlue);
    GrRectFill(&g_sContext, &sRect);

    //
    // Change foreground for white text.
    //
    GrContextForegroundSet(&g_sContext, ClrWhite);

    //
    // Put the application name in the middle of the banner.
    //
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    GrStringDrawCentered(&g_sContext, "timers", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 4, 0);

    //
    // Initialize timer status display.
    //
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    GrStringDraw(&g_sContext, "Timer 0:", -1, 16, 26, 0);
    GrStringDraw(&g_sContext, "Timer 1:", -1, 16, 36, 0);

    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Configure the two 32-bit periodic timers.
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() * 4);
    //TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 2);

    //
    // Setup the interrupts for the timer timeouts.
    //

    //GPIO_PF2_T1CCP0
    // SW5 - PM4 - timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_2);
    //GPIODirModeSet(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_DIR_MODE_HW);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // pull-up


    GPIOPinTypeTimer(GPIO_PORTM_BASE, GPIO_PIN_2);
    GPIOPinConfigure(GPIO_PM2_T5CCP0);

    TimerDisable(TIMER5_BASE, TIMER_A);

    TimerConfigure(TIMER5_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP);
    TimerControlEvent(TIMER5_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);

    TimerLoadSet(TIMER5_BASE, TIMER_A, 3);




    IntEnable(INT_TIMER0A);
    //IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Enable the timers.
    //
    TimerEnable(TIMER0_BASE, TIMER_A);
    //TimerEnable(TIMER1_BASE, TIMER_A);
    TimerEnable(TIMER5_BASE, TIMER_A);

    //
    // Loop forever while the timers run.
    //
    while(1)
    {
        int pressCount = TimerValueGet(TIMER5_BASE, TIMER_A);
        //TimerLoadSet(TIMER5_BASE, TIMER_A, 0);

        sprintf(str, "%d", pressCount);
        SysCtlDelay(SysCtlClockGet() / 4);
    }
}
