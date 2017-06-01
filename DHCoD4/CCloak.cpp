#pragma warning ( disable : 4530 )
#include <stdlib.h>
#include <windows.h>
#pragma hdrstop

#include "CCloak.h"
#include "hMath.h"
#include "hConfig.h"
#include "hBot.h"

MFBEG(bool, CloakDll(HMODULE hMod));
bool CloakDll(HMODULE hMod)
{
   ANTI_CRASH_BEGIN

   ProcessModuleInfo *pmInfo = NULL;
   ModuleInfoNode    *module = NULL;

   _asm
   {
      push esi
      mov esi, fs :[18h]                // TEB
      mov esi, [esi + 30h]              // PEB
      mov esi, [esi + 0Ch]              // PROCESS_MODULE_INFO

      mov pmInfo, esi
      pop esi
   }

   module = (ModuleInfoNode *)(pmInfo->LoadOrder.Flink);

   while (module->baseAddress && module->baseAddress != hMod)
   {
      module = (ModuleInfoNode *)(module->LoadOrder.Flink);
   }

   if (!module->baseAddress)
   {
      return(false);
   }

   //	Remove the module entry from the list here
   ///////////////////////////////////////////////////
   //	Unlink from the load order list
   UNLINK(module->LoadOrder);
   //	Unlink from the init order list
   UNLINK(module->InitOrder);
   //	Unlink from the memory order list
   UNLINK(module->MemoryOrder);
   //	Unlink from the hash table
   UNLINK(module->HashTable);

   //	Erase all traces that it was ever there
   ///////////////////////////////////////////////////

   //	This code will pretty much always be optimized into a rep stosb/stosd pair
   //	so it shouldn't cause problems for relocation.
   //	Zero out the module name
   memset(module->name.Buffer, 0, module->name.Length);
   memset(module->fullPath.Buffer, 0, module->fullPath.Length);

   //	Zero out the memory of this module's node
   memset(module, 0, sizeof(ModuleInfoNode));

   PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMod;
   PIMAGE_NT_HEADERS pNtHeader  = (PIMAGE_NT_HEADERS)(hMod + pDosHeader->e_lfanew);
   DWORD             oldp;

   //  Zero out our NT header(partially, atleast)
   VirtualProtect(pNtHeader, sizeof(IMAGE_NT_HEADERS), PAGE_EXECUTE_READWRITE, &oldp);
   memset(&pNtHeader->FileHeader, 0, sizeof(IMAGE_FILE_HEADER));
   pNtHeader->Signature = 0;
   VirtualProtect(pNtHeader, sizeof(IMAGE_NT_HEADERS), oldp, &oldp);
   // TODO: zero out idata

   //  Zero out our DOS header
   VirtualProtect(pDosHeader, sizeof(IMAGE_DOS_HEADER), PAGE_EXECUTE_READWRITE, &oldp);
   memset(pDosHeader, 0, sizeof(IMAGE_DOS_HEADER));
   VirtualProtect(pDosHeader, sizeof(IMAGE_DOS_HEADER), oldp, &oldp);

   ANTI_CRASH_END

   return(true);
}


MFEND(bool, CloakDll(HMODULE hMod));
