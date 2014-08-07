/* 
 * \file Name: GGameMainLEDView.h
 * Created:  Grant Zhou 08/05/2014
 * Modified: Grant Zhou 08/07/2014 00:28>
 * 
 * \brief Gaming System Main LED View Layer Include File
 * 
 * \details
 * View Layer Definitions
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
#ifndef _GGAME_MAIN_LED_VIEW_H
#define _GGAME_MAIN_LED_VIEW_H

#include "CommonInc.h"
/**
************************************************************
*  Macro Definitions
************************************************************
*/
#define MAX_LED     3

/* LED Terminal Color Setting */
#define TCOLOR_NRM  "\x1B[0m"     
#define TCOLOR_RED  "\x1B[31m"
#define TCOLOR_GRN  "\x1B[32m"
#define TCOLOR_YEL  "\x1B[33m"
#define TCOLOR_BLU  "\x1B[34m"
#define TCOLOR_MAG  "\x1B[35m"
#define TCOLOR_CYN  "\x1B[36m"
#define TCOLOR_WHT  "\x1B[37m"

#define LED_POS_X_DEFAULT         6  /* Default LED X Position                  */
#define LED_POS_Y_DEFAULT         6  /* Default LED Y Position                  */
#define LED_POS_INTERVAL_DEFAULT  1  /* Default LED interval between each other */
#define LED_POS_HEIGHT_DEFAULT    3  /* Default LED BAR Height                  */
#define LED_POS_WIDTH_DEFAULT     5  /* Default LED BAR Width                   */

#define IS_VALID_LED_COLOR(x)                   \
    (                                           \
        (x) >= LED_OFF &&                       \
        (x) < LED_COLOR_MAX                     \
    )

#define IS_VALID_LEDDRV_STAT_(x)                \
    (                                           \
        (x) > LEDDRV_ST_UNKNOWN  &&             \
        (x) < LED_COLOR_MAX                     \
    )

/**
************************************************************
*  Type Definitions
************************************************************
*/
typedef enum LED_COLOR_TAG
{
    LED_OFF     = 0,  /* LED OFF           */
    LED_GREEN      ,  /* Green color       */
    LED_ORANGE     ,  /* Orange Color      */
    LED_RED        ,  /* Red Color         */
    LED_COLOR_MAX     /* Maximum LED Color */
} LED_COLOR_t;

typedef enum LEDDRV_STAT_TAG
{
    LEDDRV_ST_UNKNOWN = 0 , /* LED Device Status Unknown */
    LEDDRV_ST_DEVICE_NA   , /* LED Device Not available  */
    LEDDRV_ST_NORMAL        /* LED Device Status Normal  */
} LEDDRV_STAT_t;

typedef struct TERM_COLOR_TAG
{
    S8 *colorStr;           /* LED Color String          */
    S8 *termColor;           /* LED Terminal Color String */ 
}TERM_COLOR_t;

/**
************************************************************
*  Function prototype
************************************************************
*/
S16  VLED_Init(U16 x, U16 y, U16 intVal);
void VLED_ResetLedAll();

/* Interface to controller to trigger the batch LED color update */
void VLED_UpdateView();

/* set one LED color */
void VLED_SetLedColor(U8 ledIndex, LED_COLOR_t color);

/* loop the ledStat[] array and set all the LEDs’ color. */
void VLED_BatchSetLedColor(LED_COLOR_t *ledColors);

/* Check LED Driver */
S16 VLED_CheckLedDriver();

/* Return color string */
S8 * SColorInfo(U8 color);

#endif
