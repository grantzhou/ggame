/* 
 * \file Name: GGameMainModel.c
 * Created:  Grant Zhou 08/05/2014
 * Modified: Grant Zhou 08/06/2014 12:39>
 * 
 * \brief Gaming System Main Model Layer
 * 
 * \details
 * This is the main medel layer of the GGame System
 * This layer will provide main data/state storage.
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
#include "CommonInc.h"
#include "SysLogging.h"
#include "GGameMainModel.h"

static PROC_INFO_t md_ProcInfo; /* model data, proc information */

S16 setProcInfo(PROC_INFO_t *proc)
{
    if(proc)
        md_ProcInfo = *proc;
    return SUCCESS;
}

S16 getProcInfo(PROC_INFO_t *proc)
{
    if(proc)
        *proc = md_ProcInfo;
    return SUCCESS;
}

void dumpProcInfo()
{
    SLOGINFO("md_ProcInfo.btnSeq=%s",md_ProcInfo.btnSeq);
    SLOGINFO("md_ProcInfo.btnUserInput=%s",md_ProcInfo.btnUserInput);
    SLOGINFO("md_ProcInfo.ledStat=%d %d %d",
             md_ProcInfo.ledStat[0],md_ProcInfo.ledStat[1],md_ProcInfo.ledStat[2]);
    SLOGINFO("md_ProcInfo.inputIndex=%d",md_ProcInfo.inputIndex);
    SLOGINFO("md_ProcInfo.procStat=%d",md_ProcInfo.procStat);
    SLOGINFO("md_ProcInfo.fsmEnt=%p",&md_ProcInfo.fsmEnt);
}
