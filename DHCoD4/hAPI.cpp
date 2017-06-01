#pragma warning ( disable : 4530 )
#include <windows.h>
#include <string>
#include <vector>
#include <MMSystem.h>
#include <tlhelp32.h>
#pragma hdrstop

#pragma comment (lib, "WinMM.lib")

#include "hAPI.h"
#include "hCrypt.h"
#include "hLog.h"
#include "hMath.h"
#include "hClasses.h"

typedef void (WINAPI * tGetNativeSystemInfo)(LPSYSTEM_INFO);

char rndclr[256];


bool DataCompare(const BYTE *pData, const BYTE *bMask, const char *szMask)
{
   for ( ; *szMask; ++szMask, ++pData, ++bMask)
   {
      if ((*szMask == 'x') && (*pData != *bMask))
      {
         return(false);
      }
   }

   return((*szMask) == NULL);
}


MFBEG(ADDRESS_PROC, FindPattern(ADDRESS_PROC dwdwAdd, ADDRESS_PROC dwLen, BYTE * bMask, char *szMask, BOOL wipeMaskStr));
ADDRESS_PROC FindPattern(ADDRESS_PROC dwdwAdd, ADDRESS_PROC dwLen, BYTE *bMask, char *szMask, BOOL wipeMaskStr)
{
   __try
   {
      ADDRESS_PROC end = dwdwAdd+dwLen,i;
      for (i = dwdwAdd; i < end; i++)
      {
         if (DataCompare((BYTE *)i, bMask, szMask))
         {
            break;
         }
      }

      size_t s = strlen(szMask) + 1;
      if(wipeMaskStr)
      {
         FORCE_RANDOMIZE(szMask, s);
      }
      FORCE_RANDOMIZE(bMask, s-1);
      return(i);
   }
   __except(1)
   {
     return(NULL);
   }

   return(NULL);
}


MFEND(ADDRESS_PROC, FindPattern(ADDRESS_PROC dwdwAdd, ADDRESS_PROC dwLen, BYTE * bMask, char *szMask, BOOL wipeMaskStr));


int randomInt(int min, int max) // TODO: just use random, it is the same thing
{
   return(min + (hrand() % (max - min + 1)));
}


char *randomString(int length)
{
   char* str = (char*)GlobalAlloc(GMEM_FIXED, length + 2);
   if(!str)
      return NULL;
   char chars[] = {"abcdefhijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ_"};
   for(int i = 0; i<=length; i++)
      str[i] = chars[hrand()%sizeof(chars)];
   str[length+1] = '\0';
   return str;
}


char *RandomizeColors(char *str)
{
   strcpy_s(rndclr, str);
   char *ostr = rndclr;
   char *pwr = strstr(ostr, "^");

   if (!pwr)
   {
      return(ostr);
   }

   while (1)
   {
      ++pwr;

      if ((pwr[0] >= '0') && (pwr[0] <= '9'))
      {
         DWORD dwOld;
         VirtualProtect(pwr, 1, PAGE_READWRITE, &dwOld);
         pwr[0] = '1' + (unsigned char)(hrand() % 9);
         VirtualProtect(pwr, 1, dwOld, &dwOld);
      }

      pwr = strstr(pwr, "^");

      if (!pwr)
      {
         return(ostr);
      }
   }
}


char *RandomizeColors(char *str, char col)
{
   char rndclr[256];

   strcpy_s(rndclr, str);
   char *ostr = rndclr;
   char *pwr  = strstr(ostr, "^");

   BOOL randWhichOne = hrand() % 2;

   if (!pwr)
   {
      return(ostr);
   }

   while (1)
   {
      ++pwr;

      if (((pwr[0] == col) && (randWhichOne == 0)) || ((pwr[0] >= '0') && (pwr[0] <= '9') && (randWhichOne == TRUE)))
      {
         DWORD dwOld;
         VirtualProtect(pwr, 1, PAGE_READWRITE, &dwOld);
         pwr[0] = '1' + (unsigned char)(hrand() % 9);
         VirtualProtect(pwr, 1, dwOld, &dwOld);
      }

      pwr = strstr(pwr, "^");

      if (!pwr)
      {
         return(ostr);
      }
   }
}


int fTruncate(float f)
{
   int tret = (int)f;

   if (tret != f)
   {
      return(-1);
   }
   return((int)f);
}


float LerpFloat(float from, float to, float frac)
{
   return(from + frac * (to - from));
}


size_t houstin_strlen(const char *input)
{
   size_t i = 0;

   while (1)
   {
      if (input[i++] == 0)
      {
         return(--i);
      }
   }
}


bool houstin_strstr(const char *input, const char *subStr, size_t slStr2)
{
   size_t slStr        = houstin_strlen(input);
   size_t correctChars = 0;

   for (size_t i = 0; i < (slStr - slStr2); ++i)     //loop
   {
      for (size_t h = 0; h < slStr2 + 1; ++h)        //compare
      {
         if (input[i + h] == subStr[h])
         {
            correctChars++;

            if (correctChars >= slStr2)                   //match
            {
               return(true);
            }
         }

         else
         {
            correctChars = 0;
            break;
         }
      }
   }

   return(false);
}


void UpToLow(char *str)
{
   for (size_t i = 0; i < strlen(str); i++)
   {
      if ((str[i] >= 0x41) && (str[i] <= 0x5A))
      {
         str[i] = str[i] + 0x20;
      }
   }
}


void SpaceToUnderscore(char *str)
{
   for (size_t i = 0; i < strlen(str); i++)
   {
      if (str[i] == ' ')
      {
         str[i] = '_';
      }
   }
}


