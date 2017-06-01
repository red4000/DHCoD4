#pragma warning ( disable : 4530 )
#include <stdio.h>
#include <windows.h>
#include <iostream>
#pragma hdrstop

#include "hLog.h"
#include "hAPI.h"
#include "VMProtectSDK.h"

extern char pBuffer[2048];

CRITICAL_SECTION log_cs;

char logbuf[1024];

bool _Log(char *szLog, const char *data, size_t len)
{
   FILE *f     = NULL;

   fopen_s(&f, szLog, "a");

   if (!f)
   {
      fopen_s(&f, szLog, "a");
   }

   if (f)
   {
      fwrite(data, len, 1, f);
      fclose(f);
      return(true);
   }
   return(false);
}


unsigned long LogThread(void *)
{
   InitializeCriticalSection(&log_cs);
   char fpath[MAX_PATH];
   strcpy(fpath, HackPath);
   strcat(fpath, "g0dzilla_log.txt");
   while (1)
   {
      Sleep(100);
      EnterCriticalSection(&log_cs);
      size_t len = strlen(logbuf);
      if (len > 0)
      {
         if (_Log(fpath, logbuf, len))
         {
            memset(logbuf, 0, sizeof(logbuf));
         }
      }
      LeaveCriticalSection(&log_cs);
   }
   return(0);
}


#ifdef LOG
void Log(char *data)
{
      EnterCriticalSection(&log_cs);
      strcat_s(logbuf, data);
      LeaveCriticalSection(&log_cs);
}


void Logf(char *pText, ...)
{
   va_list valist;
   char logBuf[2048];

   va_start(valist, pText);
   vsprintf_s(logBuf, pText, valist);
   va_end(valist);

   Log(logBuf);
}


#endif // LOG
