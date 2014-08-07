/*
 * \file Name: GGameMainController.c
 * Created:  Grant Zhou 08/04/2014
 * Modified: Grant Zhou 08/07/2014 01:13>
 *
 * \brief Gaming System Main Controll Layer
 *
 * \details
 *   This is the main controller layer of the GGame System.
 *   The main functions of this layer:
 *   Generate random buttons sequence combinations
 *   Read user input with timeout control and data validations
 *   Call View Layer to display result with LEDs
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
#include <stddef.h>
#include "SysLogging.h"
#include "GGameMainController.h"

/* FSM State and timeout definitions*/
CmFsmStatDesc mainFsmDesc[] =
{
    {"MAIN_ST_INIT",  0      },
    {"MAIN_ST_START", 0      },
    {"MAIN_ST_INPUT", 0 },
    {"MAIN_ST_QUIT",  0      },
};

/* FSM Matrix */
CmFsmEntry mainCtrlFsmMt[MAIN_ST_MAX+1][CM_FSM_CTRL_MAX] =
{
    /* MAIN_ST_INIT */
    {
        {clGenerateRandomSeq,     MAIN_ST_START},   /* CM_FSM_CTRL_NORMAL */
        {clGeneralTimeoutHdl,     MAIN_ST_QUIT },   /* CM_FSM_CTRL_TIMEOUT */
    },
    /* MAIN_ST_START */
    {
        {clCollectUserInputStart, MAIN_ST_INPUT},   /* CM_FSM_CTRL_NORMAL */
        {clGeneralTimeoutHdl,     MAIN_ST_QUIT },   /* CM_FSM_CTRL_TIMEOUT */
    },
    /* MAIN_ST_INPUT */
    {
        {clCollectUserInput,      MAIN_ST_INPUT},   /* CM_FSM_CTRL_NORMAL */
        {clGeneralTimeoutHdl,     MAIN_ST_QUIT },   /* CM_FSM_CTRL_TIMEOUT */
    },
    /* MAIN_ST_QUIT */
    {
        {clFsmQuit,               MAIN_ST_QUIT },   /* CM_FSM_CTRL_NORMAL */
        {clGeneralTimeoutHdl,     MAIN_ST_QUIT },   /* CM_FSM_CTRL_TIMEOUT */
    }
};

static char *BTN_ALLLOWED="abc";

int main (int argc, char *argv[])
{
    PROC_INFO_t procInfo;
    CmFsmCp     mainFsmCp;
    S16         ret = FAILURE;

    memset(&procInfo,0,sizeof(procInfo));
    SLOGINFO("Initialize Logging .. ");
    InitSystemLogging(argv[0], LOG_INFO, LOG_OUT_SYSLOG);
    getProcInfo(&procInfo);

    SLOGINFO("Initialize FSM Control BLock ..");
    ret = cmFsmCpInit(&mainFsmCp,
                      "G-FSM",
                      clMainFsmDr,
                      (U16)offsetof(PROC_INFO_t, fsmEnt),
                      MAIN_ST_MAX,
                      mainFsmDesc,
                      &mainCtrlFsmMt[0][0] /* FSM matrix */
                     );
    if (ret != SUCCESS)
    {
        SLOGERR("Failed to init FSM Control Point");
        return FAILURE;
    }

    SLOGINFO("Initialize FSM Instance ..");
    ret = cmFsmInstInit(&mainFsmCp,
                        &procInfo,
                        "MAIN",
                        MAIN_ST_INIT);
    if (ret != SUCCESS)
    {
        SLOGERR("Failed to init FSM Instance");
        return FAILURE;
    }

    /* Init the LED data */
    SLOGINFO("Intitialize LED ..");
    ret = VLED_Init(LED_POS_X_DEFAULT,
                    LED_POS_Y_DEFAULT,
                    LED_POS_INTERVAL_DEFAULT);
    if (ret != SUCCESS)
    {
        SLOGERR("Failed to init LED ");
        return FAILURE;
    }

    /* Run FSM and update LED View */
    VLED_UpdateView();
    SLOGINFO("FSM Intance started and running ..");
    while(true)
    {
        usleep(1);
        ret = cmFsmDriver(&mainFsmCp);
        if (ret == FAILURE) break;
        /* Update Model Data */
        setProcInfo(&procInfo);
        /* Update LED View  */
        VLED_UpdateView();
    }

    SLOGINFO("Guessing Game System Quit");
    return ret;
}

/**
 * Generate the random sequence
 * The sequence char is random readed from inArray
 * @param: inArray - the input allowed char array
 * @param: number - the maximum char number in the output sequence
 * @param: outArray - output char array contains the result
 * @return: SUCCESS - generated ok
 *          FAILURE - failed to generate
 */
