/* 
 * \file Name: CommonFsm.c
 * Created:  Grant Zhou 08/05/2014
 * Modified: Grant Zhou 08/07/2014 00:42>
 * 
 * \brief 
 * 
 * \details
 * This module provide one simple FSM implementation.
 * It is not an event triggerred FSM, state was modified in application logic.
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
#include "CommonFsm.h"
#include "SysLogging.h"
#include "CommonInc.h"

/**
 * Initialize a common FSM Control Point
 *
 * @param: fsmCp     FSM Control Point
 * @param: fsmStr    FSM CP String Label
 * @param: fsmFp     Funcion to call the output function from fsmMt[][] 
 * @param: offset    Context Offset of CmFsmEntity in the FSM instance context
 * @param: numStates Maximum States of the FSM
 * @param: states    States Definitions
 * @param: fsmMt     FSM state matrix
 * @return: SUCCESS      success
 *          FAILURE  failed
 *
 */
S16 cmFsmCpInit (
    CmFsmCp        *fsmCp,   /* FSM control point */
    S8             *fsmStr,  /* String Label */
    CmFsmFp        fsmFp,    /* Function to call the output function from fsmMt[][] */
    U16            offset,   /* Context Offset of CmFsmEntity in the FSM instance context */
    U16            numStates,/* Maximum States of the FSM */
    CmFsmStatDesc  *states,  /* States Definitions */
    CmFsmEntry     *fsmMt    /* FSM state matrix */
)
{
    if (!fsmCp || !fsmMt || !fsmStr || !numStates)
    {
        SLOGERR("Invalid parameters, fsmCp:%p, fsmMt:%p, numStates:%d",
                fsmCp, fsmMt, numStates);
        return FAILURE;
    }

    strncpy(fsmCp->fsmStr, fsmStr, CM_FSM_ID_STR_LEN);
    fsmCp->fsmStr[CM_FSM_ID_STR_LEN - 1] = '\0';

    fsmCp->offset       = offset;
    fsmCp->numStates    = numStates;
    fsmCp->states       = states;
    fsmCp->fsmMt        = fsmMt;
    fsmCp->fsmFp        = fsmFp;

    return (SUCCESS);
}

/**
 * Initialize a common FSM Instance
 *
 * @param: fsmCp     FSM Control Point
 * @param: context   context User context for FSM instance
 * @param: instaName FSM instance name
 * @return: SUCCESS      success
 *          FAILURE  failed
 *
 */
S16 cmFsmInstInit(
    CmFsmCp  *fsmCp,      /* FSM control point */
    void     *context,    /* user context for FSM instance */
    S8       *instName,   /* instance name */
    U16      initState)
{
    CmFsmEntity *fsmEnt;

    if (!fsmCp || !context)
    {
        SLOGERR("Invalid Parameter, fsmCp:%p, context:%p\n",
                fsmCp, context);
        return (FAILURE);
    }

    fsmEnt=GET_FSM_ENT_FROM_CONTEXT(fsmCp, context);

    memset((U8 *)fsmEnt, 0, sizeof(CmFsmEntity));

    fsmEnt->fsmCp  = fsmCp;
    fsmCp->fsmEnt  = fsmEnt;

    /* Set Init State */
    if (initState >= fsmCp->numStates)
    {
        SLOGERR("Invalid Initial State:%d, Max States:%d\n",
                 initState, fsmCp->numStates);
        return (FAILURE);
    }

    fsmEnt->state = initState;
    snprintf(fsmEnt->instName, CM_FSM_INST_STR_LEN,
             "%s-%s", fsmCp->fsmStr, instName);
    fsmEnt->instName[CM_FSM_INST_STR_LEN-1] = 0;
    fsmEnt->timeout = fsmCp->states[initState].timeout;
    SGetMonotonicTime(&fsmEnt->timestamp);
    SAddMsToTimeStamp(&fsmEnt->timestamp, fsmEnt->timeout);

    return SUCCESS;
}

/**
 * FSM Driver to run a FSM instance
 *
 * @param: fsmCp     FSM Control Point
 * @return: SUCCESS      success
 *          FAILURE  failed
 *
 */
