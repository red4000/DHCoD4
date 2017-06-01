#pragma warning ( disable : 4530 )
#include <windows.h>
#include <iostream>
#pragma hdrstop

#define MakePtr(cast, ptr, addValue)    (cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue)) // by Matt Pietrek

/* BEGIN DETOURXS CODE BY SINNER */
#ifdef _M_IX86
#ifdef _MSC_VER // NOTE: if you're not compiling with msvc, then you'll have to set your compiler to link LDE
#pragma comment(lib, "LDE64")
#endif
extern "C" size_t __stdcall LDE(const LPVOID lpData, unsigned int size);

#else
#ifdef _MSC_VER // NOTE: if you're not compiling with msvc, then you'll have to set your compiler to link LDE
#pragma comment(lib, "LDE64x64")
#endif
extern "C" size_t __fastcall LDE(const LPVOID lpData, unsigned int size);

#endif

static const size_t relativeJmpSize = 1 + sizeof(DWORD);
static const size_t absoluteJmpSize = 2 + sizeof(DWORD) + sizeof(DWORD_PTR);

enum JmpType :
size_t
{
   Relative = relativeJmpSize,
   Absolute = absoluteJmpSize
};

JmpType GetJmpType(const LPBYTE lpbFrom, const LPBYTE lpbTo)
{
#undef min
#undef max

   const DWORD_PTR upper = reinterpret_cast<DWORD_PTR>(std::max(lpbFrom, lpbTo));
   const DWORD_PTR lower = reinterpret_cast<DWORD_PTR>(std::min(lpbFrom, lpbTo));

   return((upper - lower > 0x7FFFFFFF) ? Absolute : Relative);
}


void WriteJump(const LPBYTE lpbFrom, const LPBYTE lpbTo, JmpType jmpType)
{
   if (jmpType == Absolute)
   {
      lpbFrom[0] = 0xFF;
      lpbFrom[1] = 0x25;

      // FF 25 [ptr_to_jmp(4bytes)][jmp(4bytes)]
      *reinterpret_cast<PDWORD>(lpbFrom + 2) = reinterpret_cast<DWORD>(lpbFrom) + 6;
      *reinterpret_cast<PDWORD>(lpbFrom + 6) = reinterpret_cast<DWORD>(lpbTo);
      FlushInstructionCache(GetCurrentProcess(), lpbFrom, 10);
   }

   else if (jmpType == Relative)
   {
      // E9 [to - from - jmp_size]
      lpbFrom[0] = 0xE9;
      DWORD offset = reinterpret_cast<DWORD>(lpbTo) - reinterpret_cast<DWORD>(lpbFrom) - relativeJmpSize;
      *reinterpret_cast<PDWORD>(lpbFrom + 1) = static_cast<DWORD>(offset);
      FlushInstructionCache(GetCurrentProcess(), lpbFrom, 5);
   }
}


void WriteJump(const LPBYTE lpbFrom, const LPBYTE lpbTo)
{
   //JmpType jmpType = GetJmpType(lpbFrom, lpbTo);
   WriteJump(lpbFrom, lpbTo, Absolute);
}


/* END DETOURXS CODE BY SINNER */

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
   FlushInstructionCache(GetCurrentProcess(), lpbFrom, 7);
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
      return(tramp);
   }

   DWORD ignore;
   VirtualProtect(tramp, 1024, PAGE_EXECUTE_READWRITE, &ignore); // make executable

   memcpy(tramp, at, len);                                       //copy opcodes
   FlushInstructionCache(GetCurrentProcess(), tramp, len);

   void *reentry   = MakePtr(void *, at, len);
   void *tramp_jmp = MakePtr(void *, tramp, len);
   WriteJump((BYTE *)tramp_jmp, (BYTE *)reentry);
   return(tramp);
}


void *DetourFunction(void *from, void *to)
{
   DWORD dwOldProt, dwOldProt2;

   VirtualProtect(from, 15, PAGE_EXECUTE_READWRITE, &dwOldProt);
   size_t        tLen   = GetTrampolineLength(from, 7);
   void          *tramp = BuildTrampoline(from, tLen);
   unsigned char *stub  = (unsigned char *)VirtualAlloc(0, 15, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
   VirtualProtect(stub, 15, PAGE_EXECUTE_READWRITE, &dwOldProt2);
   stub[0] = 0x83;
   stub[1] = 0xC4;
   stub[2] = 0x04;
   WriteJump((BYTE *)stub + 3, (BYTE *)to);
   FlushInstructionCache(GetCurrentProcess(), stub, 15);
   WriteJumpEx((BYTE *)from, (BYTE *)stub);
   FlushInstructionCache(GetCurrentProcess(), 0, 0);
   VirtualProtect(from, 15, dwOldProt, &dwOldProt);
   return(tramp);
}


void RetourFunction(void *from, void *tramp)
{
   DWORD dwOldProt;

   VirtualProtect(from, 15, PAGE_EXECUTE_READWRITE, &dwOldProt);
   size_t tLen = GetTrampolineLength(from, 7);
   memcpy(from, tramp, tLen);
   FlushInstructionCache(GetCurrentProcess(), from, tLen);
   VirtualFree(tramp, 0, MEM_RELEASE);
   VirtualProtect(from, 15, dwOldProt, &dwOldProt);
}