int generateRandSeq(
    char *inArray,  // Input Allowed Char Array
    int  number,    // Maximum number to be generated
    char *outArray  // Output Sequence Array
)
{
    FILE  *fdSysRandom = NULL;
    U32   randSeed = 0;
    S32   i = 0, randIdx = 0, maxIdx  = 0;

    /** Basic ERROR/EXCEPT Handler **/
    if ( !inArray  || strlen(inArray) < 1 || number < 1 ||
         !outArray || sizeof(outArray) < number)
    {
        SLOGERR("Invalid parameters, in array %p, number %d, output %p",
                inArray,number,outArray);
        return FAILURE;
    }

    /**
     * Seed random number generator with system rand
     * to avoid getting same sequence
     */
    if (!(fdSysRandom = fopen ("/dev/urandom", "r")))
    {
      SLOGERR("Cannot open /dev/urandom!");
      return FAILURE;
    }

    /* Seed the random */
    fread(&randSeed, sizeof(randSeed), 1, fdSysRandom);
    srandom (randSeed);

    /* Generate the random sequence */
    maxIdx  = strlen(inArray);
    for (i = 0; i < number; i++) {
        /* Random index [0, sizeof(inArray)) */
        randIdx = (int) (random() % maxIdx);
        outArray[i] = inArray[randIdx];
    }

    return SUCCESS;
}

/**
 * Setting the terminal to non-blocking mode to receive the user input
 * @param: state - input non-blocking mode 
 * @param: number - the maximum char number in the output sequence
 * @param: outArray - output char array contains the result
 * @return: SUCCESS - generated ok
 *          FAILURE - failed to generate
 */
