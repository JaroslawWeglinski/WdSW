//*****************************************************************************
//
// hello.c - Simple hello world example.
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
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"
#include "utils/uartstdio.h"
#include "driverlib/gpio.h"

#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 96
int
main(void)
{
    tContext sContext;
    tRectangle sRect;

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    // Initialize the display driver.
    CFAL96x64x16Init();

    // Initialize the graphics context.
    GrContextInit(&sContext, &g_sCFAL96x64x16);

    GrContextFontSet(&sContext, g_psFontCm12);

    int strWidth = GrStringWidthGet(&sContext, "obchody", -1);
    int strHeight = GrStringHeightGet(&sContext);

    //
    // Narysuj fioletowy prostokat
    //
    sRect.i16XMin = 0;
    sRect.i16YMin = DISPLAY_HEIGHT - strHeight;
    sRect.i16XMax = strWidth + 2;
    sRect.i16YMax = DISPLAY_HEIGHT - 1;
    GrContextForegroundSet(&sContext, ClrPurple);
    GrRectDraw(&sContext, &sRect);


    // Wyswietl `obchody`
    GrContextForegroundSet(&sContext, ClrYellow);
    GrStringDraw(&sContext, "obchody", -1, 2, DISPLAY_HEIGHT - strHeight, 0);

    //
    // Flush any cached drawing operations.
    //
    GrFlush(&sContext);

    //
    // We are finished. Hang around doing nothing.
    //
    while(1)
    {
    }
}
