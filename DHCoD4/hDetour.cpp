#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#define FIC(x, y, z)
#if 0
#define FIC FlushInstructionCache
#endif

#define MakePtr(cast, ptr, addValue)    (cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue)) // by Matt Pietrek

#ifdef USE_LDE
#ifdef _M_IX86
#ifdef _MSC_VER // NOTE: if you're not compiling with msvc, then you'll have to set your compiler to link LDE
#pragma comment(lib, "LDE64")
#endif // _MSC_VER
extern "C" size_t __stdcall LDE(const LPVOID lpData, unsigned int size);

#endif // _M_IX86
#else
#include "xde.h"
#define LDE(x, y)    (size_t)xde((unsigned char *)x)
#endif // USE_LDE

// TODO: use MF* code to make wiping/randomization of this code possible

void WriteJump(const LPBYTE lpbFrom, const LPBYTE lpbTo)
{
   // E9 [to - from - jmp_size]
   lpbFrom[0] = 0xE9;
   DWORD offset = reinterpret_cast<DWORD>(lpbTo) - reinterpret_cast<DWORD>(lpbFrom) - 5;
   *reinterpret_cast<PDWORD>(lpbFrom + 1) = static_cast<DWORD>(offset);
   FIC(GetCurrentProcess(), lpbFrom, 5);
}


void WriteJumpEx(const LPBYTE lpbFrom, const LPBYTE lpbTo)
{
   // allocate memory for a pointer to the hook
   void *tramp = VirtualAlloc(0, sizeof(BYTE *), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);     // TODO: allocate memory somehow else, cos we don't need a whole page for 15 bytes max

   if (!tramp)
   {
      return;
   }

   *(BYTE **)tramp = lpbTo;

   //write CALL [x]
   *(BYTE *)lpbFrom = 0xFF;
   *(BYTE *)MakePtr(BYTE *, lpbFrom, 1)    = 0x15;
   *(BYTE **)MakePtr(BYTE * *, lpbFrom, 2) = (BYTE *)tramp;
   FIC(GetCurrentProcess(), lpbFrom, 7);
}


size_t GetTrampolineLength(void *at, size_t lenOfHook)
{
   size_t cSize = 0;

   while (1)
   {
      void *ptr = MakePtr(void *, at, cSize);
      cSize += LDE(ptr, 0);

      if (cSize >= lenOfHook)
      {
         break;
      }
   }

   return(cSize);
}


void *BuildTrampoline(void *at, size_t len)
{
   void *tramp = VirtualAlloc(0, 20, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

   if (!tramp)
   {
      return(NULL);
   }

   DWORD ignore;
   VirtualProtect(tramp, 1024, PAGE_EXECUTE_READWRITE, &ignore); // make executable

   memcpy(tramp, at, len);                                       //copy opcodes

   void *reentry   = MakePtr(void *, at, len);
   void *tramp_jmp = MakePtr(void *, tramp, len);
   WriteJump((BYTE *)tramp_jmp, (BYTE *)reentry);
   FIC(GetCurrentProcess(), tramp, len);
   return(tramp);
}


void *DetourFunction(void *from, void *to, void **tramp)
{
   DWORD dwOldProt, dwOldProt2;

   VirtualProtect(from, 15, PAGE_EXECUTE_READWRITE, &dwOldProt);
   if (*tramp == 0)
   {
      size_t tLen = GetTrampolineLength(from, 7);
      *tramp = BuildTrampoline(from, tLen);
   }
   unsigned char *stub = (unsigned char *)VirtualAlloc(0, 15, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
   if (!stub || !(*tramp))
   {
      return(NULL);
   }
   VirtualProtect(stub, 15, PAGE_EXECUTE_READWRITE, &dwOldProt2);
   stub[0] = 0x83;
   stub[1] = 0xC4;
   stub[2] = 0x04;
   WriteJump((BYTE *)stub + 3, (BYTE *)to);
   FIC(GetCurrentProcess(), stub, 15);
   WriteJumpEx((BYTE *)from, (BYTE *)stub);
   FIC(GetCurrentProcess(), from, 7);
   VirtualProtect(from, 15, dwOldProt, &dwOldProt);
   return(tramp);
}


void *DetourFunctionJmp(void *from, void *to, void **tramp)
{
   DWORD dwOldProt;

   VirtualProtect(from, 15, PAGE_EXECUTE_READWRITE, &dwOldProt);
   if (*tramp == 0)
   {
      size_t tLen = GetTrampolineLength(from, 5);
      *tramp = BuildTrampoline(from, tLen);
   }
   WriteJump((BYTE *)from, (BYTE *)to);
   FIC(GetCurrentProcess(), from, 5);
   VirtualProtect(from, 15, dwOldProt, &dwOldProt);
   return(*tramp);
}


void RetourFunction(void *from, void *tramp)
{
   DWORD dwOldProt;

   VirtualProtect(from, 15, PAGE_EXECUTE_READWRITE, &dwOldProt);
   size_t tLen = GetTrampolineLength(from, 5);
   memcpy(from, tramp, tLen);
   FIC(GetCurrentProcess(), from, tLen);
#if 0
   VirtualFree(tramp, 0, MEM_RELEASE);
#endif
   VirtualProtect(from, 15, dwOldProt, &dwOldProt);
}