void setSysNonBlockMode(U8 state)
{
    struct termios ttystate;

    /* get current terminal state */
    tcgetattr(STDIN_FILENO, &ttystate);

    if (state==NB_ENABLE)
    {
        /* turn off canonical mode      */
        ttystate.c_lflag &= ~ICANON;

        /* minimum of number input read */
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==NB_DISABLE)
    {
        /* turn on canonical mode   */
        ttystate.c_lflag |= ICANON;
    }

    /* Set the terminal attributes */
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

/**
 * Check user input and return immediately even there
 *  is no input.
 * Used in non-blocking mode keyboard reading
 * @param: None
 * @return: number of key pressed
 *
 */
S32 keyHit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

/**
 * Wrapper function to call FSM functions
 *
 * @param: outputFn   - Function Pointer
 * @param: context    - Data Exchange Context
 * @return: None
 *
 */
S16 clMainFsmDr(void *outputFn, void *context)
{
    ((S16 (*)(PROC_INFO_t *context))(outputFn))(context);
    return (SUCCESS);
}

/**
 * Generate one randome sequence number
 *
 * @param: context - Data Exchange Context during FSM running
 * @return: SUCCESS - executed successfully
 *          FAILURE - errors happen
 *
 */
static S16 clGenerateRandomSeq(PROC_INFO_t *context)
{
    PROC_INFO_t *procInfo = context;
    S16 ret = FAILURE;

    /* Clear the original random sequence */
    ret = generateRandSeq(BTN_ALLLOWED,MAX_BTN_CNT,procInfo->btnSeq);
    if(ret == SUCCESS)
    {
        SLOGINFO("New random sequence generated:%s",procInfo->btnSeq);
    }

    return ret;
}

/**
 * Reset counter and start collecting user input
 *
 * @param: context - Data Exchange Context during FSM running
 * @return: SUCCESS - executed successfully
 *          FAILURE - errors happen
 *
 */
static S16 clCollectUserInputStart(PROC_INFO_t *context)
{
    PROC_INFO_t *procInfo = context;
    /* Reset any history user input data */
    procInfo->inputIndex = 0;
    memset(procInfo->btnUserInput, 0, sizeof(procInfo->btnUserInput));
    memset(procInfo->ledStat, 0, sizeof(procInfo->btnUserInput));
    return SUCCESS;
}

/**
 * Collecting user input
 *
 * @param: context - Data Exchange Context during FSM running
 * @return: SUCCESS - executed successfully
 *          FAILURE - errors happen
 *
 */
static S16 clCollectUserInput(PROC_INFO_t *context)
{
    PROC_INFO_t *procInfo=context;
    U32 idx  = 0;
    S32 i = 0;
    S32 ret  = FAILURE;
    S8  chrSeq = 0, chrUserInput = 0;
    
    idx = procInfo->inputIndex;
    SLOGINFO("Current idx: %d MAX: %d --- %d",
             idx, MAX_BTN_CNT,
             context->inputIndex);
    /**
     * Shift the items in the arrays
     * Array[MAX_BTN_CNT-1] will always save the latest input
     */
    if(idx > MAX_BTN_CNT-1)
    {
        /* Max loop exceeded, check if it is all green or not */
        ret = FAILURE;
        for(i=0; i<MAX_BTN_CNT; i++)
        {
            if( procInfo->ledStat[i] != LED_GREEN) break;
        }

        /* All GREEN will trigger new random sequence generation */
        if (i == MAX_BTN_CNT)
        {
            //cmFsmSetState
            cmFsmSetState(procInfo->fsmEnt.fsmCp,  MAIN_ST_INIT);
            SLOGINFO("All GREEN, FSM one batch done");
        }
        else
        {
            //
            SLOGINFO("Game not passed, retry....");
            cmFsmSetState(procInfo->fsmEnt.fsmCp,  MAIN_ST_START);
        }
        return SUCCESS;
    }
   SLOGINFO("BEFORE SHIFT: %10s %10s %10s [%d%d%d]",
             SColorInfo(procInfo->ledStat[0]),
             SColorInfo(procInfo->ledStat[1]),
             SColorInfo(procInfo->ledStat[2]),
             procInfo->btnUserInput[0],
             procInfo->btnUserInput[1],
             procInfo->btnUserInput[2]);
    /* Read one user input with 20 seconds timeout */
    ret = clReadUserInputChar(BTN_ALLLOWED,UI_TIMEOUT,
                              &(procInfo->btnUserInput[MAX_BTN_CNT-1]));
    if (ret == SUCCESS)
    {
        chrUserInput = procInfo->btnUserInput[MAX_BTN_CNT-1];

        /* Shift the value after user input */
        if(idx > 0)
        {
            /**
             * Shift the value from lower position, for example
             * 1->0, then 2->1
             */
            for(i=idx-1; i>=0; i--)
            {
                if( (MAX_BTN_CNT-2-i) >= 0 )
                {
                    procInfo->btnUserInput[MAX_BTN_CNT-2-i]
                        = procInfo->btnUserInput[MAX_BTN_CNT-1-i];
                    procInfo->ledStat[MAX_BTN_CNT-2-i]
                        = procInfo->ledStat[MAX_BTN_CNT-1-i];
                }
            }
        }

        /* Compare the result and update the color */
        chrSeq = procInfo->btnSeq[idx];

        SLOGINFO("Got %d, compare with %c(%s)",
                 chrUserInput,chrSeq,
                 procInfo->btnSeq);

        procInfo->ledStat[MAX_BTN_CNT-1] = LED_RED;

        if (chrSeq == chrUserInput)
            procInfo->ledStat[MAX_BTN_CNT-1] = LED_GREEN;
        else if (SCharIncluded(chrUserInput, procInfo->btnSeq) == SUCCESS)
        {
            /* Input char in the sequence list */
            procInfo->ledStat[MAX_BTN_CNT-1] = LED_ORANGE;
        }

        procInfo->inputIndex++;
    }
SLOGINFO("AFTER  SHIFT: %10s %10s %10s [%d%d%d]",
             SColorInfo(procInfo->ledStat[0]),
             SColorInfo(procInfo->ledStat[1]),
             SColorInfo(procInfo->ledStat[2]),
             procInfo->btnUserInput[0],
             procInfo->btnUserInput[1],
             procInfo->btnUserInput[2]);
    return SUCCESS;
}

/**
 * Collecting user input with timeout
 *
 * @param: allowedStr  allowed user input
 * @param: timeout     the maximum waiting time (seconds)
 * @param: outputChr   output user input if success
 * @return: SUCCESS - Got one user input
 *          FAILURE - error happen or time out
 *
 */
S16 clReadUserInputChar(
    S8 *allowedStr,
    U32 timeout,
    S8 *outputChr
)
{
    S32 keyCnt = 0, ret = FAILURE;
    S8  outChr;
    TIMESTAMP tsGate, tsNow;

    SGetMonotonicTime(&tsGate);
    tsGate.uiSeconds += timeout;

    setSysNonBlockMode(NB_ENABLE);
    while(!keyCnt)
    {
        usleep(1);
        keyCnt=keyHit();
        if (keyCnt != 0)
        {
            /* Read user input */
            outChr = fgetc(stdin);
            if (SCharIncluded(outChr,allowedStr) == SUCCESS)
                keyCnt = 1;
            else
                keyCnt = 0;
        }

        /* Check if the time expired */
        SGetMonotonicTime(&tsNow);
        ret = SCompareTimeStamp(&tsNow, &tsGate);
        if (ret != TIME_NOT_EXPIRED)
        {
            /* Maximum waiting time expired */
            SLOGERR("Timed out to wait user input ");
            keyCnt = 0xF;
        }
    }
    setSysNonBlockMode(NB_DISABLE);

    if(keyCnt == 1)
    {
        *outputChr=outChr;
        return SUCCESS;
    }
    return FAILURE;
}

static S16 clGeneralTimeoutHdl(PROC_INFO_t *context)
{
    SLOGERR("General timeout handler triggered");
    cmFsmSetState(context->fsmEnt.fsmCp,  MAIN_ST_QUIT);
    return SUCCESS;
}
static S16 clFsmQuit(PROC_INFO_t *context)
{
    SLOGINFO("Quitting FSM ");
    return FAILURE;
}
