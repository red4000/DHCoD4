#pragma warning ( disable : 4530 )
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#pragma hdrstop

#include "hTypes.h"
#include "hLog.h"

 // TODO: functions for hex dumping memory.

char    out[512];
HMODULE hMods[1024];
HANDLE  hProcess;
DWORD   cbNeeded;

BOOL UpdateModuleCache()
{
   return(EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded));
}


bool __stdcall CheckValidReadPtr( DWORD Protect )
{
	if( (Protect & PAGE_GUARD ) == PAGE_GUARD ) // page is guarded
		return false;

	if( ( Protect & PAGE_EXECUTE_READ ) == PAGE_EXECUTE_READ || ( Protect & PAGE_READONLY ) == PAGE_READONLY || ( Protect & PAGE_READWRITE ) == PAGE_READWRITE || ( Protect & PAGE_EXECUTE_READ ) == PAGE_EXECUTE_READ ) // page is readable
		return true;

	return false;
}


BOOL IsAddressReadable(ADDRESS_PROC ea) // TODO: finish implementing
{
  MEMORY_BASIC_INFORMATION mbi;
  VirtualQuery((LPVOID)ea,&mbi,64); // TODO: check return value
  return FALSE;
}


BOOL IsPointerToASCII(ADDRESS_PROC ea)
{/*
  if(TRUE == IsAddressReadable(ea))
  {
    ADDRESS_PROC _ea = *((ADDRESS_PROC*)ea);
    if(TRUE == IsAddressReadable(_ea))
    {
      char *pea = (char *)ea;
#define IS_ASCII(x) (((x) >= 'a' && ((x) <= 'z')) || ((x)>='A' && (x)<='Z') || ((x)>='0' && (x)<='9')))
      unsigned int numAscii = 0;
      for(size_t i = 0; i < 4; ++i)
      {
        if(IS_ASCII(pea[i]))
          ++numAscii;
      }
      if(numAscii >= 3)
      {
        if(strlen_s(pea,64) < 62)
        {
          return TRUE;
        }
      }
    }
  }
  */
   return(FALSE);
}


void GetExtraInfo(char* output, ADDRESS_PROC ea)
{
    output[0] = '\0';
	return;
#if 0
  ADDRESS_PROC *pea = &ea;
  if(IsPointerToASCII((ADDRESS_PROC)pea))
  {
    // ea points to an ASCII string.
    sprintf(output, " \"%s\"", (char*)ea);
  }
  else if(IsPointerToASCII(ea))
  {
    // ea points to a pointer of an ASCII string.
    sprintf(output, " PTR to \"%s\"", *((char**)ea));
  }
  else
  {
    output[0] = '\0';
  }
#endif
}


char *GetAddressStringAsRVA(ADDRESS_PROC ea)
{
   unsigned int i;
   char extraInfo[128];
   
   GetExtraInfo(extraInfo, ea);

   if (ea < 0x00100000)
   {
      if (ea == 0)
      {
         strcpy_s(out, "0");
         return(out);
      }
      sprintf_s(out, "%08X%s", ea, extraInfo);
      return(out);
   }

   for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
   {
      char       szModName[MAX_PATH];
      MODULEINFO mi;

      GetModuleInformation(hProcess, hMods[i], &mi, sizeof(MODULEINFO));
      if ((ea >= (ADDRESS_PROC)hMods[i]) && (ea <= (ADDRESS_PROC)hMods[i] + (ADDRESS_PROC)mi.SizeOfImage))
      {
         if (GetModuleFileNameExA(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(char)))
         {
            char *imagename = strrchr(szModName, '\\') + 1;
            if (!imagename)
            {
               imagename = szModName;
            }
            char *ext = strchr(imagename, '.');
            if (ext)
            {
               *ext = 0;
            }
            sprintf_s(out, "%s.%08X%s", imagename, ea - (ADDRESS_PROC)hMods[i], extraInfo);
            return(out);
         }
      }
   }

   sprintf_s(out, "%08X%s", ea, extraInfo);
   return(out);
}