S16 cmFsmDriver( CmFsmCp *fsmCp )
{
    S16 ret = FAILURE;
    CmFsmEntry *fsmRow;
    U16 state;
    void *context;
    CmFsmEntity *fsmEnt = fsmCp->fsmEnt;
    
    fsmRow = fsmCp->fsmMt + fsmEnt->state*CM_FSM_CTRL_MAX;
    state = fsmEnt->state;

    /* Check if the instance FSM instance timeout */
    ret = cmFsmCheckTmr(fsmCp);
    if (ret != TIME_NOT_EXPIRED)
    {
        /* FSM State Timed out */
        fsmRow ++; // point to next timeout control function point
    }

    if ( fsmRow->nextState <= fsmCp->numStates )
    {
        fsmEnt->lastState = fsmEnt->state;
        fsmEnt->state    = fsmRow->nextState;
        fsmEnt->timeout = fsmCp->states[fsmRow->nextState].timeout;
        SGetMonotonicTime(&fsmEnt->timestamp);
        SAddMsToTimeStamp(&fsmEnt->timestamp, fsmEnt->timeout);

        SLOGINFO("FSM %s, STAT %s-->%s timeout %d", fsmEnt->instName, 
                 fsmCp->states[fsmEnt->lastState].stateStr, 
                 fsmCp->states[fsmEnt->state].stateStr, fsmEnt->timeout);
    }
    else
    {
        /* the output function will set the new state if required */
        SLOGINFO("FSM %s, STAT %s\n", fsmEnt->instName,
                 fsmCp->states[fsmEnt->state].stateStr);
    }

    if (fsmRow->outputFn)
    {
        context = CM_FSM_GET_CONTEXT(fsmEnt);
        ret = fsmCp->fsmFp(fsmRow->outputFn, context);
        if (ret != SUCCESS)
        {
            SLOGERR("Output Function in FSM return failure");
        }
    }
    else
    {
       SLOGERR("CM_FSM:fsmRow does not have an output func");
       ABORT_DEBUG;
    }

    if (fsmEnt->state >= fsmCp->numStates)
    {
        SLOGERR(" Max States:%d reached!", fsmCp->numStates);
        return (FAILURE);
    }

    return SUCCESS;
}

/**
 * Check if the state timed out
 *
 * @param: fsmCp     FSM Control Point
 * @return: TIME_EXPIRED  state timed out
 *          others        state not timed out
 *
 */
U32 cmFsmCheckTmr( CmFsmCp *fsmCp )
{
    U32 ret = TIME_NOT_EXPIRED;
    TIMESTAMP ts;

    if (fsmCp->states[fsmCp->fsmEnt->state].timeout > 0)
    {
        SGetMonotonicTime(&ts);
        ret = SCompareTimeStamp(&ts, &(fsmCp->fsmEnt->timestamp));
    }
    return ret;
}

/**
 * Set state for a FSM instance
 *
 * @param: fsmCp     FSM Control Point
 * @param: state     State to be returned
 * @return: SUCCESS      successful
 *          FAILURE  failed
 *
 */
S16 cmFsmSetState(
    CmFsmCp  *fsmCp,      /* FSM control point */
    U16      state        /* new state */
)
{
    CmFsmEntity *fsmEnt;

    if (!fsmCp )
    {
        SLOGERR("Invalid fsmCp:%p ",fsmCp);
        return (FAILURE);
    }
    
    fsmEnt = fsmCp->fsmEnt;

    if (state > fsmCp->numStates)
    {
       SLOGERR("Invalid state:%d, numStates:%d", state, fsmCp->numStates);
       return (FAILURE);
    }

    /* Setup new timeout for previous state */
    fsmEnt->timeout   = fsmCp->states[state].timeout;
    fsmEnt->lastState = fsmEnt->state;
    fsmEnt->state     = state;
    SGetMonotonicTime(&fsmEnt->timestamp);
    SAddMsToTimeStamp(&fsmEnt->timestamp, fsmEnt->timeout);

    SLOGINFO("%s:SetState:%s-->%s, timeout: %d\n",
             fsmEnt->instName, 
             fsmCp->states[fsmEnt->lastState].stateStr, 
             fsmCp->states[fsmEnt->state].stateStr,
             fsmCp->states[state].timeout);
    
    return (SUCCESS);
} /* cmFsmSetState() */


/**
 * Returns state for a FSM instance
 *
 * @param: fsmCp     FSM Control Point
 * @param: context   Context for data exchange
 * @param: state     State to be returned
 * @param: lastState Last State to be returned
 * @return: SUCCESS      successful
 *          FAILURE  failed
 *
 */
S16 cmFsmGetState(
    CmFsmCp  *fsmCp,      /* FSM control point */
    void     *context,    /* user context for FSM instance */
    U16      *state,      /* state to be returned */
    U16      *lastState   /* last state to be returned */
)
{
    CmFsmEntity *fsmEnt;

    if (!fsmCp || !context)
    {
        SLOGERR("Invalid fsmCp:%p, context:%p ",
                fsmCp, context);
        return (FAILURE);
    }
    
    fsmEnt = fsmCp->fsmEnt;    
    *state     = fsmEnt->state;
    *lastState = fsmEnt->lastState;
    return (SUCCESS);
} /* cmFsmGetState() */