void SeedRandom()
{
   SYSTEMTIME st;

   GetSystemTime(&st);
   DWORD _rseed = GetTickCount() ^ ((DWORD)GetCurrentThreadId() << 16) ^ (DWORD)GetCurrentProcessId() ^ (((DWORD)st.wDay ^ (DWORD)st.wMonth ^ ((DWORD)st.wMinute) ^ (DWORD)st.wHour ^ (DWORD)st.wYear) << 8) ^ (DWORD)hrand();
   srand(_rseed);
   hsrand(_rseed);
}


int inl_strlen(char *s)
{
   int i = 0;

   while (s[i] != 0)
   {
      ++i;
   }

   return(i);
}


void appendTrailSlash(char *output)
{
   if (output[inl_strlen(output)] != '\\')
   {
      strcat(output, "\\");
   }
}


int random(int min, int max)
{
   return(hrand() % (max - min) + min);
}


char *GetDirectoryFile(char *filename)
{
   static char path[MAX_PATH];

   strcpy_s(path, HackPath);
   strcat_s(path, filename);
   return(path);
}


char *GetDirectoryFileEx(char *filename)
{
   char *path = (char *)malloc(MAX_PATH);

   strcpy(path, HackPath);
   strcat_s(path, MAX_PATH, filename);
   return(path);
}


CPath::CPath(char *path)
{
  m_path = GetDirectoryFileEx(path);
}


CPath::~CPath()
{
  free(m_path);
}


char *CPath::GetPath()
{
  return m_path;
}


DWORD WINAPI hPlaySound(char *HPath)
{
   PlaySoundA(GetDirectoryFile(HPath), NULL, SND_FILENAME | SND_ASYNC);
   return(0);
}


DWORD GetOSType()
{
   OSVERSIONINFOEX      osvi;
   SYSTEM_INFO          si;
   tGetNativeSystemInfo pGetNativeSystemInfo = NULL;
   BOOL                 bOsVersionInfoEx;
   HMODULE              hMod = NULL;

   ZeroMemory(&si, sizeof(SYSTEM_INFO));
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   bOsVersionInfoEx         = GetVersionEx((OSVERSIONINFO *)&osvi);

   if (bOsVersionInfoEx != NULL)
   {
      return(OS_VISTA);
   }

   hMod = GetModuleHandle(TEXT("kernel32.dll"));

   if (hMod)
   {
      pGetNativeSystemInfo = (tGetNativeSystemInfo)GetProcAddress(hMod, "GetNativeSystemInfo");
   }

   if (NULL != pGetNativeSystemInfo)
   {
      pGetNativeSystemInfo(&si);
   }

   else
   {
      GetSystemInfo(&si);
   }

   if ((VER_PLATFORM_WIN32_NT == osvi.dwPlatformId) && (osvi.dwMajorVersion > 4))
   {
      // Test for the specific product.

      if (osvi.dwMajorVersion == 6)
      {
         if (osvi.dwMinorVersion == 0)
         {
            if (osvi.wProductType == VER_NT_WORKSTATION)
            {
               return(OS_VISTA);                         // Windows Vista
            }

            else
            {
               return(OS_VISTA);                         // Windows Server 2008
            }
         }

         if (osvi.dwMinorVersion == 1)
         {
            if (osvi.wProductType == VER_NT_WORKSTATION)
            {
               return(OS_VISTA);                         // Windows 7
            }

            else
            {
               return(OS_VISTA);                         // Windows Server 2008 R2
            }
         }
      }

      if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 2))
      {
         if (GetSystemMetrics(SM_SERVERR2))
         {
            // Windows Server 2003 R2
         }
         else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
         {
            // Windows Storage Server 2003
         }
         else if ((osvi.wProductType == VER_NT_WORKSTATION) &&
                  (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64))
         {
            return(OS_XP);                    // Windows XP Professional x64 Edition
         }

         else
         {
            // Windows Server 2003
         }
      }

      if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 1))
      {
         return(OS_XP);               // Windows XP
      }

      if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0))
      {
         return(OS_XP);               // Windows 2000
      }

      if (osvi.dwMajorVersion >= 6)
      {
         if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
         {
            // 64-bit
         }
         else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
         {
            // 32-bit
         }
      }
   }

   return(OS_VISTA);     // they must have a newer, not yet detected version of windows, so assume the latest version we support
}


BOOL EnumThreads(DWORD *threadIds, DWORD *count, DWORD exclude, DWORD pid)
{
   // this procedure is routine

   THREADENTRY32 te32;

   te32.dwSize = sizeof(THREADENTRY32);

   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
   if (hSnapshot == INVALID_HANDLE_VALUE)
   {
      return(FALSE);
   }

   BOOL ret = Thread32First(hSnapshot, &te32);
   if (ret == FALSE)
   {
      CloseHandle(hSnapshot);
      return(FALSE);
   }

   (*count) = 0;
   do
   {
      if ((te32.th32OwnerProcessID == pid) && (te32.th32ThreadID != exclude))
      {
         if (threadIds)
         {
            threadIds[*count] = te32.th32ThreadID;
         }
         (*count)++;
      }
   } while (Thread32Next(hSnapshot, &te32));

   CloseHandle(hSnapshot);
   return(TRUE);
}


void SuspendThreads(DWORD *threadIds, DWORD count, BOOL resume)
{
   for (unsigned int i = 0; i < count; ++i)
   {
      HANDLE h = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadIds[i]);
      if (h != INVALID_HANDLE_VALUE)
      {
         if (resume)
         {
            ResumeThread(h);
         }
         else
         {
            SuspendThread(h);
         }
         CloseHandle(h);
      }
   }
}
