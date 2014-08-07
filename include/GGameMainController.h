/* 
 * \file Name: GGameMainController.h
 * Created:  Grant Zhou 08/05/2014
 * Modified: Grant Zhou 08/07/2014 09:39>
 * 
 * \brief Gaming System Main Controll Layer Include File
 * 
 * \details
 * Controller Layer Definitions
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
#ifndef _GGAME_MAIN_CONTROLLER_H
#define _GGAME_MAIN_CONTROLLER_H
#include "GGameMainModel.h"

/**
************************************************************
*  Macro Definitions
************************************************************
*/

/**
************************************************************
*  Type Definitions
************************************************************
*/
#define NB_ENABLE   1   /* Non-Blocking Mode Enabled  */
#define NB_DISABLE  0   /* Non-Blocking Mode Disabled */
#define UI_TIMEOUT  10  /* Maximum user input timeout */

/**
************************************************************
*  Function prototype
************************************************************
*/
int generateRandSeq(
    char *inArray,
    int  number,
    char *outArray
);

S32 keyHit();
void setSysNonBlockMode(U8 state);
S16 clReadUserInputChar(S8 *allowedStr,U32 timeout,S8 *outputChr);
static S32 clInstallSignalHandler(void);
static void clSignalHandler (int sig, siginfo_t * siginf, void *ptr);

/* FSM Control Layer Functions */
static S16 clGenerateRandomSeq(PROC_INFO_t *context);
static S16 clCollectUserInputStart(PROC_INFO_t *context);
static S16 clCollectUserInput(PROC_INFO_t *context);
static S16 clGeneralTimeoutHdl(PROC_INFO_t *context);
static S16 clFsmQuit(PROC_INFO_t *context);
S16 clMainFsmDr(void *outputFn, void *context);


#endif