char *GetCommonExceptionStr(DWORD ExceptionCode)
{
   switch (ExceptionCode)
   {
   case EXCEPTION_ACCESS_VIOLATION:
      return("EXCEPTION_ACCESS_VIOLATION");

   case EXCEPTION_DATATYPE_MISALIGNMENT:
      return("EXCEPTION_DATATYPE_MISALIGNMENT");

   case EXCEPTION_BREAKPOINT:
      return("EXCEPTION_BREAKPOINT");

   case EXCEPTION_SINGLE_STEP:
      return("EXCEPTION_SINGLE_STEP");

   case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      return("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");

   case EXCEPTION_FLT_DENORMAL_OPERAND:
      return("EXCEPTION_FLT_DENORMAL_OPERAND");

   case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      return("EXCEPTION_FLT_DIVIDE_BY_ZERO");

   case EXCEPTION_FLT_INEXACT_RESULT:
      return("EXCEPTION_FLT_INEXACT_RESULT");

   case EXCEPTION_FLT_INVALID_OPERATION:
      return("EXCEPTION_FLT_INVALID_OPERATION");

   case EXCEPTION_FLT_OVERFLOW:
      return("EXCEPTION_FLT_OVERFLOW");

   case EXCEPTION_FLT_STACK_CHECK:
      return("EXCEPTION_FLT_STACK_CHECK");

   case EXCEPTION_FLT_UNDERFLOW:
      return("EXCEPTION_FLT_UNDERFLOW");

   case EXCEPTION_INT_DIVIDE_BY_ZERO:
      return("EXCEPTION_INT_DIVIDE_BY_ZERO");

   case EXCEPTION_INT_OVERFLOW:
      return("EXCEPTION_INT_OVERFLOW");

   case EXCEPTION_PRIV_INSTRUCTION:
      return("EXCEPTION_PRIV_INSTRUCTION");

   case EXCEPTION_IN_PAGE_ERROR:
      return("EXCEPTION_IN_PAGE_ERROR");

   case EXCEPTION_ILLEGAL_INSTRUCTION:
      return("EXCEPTION_ILLEGAL_INSTRUCTION");

   case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      return("EXCEPTION_NONCONTINUABLE_EXCEPTION");

   case EXCEPTION_STACK_OVERFLOW:
      return("EXCEPTION_STACK_OVERFLOW");

   case EXCEPTION_INVALID_DISPOSITION:
      return("EXCEPTION_INVALID_DISPOSITION");

   case EXCEPTION_GUARD_PAGE:
      return("EXCEPTION_GUARD_PAGE");

   case EXCEPTION_INVALID_HANDLE:
      return("EXCEPTION_INVALID_HANDLE");

   //case EXCEPTION_POSSIBLE_DEADLOCK: return "EXCEPTION_POSSIBLE_DEADLOCK";
   //default: return "EXCEPTION_UNKNOWN";
   default:
      return(NULL);
   }
}


LONG WINAPI CrashHandler(EXCEPTION_POINTERS *pExceptionInfo)
{
   char *szException = GetCommonExceptionStr(pExceptionInfo->ExceptionRecord->ExceptionCode);

   if (szException)
   {
      UpdateModuleCache();
      if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
      {
         if (pExceptionInfo->ExceptionRecord->NumberParameters != 0)
         {
            if (pExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 0)                    // reading
            {
               Logf("[%s] at %s reading %08X\n", szException, GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip), pExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
            }
            else if (pExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 1)                    // writing
            {
               Logf("[%s] at %s writing %08X\n", szException, GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip), pExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
            }
            goto logged;
         }
      }
      Logf("[%s] at %s\n", szException, GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip));
logged:
#ifdef REGISTER_DUMP
      Logf("EAX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eax));
      Logf("ECX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Ecx));
      Logf("EDX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Edx));
      Logf("EBX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Ebx));
      Logf("ESP %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Esp));
      Logf("EBP %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Ebp));
      Logf("ESI %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Esi));
      Logf("EDI %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Edi));
      Logf("EIP %s\n\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip));
      Logf("EFL %08X\n\n", pExceptionInfo->ContextRecord->EFlags);
#endif // REGISTER_DUMP

#ifdef STACK_DUMP
      // traverse stack upwards to end of stack page
      MEMORY_BASIC_INFORMATION mbi;
      VirtualQuery((LPCVOID)pExceptionInfo->ContextRecord->Esp, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
      // TODO: make a loop to add more pages as long as they have PAGE_READWRITE
      if (mbi.Protect == PAGE_READWRITE)
      {
         ADDRESS_PROC ea2 = (ADDRESS_PROC)mbi.BaseAddress + mbi.RegionSize;
         for (ADDRESS_PROC ea = (ADDRESS_PROC)pExceptionInfo->ContextRecord->Esp; ea < ea2; ea += 4)
         {
            Logf("%08X: %s\n", ea, GetAddressStringAsRVA(*(ADDRESS_PROC *)ea));
         }
      }
#endif // STACK_DUMP
      Sleep(200);           // sleep to make sure the log gets saved.
   }
   return(EXCEPTION_CONTINUE_SEARCH);
}
