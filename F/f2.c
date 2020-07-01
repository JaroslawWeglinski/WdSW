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


void
clear(tContext *sContext)
{
    GrContextForegroundSet(sContext, ClrBlue);
    tRectangle sRect;
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = DISPLAY_WIDTH - 1;
    sRect.i16YMax = DISPLAY_HEIGHT - 1;
    GrRectFill(sContext, &sRect);
}

int
main(void)
{
    CFAL96x64x16Init();
    tContext sContext;
    tRectangle sRect;
    GrContextInit(&sContext, &g_sCFAL96x64x16);

    const char text[] = "Swiat jest scena, tylko sztuka jest zle obsadzona - Oscar Wilde.";

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

    SysCtlDelay(SysCtlClockGet());

    //
    // F2
    //

    GrContextFontSet(&sContext, g_psFontCm30);

    int textWidth = GrStringWidthGet(&sContext, text, -1);
    int textHeight = GrStringHeightGet(&sContext);

    int xoff = 0;
    while(1)
    {
        clear(&sContext);

        // Wyswietl tekst
        GrContextForegroundSet(&sContext, ClrWhite);
        GrStringDraw(&sContext, text, -1, 0 - xoff, DISPLAY_HEIGHT / 2, 0);
        xoff += 2;
        if (xoff >= textWidth)
            xoff = -DISPLAY_WIDTH;

        //
        // Flush any cached drawing operations.
        //
        GrFlush(&sContext);
    }
}
