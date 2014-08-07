/*
 * \file Name: SysLogging.c
 * Created:  Grant Zhou 08/04/2014
 * Modified: Grant Zhou 08/06/2014 20:15>
 *
 * \brief System common logging functions
 *
 * \details
 *   System common logging functions contains all the logging
 * related functions. All the output will go to common system
 * log of Linux system.
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
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syslog.h>
#include <libgen.h>
#include <stdarg.h>
#include "SysLogging.h"

static char sl_ProcName[MAX_NAME_LEN] = {0};  /* System Logging Process Name */
static int  sl_iVerbosity             = 0  ;  /* System Verbosity Level      */
static int  sl_iOutput                = LOG_OUT_STDOUT;

void slogf(int severity, const char * fmt, ... );

/**
 * This function initialize the gloabal system logging
 * @param: verbosity system verbosity level, comply with linux system setting
 * @param: p_LogName process name in this program
 * @param: iOutput   LOG_OUT_SYSLOG/LOG_OUT_STDOUT
 * @return: None
 *
 */
void InitSystemLogging(
    const char *logProcName,   /* process name          */
    int  iLevel,               /* verbosity/debug level */
    int  iOutput               /* Log output            */
)
{
    /* Validate input verbosity level */
    if ( iLevel < LOG_CRIT  )  iLevel = LOG_CRIT;
    if ( iLevel > LOG_DEBUG )  iLevel = LOG_DEBUG ;

    sl_iVerbosity = iLevel;

    /* Save input process name */
    memset(sl_ProcName,0,sizeof(sl_ProcName));
    if (logProcName)
    {
        strncpy (sl_ProcName, basename(logProcName), MAX_NAME_LEN ) ;
    }
    else
    {
        strcpy (sl_ProcName, "NONE");
    }

    sl_ProcName[ MAX_NAME_LEN-1 ] = 0 ;

    /* Default output to system log */
    sl_iOutput = (iOutput == LOG_OUT_STDOUT)?LOG_OUT_STDOUT:LOG_OUT_SYSLOG;
}

/**
 * Log one message into system log.
 *
 * @param: iLevel targe print level of this message
 * @param: iType  message type, defined in LOG_TYPE_...
 * @param: strFile File name
 * @param: strFunc Function name
 * @param: iLine   File Line Number
 * @param: format  Message format
 * @param: ...     Arguments for format
 * @return: None
 *
 */
void sl_LogSysMsg
(
    int         iLevel,   //!< Min verbosity level to print at
    int         iType,    //!< Type of log msg (see SLT_xx defines)
    const char* strFile,  //!< Source file where error is generated
    const char* strFunc,  //!< Function where error is generated
    int         iLine,    //!< Line num where error is generated
    const char  * const format,   //!< Msg (format string and args)
    ...
)
{
    va_list ap;
    char    szFoo[ SYS_LOG_BUFFER_SIZE ] ;
    char    logType[MAX_NAME_LEN];
    int     iLoc ;

    /* Check input log level before print into syslog */
    if ( iLevel < LOG_CRIT )
        iLevel = LOG_CRIT;
    if ( iLevel > LOG_DEBUG )
        iLevel = LOG_DEBUG ;

    if (iLevel > sl_iVerbosity) return;

    /* Based on the input type, add log identity */
    memset(logType,0,sizeof(logType));
    switch (iType)
    {
    case LOG_TYPE_ERROR:
        strcpy(logType,"ERR ");
        break;
    case LOG_TYPE_USR:
        strcpy(logType,"USR ");
        break;
    case LOG_TYPE_WARN:
        strcpy(logType,"WARN ");
        break;
    case LOG_TYPE_INFO:
        strcpy(logType,"INFO ");
        break;
    case LOG_TYPE_COMMON:
    default:
        break;
    }

    /* Print input message into system log */
    memset(szFoo,0 , SYS_LOG_BUFFER_SIZE);
    iLoc = snprintf ( szFoo, SYS_LOG_BUFFER_SIZE-1, "%u(%u) %s[%s:%s:%d] ",
                      (unsigned int)getpid(),(unsigned int)pthread_self(),
                      logType, strFile, strFunc, iLine ) ;
    va_start ( ap, format ) ;
    vsnprintf ( szFoo+iLoc, SYS_LOG_BUFFER_SIZE-iLoc , format, ap ) ;
    va_end ( ap ) ;


    if (sl_iOutput == LOG_OUT_STDOUT)
    {
        time_t ltime;
        struct tm *Tm;
        ltime=time(NULL);
        Tm=localtime(&ltime);
        printf("[%d/%d/%d %d:%d:%d] %s %s\n",
               Tm->tm_mday,
               Tm->tm_mon+1,
               Tm->tm_year+1900,
               Tm->tm_hour,
               Tm->tm_min,
               Tm->tm_sec,
               sl_ProcName,
               szFoo);
    }
    else
        slogf(iLevel,"%s",szFoo);

}

/**
 * Linux syslog facility wrapper
 *
 * @param: iLevel targe print level of this message
 * @param: format  Message print format
 * @param: ...     Arguments for format
 * @return: None
 *
 */
void slogf(int iLevel, const char * format, ... )
{
    if (strlen(format)>MAX_FORMAT) return;
    va_list ap;
    va_start(ap,format);
	vsyslog(LOG_USER|iLevel,format,ap);
    va_end(ap);
}
