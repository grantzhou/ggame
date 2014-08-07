/*
 * \file Name: GGameMainLEDView.c
 * Created:  Grant Zhou 08/04/2014
 * Modified: Grant Zhou 08/07/2014 01:12>
 *
 * \brief Gaming System Main LED View Layer
 *
 * \details
 *   This is the main LED View Layer of the GGame System.
 *   The main functions of this layer:
 *   Control LEDs to display different colors
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * Had you not received a copy of the GNU General Public License yet, write
 * to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include "SysLogging.h"
#include "GGameMainLEDView.h"
#include "GGameMainModel.h"

/**
 * Static member variables with initial value
 */
static U32 VLED_X        = LED_POS_X_DEFAULT;        /* LED X Position                  */
static U32 VLED_Y        = LED_POS_Y_DEFAULT;        /* LED Y Position                  */
static U32 VLED_INTERVAL = LED_POS_INTERVAL_DEFAULT; /* LED interval between each other */
static U32 VLED_HEIGHT   = LED_POS_HEIGHT_DEFAULT;   /* LED BAR Height                  */
static U32 VLED_WIDTH    = LED_POS_WIDTH_DEFAULT;    /* LED BAR Width                   */

static const TERM_COLOR_t TERMCOLORS[LED_COLOR_MAX+1]=
{
    {"LED_OFF      ",  TCOLOR_WHT},
    {"LED_GREEN    ",  TCOLOR_GRN},
    {"LED_ORANGE   ",  TCOLOR_YEL},
    {"LED_RED      ",  TCOLOR_RED},
    {"LED_COLOR_MAX",  TCOLOR_NRM}
};

void printHelp();

/**
 * VLED Layer Reset terminal
 * @param: x - LED x position
 * @param: y - LED y position
 * @return: None
 */
void VLED_ResetLedAll()
{
    printf("%c[2J",27);
}

/**
 * Read data from model and display
 * @param: x - LED x position
 * @param: y - LED y position
 * @return: None
 */
void VLED_UpdateView()
{
    PROC_INFO_t data;
    memset(&data,0,sizeof(data));
    getProcInfo(&data);
    VLED_BatchSetLedColor(data.ledStat);
    printHelp();
}

/**
 * VLED Layer Set one LED color
 * TODO: Handle different terminal type
 * @param: x - LED x position
 * @param: y - LED y position
 * @return: None
 */
void VLED_SetLedColor(U8 ledIndex, LED_COLOR_t color)
{
    int i,j;
    for (i=0; i<VLED_HEIGHT; i++)
    {
        for(j=0; j<VLED_WIDTH; j++)
        {
            if (color == LED_OFF)
            {
                printf("%s\033[%d;%dH ",
                       TERMCOLORS[color].termColor,
                       VLED_X+i,
                       ledIndex*(VLED_WIDTH+VLED_INTERVAL)+VLED_Y+j);
            }
            else
            {
                printf("%s\033[%d;%dH\u2589%s",
                       TERMCOLORS[color].termColor,
                       VLED_X+i,
                       ledIndex*(VLED_WIDTH+VLED_INTERVAL)+VLED_Y+j,
                       TCOLOR_NRM);
            }
        } // End of loop LED Width
    } // End of loop LED height

    /* Print bar numbers */
    printf("%s\033[%d;%dH%d%s\n\n", TERMCOLORS[color].termColor, VLED_X+i,
           ledIndex*(VLED_WIDTH+VLED_INTERVAL)+VLED_Y+VLED_WIDTH/2,
           ledIndex+1,TCOLOR_NRM);
}

/**
 * loop the ledColors[] array and set all the LEDs’ color
 * @param: x - LED x position
 * @param: y - LED y position
 * @return: None
 */
void VLED_BatchSetLedColor(LED_COLOR_t *ledColors)
{
    int i;
    if(!ledColors) return;

    VLED_ResetLedAll();
    for (i=0;i<MAX_LED;i++)
    {
        if(IS_VALID_LED_COLOR(ledColors[i]))
            VLED_SetLedColor(i,ledColors[i]);
    }
}

/**
 * Check LED Driver Stat
 * Simulate ioctrl operations
 * @return: SUCCESS
 */
S16 VLED_CheckLedDriver()
{
    SLOGINFO("LED Driver Checking OK");
    return SUCCESS;
}

/**
 * VLED Layer Init Part
 * @param: x - LED x position
 * @param: y - LED y position
 * @return: SUCCESS - successed
 *          FAILURE - Failed to init
 */
S16  VLED_Init(U16 x, U16 y, U16 intVal)
{
    // Setup display data
    VLED_X=x;
    VLED_Y=y;
    VLED_INTERVAL=intVal;

    return VLED_CheckLedDriver();
}

/**
 * Return the string of the LED color
 * @param: color LED color enum value
 * @return: NULL - if input error
 *          Pointer to the string if the enum found
 */
S8 * SColorInfo(U8 color)
{
    if (IS_VALID_LED_COLOR(color))
        return TERMCOLORS[color].colorStr;
    return NULL;
}

/**
 * Print Help information on the screen
 * @param: None
 * @return: None
 */
void printHelp()
{
    printf("%s\033[%d;%dH \n",
           TCOLOR_NRM,
           VLED_X + VLED_HEIGHT + 3,
           VLED_Y);

    printf("===============================================\n"
           "Guessing System Help: \n"
           "Please guess the sequence of the a,b,c button combination (e.g. bac, ccb, aaa)\n"
           "Hints:\n"
           " LED 3 will always represent the most recent button event \n"
           " LED 2 the one before that \n"
           " LED 1 the one before that \n\n"
           " Red  - indicates that the button pressed was wrong for this position,\n"
            "       and does not appear in a different position.\n"
           " Orange - indicates that the button pressed was wrong for this position,\n"
           "        but it does appear in a different position.\n"
           " Green  - indicates that the button pressed was correct for this position.\n");

    /* Return position */
    printf("%s\033[%d;%dH\n",
           TCOLOR_NRM,
           VLED_X + VLED_HEIGHT + 1,
           VLED_Y);
}
