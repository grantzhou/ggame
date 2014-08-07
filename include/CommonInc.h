/* 
 * \file Name: CommonInc.h
 * Created:  Grant Zhou 08/04/2014
 * Modified: Grant Zhou 08/07/2014 10:19>
 * 
 * \brief Common Include File
 * 
 * \details
 *  This file contains all the common used definitions
 *  All the files in the system will include it
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
#ifndef _COMMON_INC_H
#define _COMMON_INC_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <time.h>
#include "SysLogging.h"

/**
************************************************************
*  Macro Definitions
************************************************************
*/
#define TIME_EQUAL       0
#define TIME_EXPIRED     1
#define TIME_NOT_EXPIRED 2
#define ABORT_DEBUG      abort()

/**
************************************************************
*  Type Definitions
************************************************************
*/
#ifdef PUBLIC
  #undef PUBLIC
  #define PUBLIC              /* default C public scope */
#else
  #define PUBLIC
#endif /* PUBLIC */

#ifdef PRIVATE
  #undef PRIVATE
  #define PRIVATE static      /* c static scope */
#else
  #define PRIVATE static      /* c static scope */
#endif /* PRIVATE */

#ifdef EXTERN
  #undef EXTERN
  #define EXTERN  extern
#else /* not EXTERN */
  #define EXTERN  extern
#endif /* EXTERN */


#ifdef ANSI
  #define CONSTANT const       /* constant     */
#else
  #define CONSTANT
#endif /* ANSI */

/* Linux Type Definitions */
#define S8    char
#define S16   short
#define S32   int
#define S64   long long int

#define U8    unsigned char
#define U16   unsigned short
#define U32   unsigned int
#define U64   unsigned long long int

#define DateTime struct tm

#ifndef FALSE
 #define FALSE   0
#endif

#ifndef TRUE
 #define TRUE    1
#endif

#ifndef true
 #define true  TRUE
 #define false FALSE
#endif

#ifndef FAILURE
 #define FAILURE   -1
#endif

#ifndef SUCCESS
 #define SUCCESS   0
#endif

#ifndef INLINE
 #define INLINE extern inline
#endif

typedef struct
{
  U32 uiSeconds;
  U32 uiMicroseconds;
}TIMESTAMP;

/**
************************************************************
*  Function prototype
************************************************************
*/
INLINE void SGetMonotonicTime(TIMESTAMP *tv)  __attribute__((always_inline));
INLINE U32 SCompareTimeStamp(TIMESTAMP *tsNow, TIMESTAMP *tsCmp)  __attribute__((always_inline));
INLINE void SAddMsToTimeStamp(TIMESTAMP *ts, U32 ms) __attribute__((always_inline));
INLINE void dumpTimeStamp(TIMESTAMP *ts)  __attribute__((always_inline));
INLINE S32 SCharIncluded(const S8 chr,const S8 *str)  __attribute__((always_inline));


/**
 * Inline function to get monotonic time
 * @param: tv - output timestamp
 * @return: None
 */
INLINE void SGetMonotonicTime(TIMESTAMP *tv)
{
    struct timespec ts;
    int retval = FAILURE;
  
    memset(tv,0,sizeof(TIMESTAMP));

    retval = clock_gettime(CLOCK_MONOTONIC, &ts);
    if(retval < 0)
    {
        retval = errno;
        SLOGERR("Failed to call clock_gettime (%s)",strerror(retval));
        return;
     }

     tv->uiSeconds      = ts.tv_sec;
     tv->uiMicroseconds = ts.tv_nsec / 1000;
     return;
}

/**
 * Inline function to compare two monotonic time
 * @param: tsCmp - the time stamp to be compared
 * @param: tsNow - the time stamp to compare
 * @return: TIME_EQUAL   - same time
 *          TIME_EXPIRED - the 
 */
INLINE U32 SCompareTimeStamp(TIMESTAMP *tsNow, TIMESTAMP *tsCmp)
{
    if(tsCmp->uiSeconds < tsNow->uiSeconds)
        return TIME_EXPIRED;
   
    if(tsCmp->uiSeconds > tsNow->uiSeconds)
        return TIME_NOT_EXPIRED;

    if(tsCmp->uiSeconds == tsNow->uiSeconds)
    {
        // SLOGINFO("%d vs %d %d vs %d",
        //         tsCmp->uiSeconds,tsNow->uiSeconds,
        //         tsCmp->uiMicroseconds, tsNow->uiMicroseconds);
        if(tsCmp->uiMicroseconds == tsNow->uiMicroseconds)
            return TIME_EQUAL;
        else if(tsCmp->uiMicroseconds < tsNow->uiMicroseconds)
            return TIME_EXPIRED;
        else if(tsCmp->uiMicroseconds > tsNow->uiMicroseconds)
            return TIME_NOT_EXPIRED;
    }   

    return TIME_EXPIRED;
}

/**
 * Inline function to add micro seconds onto the time stamp
 * @param: ts - the target time stamp
 * @param: ms - the micro seconds to add
 * @return: SUCCESS
 */
INLINE void SAddMsToTimeStamp(TIMESTAMP *ts, U32 ms)
{
    ts->uiSeconds += ((ms + 500) / 1000);    
    ts->uiMicroseconds +=  ms % 1000;
    return;
}

/**
 * Inline function to dump the timestamp contents
 * Debug use only
 * @param: ts - input time stamp
 * @return: None
 */
INLINE void dumpTimeStamp(TIMESTAMP *ts)
{
    SLOGINFO("TS DATA: %d %d", ts->uiSeconds, ts->uiMicroseconds);
}
/**
 * Inline function to check if the char in list
 * @param: chr - input char
 * @param: str - allowed char list
 * @return: SUCCESS - allowed
 *          FAILURE - not allowed
 */
INLINE S32 SCharIncluded(const S8 chr,const S8 *str)
{
    U32 i,iLen;
    iLen=strlen(str);
    for (i=0; i<iLen; i++)
    {
        if(chr == str[i]) return SUCCESS;
    }
    return FAILURE;
}

#ifdef __cplusplus
}
#endif

#endif
