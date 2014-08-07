/* 
 * \file Name: CommonFsm.h
 * Created:  Grant Zhou 08/05/2014
 * Modified: Grant Zhou 08/06/2014 22:30>
 * 
 * \brief Simple non-event triggerred FSM implementation
 * 
 * \details
 * This module provide one simple FSM implementation.
 * It is not an event triggerred FSM, only simple timeout mechnism provided.
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
#ifndef _COMMON_FSM_H
#define _COMMON_FSM_H

#include "CommonInc.h"

/**
************************************************************
*  Macro Definitions
************************************************************
*/
#define CM_FSM_ID_STR_LEN   (20)  /* Max FSM Instance Name Length */
#define CM_FSM_INST_STR_LEN (40)

#define CM_FSM_GET_CONTEXT(ent) ((void *)((U8 *)ent - ent->fsmCp->offset))
#define GET_FSM_ENT_FROM_CONTEXT(fsmCp, context) ((CmFsmEntity *)((U8 *)context + fsmCp->offset))
#define GET_CONTEXT_FROM_FSM_ENT(fsmEnt) ((void *)((U8 *)fsmEnt - fsmEnt->fsmCp->offset))

/**
************************************************************
*  Type Definitions
************************************************************
*/
typedef S16 (*CmFsmFp)(void *outputFn, void *context);

enum
{
    CM_FSM_CTRL_NORMAL = 1,
    CM_FSM_CTRL_TIMEOUT,
    CM_FSM_CTRL_MAX = CM_FSM_CTRL_TIMEOUT
};

typedef struct cmFsmEntry
{
    void *outputFn;
    U8    nextState;
} CmFsmEntry;

typedef struct cmFsmStatDesc
{
    char *stateStr; /* String State Indentifier          */
    U16  timeout;   /* State timeout, 0 means no timeout */
} CmFsmStatDesc;

typedef struct cmFsmCp CmFsmCp;

typedef struct cmFsmEntity
{
    S8        instName[CM_FSM_INST_STR_LEN]; /* FSM name string, used for logging */
    U16       lastState;   /* last FSM instance state    */
    U16       state;       /* current FSM instance state */
    TIMESTAMP timestamp;  /* FSM creation time          */
    U32       timeout;     /* Timeout for this state, 0 for infinite */
    U32       fsmCnt;      /* FSM execution count, used for logging  */
    CmFsmCp   *fsmCp;
} CmFsmEntity;

typedef struct cmFsmCp
{
    S8              fsmStr[CM_FSM_ID_STR_LEN];
    CmFsmFp         fsmFp;       /* Save the function pointer */
    U32             offset;      /* offset of entity in FSM context */
    U8              numStates;
    CmFsmStatDesc   *states;
    CmFsmEntry      *fsmMt;      /* FSM state matrix        */
    CmFsmEntity     *fsmEnt;     /* Point to the FSM entity */
} CmFsmCp;

/**
************************************************************
*  Function prototype
************************************************************
*/

S16 cmFsmCpInit (
    CmFsmCp        *fsmCp,     /* FSM control point */
    S8             *fsmStr,    /* string label */
    CmFsmFp        fsmFp,      /* Function Point */
    U16            offset,     /* offset of CmFsmEnt in the FSM instance context */
    U16            numStates, 
    CmFsmStatDesc  *states,
    CmFsmEntry     *fsmMt     /* FSM state matrix */
);

S16 cmFsmInstInit(
    CmFsmCp  *fsmCp,      /* FSM control point */
    void     *context,    /* user context for FSM instance */
    S8       *instName,   /* instance name */
    U16      initState    /* initial state for this FSM instance */
);

S16 cmFsmSetState(
    CmFsmCp  *fsmCp,      /* FSM control point */
    U16      state        /* new state */
);

S16 cmFsmDriver( CmFsmCp *fsmCp );
U32 cmFsmCheckTmr( CmFsmCp *fsmCp );

#endif
