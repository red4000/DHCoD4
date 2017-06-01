#ifndef HLOG_H
#define HLOG_H

#include "hDefines.h"

// TODO: functions that allow changing what file to log to

unsigned long LogThread(void *);

#ifdef LOG
void Log(char *data);
void Logf(char *pText, ...);

#else
#define Log
#define Logf
#endif // LOG

#ifdef COMPILE_DEBUG
#define DEBUG_LOG    Logf
#else
#define DEBUG_LOG
#endif // COMPILE_DEBUG

#endif
