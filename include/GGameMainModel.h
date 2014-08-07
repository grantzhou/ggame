/* 
 * \file Name: GGameMainModel.h
 * Created:  Grant Zhou 08/05/2014
 * Modified: Grant Zhou 08/07/2014 00:21>
 * 
 * \brief  Gaming System Main Model Layer
 * 
 * \details
 * Main Model Layer will provide system the data storage
 * LED Status, Process Infomation and FSM data are all maintains here.
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
#ifndef _GGAME_MAIN_MODEL_H
#define _GGAME_MAIN_MODEL_H

#include "CommonFsm.h"
#include "GGameMainLEDView.h"
/**
************************************************************
*  Macro Definitions
************************************************************
*/
#define MAX_BTN_CNT 3

/**
************************************************************
*  Type Definitions
************************************************************
*/
typedef enum PROC_STAT_TAG
{
    MAIN_ST_INIT = 0,    /* Init State                       */
    MAIN_ST_START,       /* FSM started                      */
    MAIN_ST_INPUT,       /* Wait User Input and set the LED  */
    MAIN_ST_QUIT,        /* Quit the application             */
    MAIN_ST_MAX=MAIN_ST_QUIT /* Maximum FSM               */
} PROC_STAT_t;

typedef struct PROC_INFO_TAG
{ 
    S8          btnSeq[MAX_BTN_CNT+1];       /* Save the target sequence       */
    S8          btnUserInput[MAX_BTN_CNT+1]; /* Save the user input btns       */
    LED_COLOR_t ledStat[MAX_BTN_CNT+1];      /* Save the LED state             */
    S32         inputIndex;                  /* Current Input index            */
    PROC_STAT_t procStat;                    /* Save the current process state */
    time_t      procStatTimeOut;             /* State Time out                 */
    CmFsmEntity fsmEnt;                      /* FSM Control Point              */
} PROC_INFO_t;

/**
************************************************************
*  Function prototype
************************************************************
*/

S16 setProcInfo(PROC_INFO_t *proc);
S16 getProcInfo(PROC_INFO_t *proc);
void dumpProcInfo();

#endif
