/* 
 * \file Name: SysLogging.h
 * Created:  Grant Zhou 08/04/2014
 * Modified: Grant Zhou 08/05/2014 00:19>
 * 
 * \brief Prototypes for system common logging functions
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
#ifndef _SYS_LOGGING_H
#define _SYS_LOGGING_H
#include <sys/syslog.h>

/* System Log Output Target */
#define LOG_OUT_SYSLOG  0
#define LOG_OUT_STDOUT  1

/* System Log Types       */
#define LOG_TYPE_MIN     0
#define LOG_TYPE_COMMON  1  /* Common Logging Type */
#define LOG_TYPE_ERROR   2  /* ERROR  Logging Type */
#define LOG_TYPE_USR     3  /* USER   Logging Type */
#define LOG_TYPE_WARN    4  /* WARN   Logging Type */
#define LOG_TYPE_INFO    5  /* INFO   Logging Type */
#define LOG_TYPE_NOTICE  6  /* NOTICE Logging Type */
#define LOG_TYPE_MAX     7
#define MAX_NAME_LEN         80
#define MAX_FORMAT           256
#define SYS_LOG_BUFFER_SIZE  8192

#define VALID_LOGTYPE(x) (((x) > LOG_TYPE_MIN ) && ((x) < LOG_TYPE_MAX))

#define SLOGCRI(...)     sl_LogSysMsg(LOG_CRIT,  LOG_TYPE_COMMON,  __FILE__,__FUNCTION__,__LINE__,__VA_ARGS__ );
#define SLOGERR(...)     sl_LogSysMsg(LOG_ERR,  LOG_TYPE_ERROR,  __FILE__,__FUNCTION__,__LINE__,__VA_ARGS__ );
#define SLOGWARN(...)    sl_LogSysMsg(LOG_WARN, LOG_TYPE_WARN,   __FILE__,__FUNCTION__,__LINE__,__VA_ARGS__ );
#define SLOGNOTE(...)    sl_LogSysMsg(LOG_NOTICE,  LOG_TYPE_COMMON, __FILE__,__FUNCTION__,__LINE__,__VA_ARGS__ );
#define SLOGINFO(...)    sl_LogSysMsg(LOG_INFO, LOG_TYPE_INFO,   __FILE__,__FUNCTION__,__LINE__,__VA_ARGS__ );

void InitSystemLogging(
    const char *logProcName,     /* process name    */
    int  verbosity,    /* verbosity level */
    int  iOutput 
);

void sl_LogSysMsg
(
    int         iLevel,   //!< Min verbosity level to print at
    int         iType,    //!< Type of log msg (see SLT_xx defines)
    const char* strFile,  //!< Source file where error is generated
    const char* strFunc,  //!< Function where error is generated
    int         iLine,    //!< Line num where error is generated
    const char  * const pszFormat,   //!< Msg (format string and args)
    ...
);

#endif
