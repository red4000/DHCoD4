#pragma warning (disable : 4530)
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <intrin.h>
#include <vector>
#pragma hdrstop
#pragma comment(lib, "dxguid.lib")

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hDisplay.h"
#include "hConfig.h"
#include "hBot.h"
#include "hDetour.h"
#include "hLog.h"
#include "hAntiPB.h"
#include "VMProtectSDK.h"
#include "hAPI.h"
#include "WELL44497a_new.h"

#include "CPE.h"

using namespace std;

ADDRESS_PROC op_pb_localScreenShot  = NULL;
ADDRESS_PROC op_pb_remoteScreenShot = NULL;

extern DWORD dwSafetyTime;

ADDRESS_PROC dwPBBase = NULL;

const DWORD dwPBSize  = 940000; // TODO: get this by reading PE.OptionalHeader.imageSize or whatever
DWORD pb_xorKey;

CWELLRNG44497 rng;
char alphabet[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
char key[70];
char oldKey[42];

typedef DWORD (WINAPI* tGetWindowThreadProcessId)(HWND,LPDWORD);
tGetWindowThreadProcessId oGetWindowThreadProcessId;

#if HVERSION != 3

void AntiDvarCheckf(CCVar **ppCurDvar, float defaultVal, float minVal, float maxVal, CCVar** ppoverride) // TODO: merge this into the DWORD based one somehow, maybe by changing it into some macro.
{
   if(0==(*ppCurDvar))
   {
      return;
   }
   CCVar *pointer = *ppCurDvar, **gppCurDvar;
   if(ppoverride == 0)
      gppCurDvar = (CCVar**)FindPattern(0x71B000, 0xD933600-0x71B000,(BYTE*)&pointer,"xxxx",FALSE); // TODO: calculate .data section pointer and size by reading PE header of game
   else
      gppCurDvar = ppoverride;
   if(!gppCurDvar)
      return;
   (*ppCurDvar) = (*gppCurDvar) = CG_DvarCreate(randomString(randomInt(5, 12)), randomString(randomInt(4, 20)), &defaultVal, &minVal, &maxVal);
}


void AntiDvarCheckdw(CCVar **ppCurDvar, DWORD defaultVal, DWORD minVal, DWORD maxVal, CCVar** ppoverride)
{
   if(0==(*ppCurDvar))
   {
      return;
   }
   CCVar *pointer = *ppCurDvar, **gppCurDvar;
   if(ppoverride == 0)
      gppCurDvar = (CCVar**)FindPattern(0x71B000, 0xD933600-0x71B000,(BYTE*)&pointer,"xxxx",FALSE); // TODO: calculate .data section pointer and size by reading PE header of game
   else
      gppCurDvar = ppoverride;
   if(!gppCurDvar)
      return;
   (*ppCurDvar) = (*gppCurDvar) = CG_DvarCreate(randomString(randomInt(5, 12)), randomString(randomInt(4, 20)), (float*)&defaultVal, (float*)&minVal, (float*)&maxVal);
}


void __declspec(naked) h_pb_remoteScreenShot()
{
   PRESERVE

   if (!cvar_PBSSCleaner || (cvar_PBSSCleaner->iValue == 1))
   {
      DisableVisuals();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
      Com_Frame();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
      Com_Frame();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
      Com_Frame();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
   }

   RESTORE
   _asm jmp [op_pb_remoteScreenShot]
}

void __declspec(naked) h_pb_localScreenShot()
{
   PRESERVE

   if (!cvar_PBSSCleaner || (cvar_PBSSCleaner->iValue == 1))
   {
      DisableVisuals();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
      Com_Frame();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
      Com_Frame();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
      Com_Frame();
      dwSafetyTime = GetTickCount() + SS_SAFETY_TIME;
   }

   RESTORE
   _asm jmp [op_pb_localScreenShot]
}

BOOL WINAPI hGetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessId)
{
	DWORD pbBaseAddress	= (DWORD)GetModuleHandleA("pbcl.dll");
	DWORD RetAddr		= (DWORD)_ReturnAddress();

	if((RetAddr > pbBaseAddress) && (RetAddr < (pbBaseAddress + dwPBSize)))
	{
		return 0;
	}
	
	return oGetWindowThreadProcessId(hWnd, lpdwProcessId);
}

void PBBypass(); // -------------------------------------------------------------------

int (__cdecl *opbHook)(DWORD, char*, DWORD, char*);

int SetGUID(char *key) // changes key to 64 byte GUID
{
	DWORD setGUID = 0x4699A0;
	int result;

	__asm
	{
		push key
		call [setGUID]
		add esp, 4
		mov result, eax
	}

	return result;
}

void GetCurrentKey(char *key)
{
	memcpy((void*)key, (void*)0x724B84, 32);
	memcpy((void*)&key[33], (void*)0x724BA8, 5);
}

void SetCurrentKey(char *key)
{
	memcpy((void*)0x724B84, (void*)key, 32);
	memcpy((void*)0x724BA8, (void*)&key[33], 5);
}

void SetCurrentGUID(char *guid)
{
	memcpy((void*)0x6D04F8, (void*)guid, 65);
}

void GenerateKey(char *key)
{
	int key5 = 0;

	for (size_t i = 0; i < 16; i++)
	{
		key[i] = alphabet[rng.WELLRNG44497(&rng) % 36];
		key5  ^= key[i];

		for (size_t j = 8; j > 0; --j)
		{
			if (key5 & 1)
			{
				key5 ^= 0x14002;
			}

			key5 >>= 1;
		}
	}

	sprintf(&key[16], "%04X", key5);

	key[20] = 0;
}

int IsKeyValid(char *key)
{
	char  key5[5];
	DWORD checkKey = 0x4712E0;
	int   valid;

	strcpy(key5, &key[16]);
	key[16] = 0;

	__asm
	{
		mov edi, key
		lea esi, [edi + 16]
		call [checkKey]
		mov [valid], eax
	}

	strcpy(&key[16], key5);

	return valid;
}

int pbHook(DWORD a1, char *hostname, DWORD cl_pb, char *guid) // cl_pb = cl_punkbuster->value
{
	GetCurrentKey(oldKey);
	SetCurrentKey(key);
	SetGUID(key);
	SetCurrentGUID(key);
	int result = opbHook(a1, hostname, cl_pb, guid);
	//SetCurrentKey(oldKey);
	//SetGUID(oldKey);

	return result;
}

// normal guid ce5a791fb046dc9fe7f064824f767be8

DWORD oshowpackets;

LONG WINAPI ExceptionHandlerPB(EXCEPTION_POINTERS *pExceptionInfo)
{
	switch (pExceptionInfo->ContextRecord->Eip)
	{
	case 0x46C205:
	case 0x50839E:
	case 0x50842C:
		pExceptionInfo->ContextRecord->Edx = oshowpackets;
		break;

	case 0x508093:
	case 0x5082BE:
	case 0x508B45:
	case 0x508DB9:
		pExceptionInfo->ContextRecord->Eax = oshowpackets;
		break;

	case 0x508C15:
		pExceptionInfo->ContextRecord->Eax = oshowpackets;

		if (0x469D50 == *(DWORD*)(pExceptionInfo->ContextRecord->Esp + 0x10))
		{
			char *authorizeString = (char*)pExceptionInfo->ContextRecord->Esi;
			CG_EchoToConsolef(CONSOLE, "---------------------------------------\n");
			CG_EchoToConsolef(CONSOLE, "hacked keyauthorize\n");
			CG_EchoToConsolef(CONSOLE, "---------------------------------------\n");
		}
		break;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

DWORD WINAPI AntiBanThread(LPVOID)
{
	SYSTEMTIME st;
	unsigned int prngState[1391];

	GetLocalTime(&st);
	srand(GetTickCount() ^ st.wDay ^ (st.wHour << 5) ^ (st.wMinute << 12));

	for (size_t i = 0; i < 1391; i++)
	{
		prngState[i] = rand() ^ (rand() << 16);
	}

	rng.InitWELLRNG44497(prngState);

	for (size_t i = 0; i < 3000; i++)
	{
		rng.WELLRNG44497(&rng);
	}

	DWORD startTime = GetTickCount();
	
	while (1)
	{
		GenerateKey(key);

		if (IsKeyValid(key))
		{
			break;
		}

		Sleep(0);
	}

	DWORD deltaTime = GetTickCount() - startTime;
	CG_EchoToConsolef(CONSOLE, "--DopeBlocker--\n", deltaTime);
	CG_EchoToConsolef(CONSOLE, "Keygen took %ims\n", deltaTime);
	CG_EchoToConsolef(CONSOLE, "---------------\n", deltaTime);
	MessageBeep(0);

	while (1)
	{
		CG_EchoToConsolef(CONSOLE, "--DopeBlocker ready--\n", deltaTime);

		while (0 == *(DWORD*)0xD8B2EC4)
		{
			if (((DWORD)pbHook) != *(DWORD*)0xD8B2EC4)
			{
				opbHook = (int (__cdecl *)(DWORD, char*, DWORD, char*))*(DWORD*)0xD8B2EC4;
				*(DWORD*)0xD8B2EC4 = (DWORD)pbHook;
			}

			Sleep(50);
		}

		Sleep(500);
	}

	return 0;
}

MFUNC(void, AntiPBSSThread(LPVOID));

/*
HRESULT WINAPI hDirectDrawCreate(GUID *guid, LPDIRECTDRAW *lpDraw, IUnknown *iUnk)
{
	HRESULT res = oDirectDrawCreate(guid, lpDraw, iUnk);

	if (myIDirectDraw)
	{
		delete myIDirectDraw;
		myIDirectDraw = 0;
	}

	myIDirectDraw = new hkIDirectDraw(lpDraw);

	Logf("hooked DirectDrawCreate, return address %08X\n", _ReturnAddress());

	return res;
}
*/

MFBEG(void, AntiPBSSThread(LPVOID));
void AntiPBSSThread(LPVOID)
{
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AntiBanThread, 0, 0, 0);
   // TODO: encrypt these strings
	//pDirectDrawCreate = (tDirectDrawCreate)GetProcAddress(LoadLibraryA("ddraw.dll"), "DirectDrawCreate");
	//DetourFunctionJmp((void*)pDirectDrawCreate, (void*)hDirectDrawCreate, (void**)&oDirectDrawCreate);

#ifdef PBSS_CLEANER
   while (!dwPBBase)
   {
      dwPBBase = (ADDRESS_PROC)GetModuleHandleA("pbcl.dll");
      if (!dwPBBase)
      {
         Sleep(500);
      }
   }

#ifdef PBSS_BLOCKER
	DetourFunction((void*)GetWindowThreadProcessId, (void*)hGetWindowThreadProcessId, (void**)&oGetWindowThreadProcessId);

	return;
#endif

	return;

#ifdef PB_LOGGER
   PBBypass();
#endif // PB_LOGGER

   ADDRESS_PROC pb_encryptAddressTableat = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\xB8\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x33\xC8\x89\x0D\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x33\xC8", "x????x????xxxx????x????xx");

   if (pb_encryptAddressTableat == NULL)
   {
      Log("Anti-PBSS not loaded!\n");
      MessageBoxA(GetForegroundWindow(), "Anti-PBSS not loaded!", "Error", 0);
      ExitProcess(0);
   }

   pb_xorKey = *(DWORD *)(pb_encryptAddressTableat + 1);

#ifdef PB_LOGGER
   Logf("PBKey = 0x%08X\n", pb_xorKey);
#endif // PB_LOGGER

   ADDRESS_PROC pb_screenshotForServerat = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x55\x8B\xEC\x83\xEC\x0C\x53\x33\xDB\x39\x1D\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8D\x88\x00\x00\x00\x00\x8D\x90", "xxxxxxxxxxx????xx????x????xx????xx");

   if (!pb_screenshotForServerat)
   {
      Log("Anti-PBSS not loaded!\n");
      MessageBoxA(GetForegroundWindow(), "Anti-PBSS not loaded!", "Error", 0);
      ExitProcess(0);
   }

   ADDRESS_PROC pb_screenshotForMeat = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x81\xEC\x00\x00\x00\x00\x53\x56\x57\x33\xDB\x33\xF6\x33\xC9\x33\xFF\x8D\x81\x00\x00\x00\x00\x8B\x10", "x????x????xx????xxxxxxxxxxxxx????xx");

   BYTE pb_remoteScreenshot[] = { 0xB9, 0, 0, 0, 0 };
   *(DWORD *)&pb_remoteScreenshot[1] = pb_screenshotForServerat;

   BYTE pb_localScreenshot[] = { 0xB9, 0, 0, 0, 0 };
   *(DWORD *)&pb_localScreenshot[1] = pb_screenshotForMeat;

   ADDRESS_PROC *pp_pb_remoteScreenShot = *(ADDRESS_PROC **)(FindPattern(pb_encryptAddressTableat, dwPBSize - (pb_encryptAddressTableat - dwPBBase), pb_remoteScreenshot, "xxxxx", FALSE) + 9);
   ADDRESS_PROC *pp_pb_localScreenShot  = *(ADDRESS_PROC **)(FindPattern(pb_encryptAddressTableat, dwPBSize - (pb_encryptAddressTableat - dwPBBase), pb_localScreenshot, "xxxxx") + 9);

   //MF(FindPattern);

   while (1)
   {
      ADDRESS_PROC tmpAddr = *pp_pb_remoteScreenShot ^ pb_xorKey;

      if (tmpAddr != (ADDRESS_PROC)h_pb_remoteScreenShot)
      {
         op_pb_remoteScreenShot  = tmpAddr;
         *pp_pb_remoteScreenShot = ((ADDRESS_PROC)h_pb_remoteScreenShot) ^ pb_xorKey;
#ifdef PB_LOGGER
         Logf("pp_pb_remoteScreenShot(%08X -> %08X) hooked from %08X to %08X\n", pp_pb_remoteScreenShot, pb_screenshotForServerat, tmpAddr, (ADDRESS_PROC)h_pb_remoteScreenShot);
#endif   // PB_LOGGER
      }

      tmpAddr = *pp_pb_localScreenShot ^ pb_xorKey;

      if (tmpAddr != (ADDRESS_PROC)h_pb_localScreenShot)
      {
         op_pb_localScreenShot  = tmpAddr;
         *pp_pb_localScreenShot = ((ADDRESS_PROC)h_pb_localScreenShot) ^ pb_xorKey;
#ifdef PB_LOGGER
         Logf("pp_pb_localScreenShot(%08X -> %08X) hooked from %08X to %08X\n", pp_pb_localScreenShot, tmpAddr, pb_screenshotForMeat, (ADDRESS_PROC)h_pb_localScreenShot);
#endif   // PB_LOGGER
      }

      Sleep(50);
   }
#endif // PBSS_CLEANER
}


MFEND(void, AntiPBSSThread(LPVOID));

// -----------------------------------------------------------
//------STUPID

typedef int (__cdecl * t_uk_printfdata)(char *a1, int a2);
t_uk_printfdata o_uk_printfdata = NULL;

int __cdecl h_uk_printfdata(char *a1, int a2)
{
   int ret = o_uk_printfdata(a1, a2);

   Logf("uk_printfdata[%08X](\"%s\")\n", (DWORD)_ReturnAddress() - dwPBBase, a1);
   return(ret);
}


// TODO: change scan logging code.  make a LogScan function, that ensures that the same scan isn't logged twice.

int tret;
typedef int (__cdecl * t_pb___vsnprintf)(char *buffer, size_t count, const char *format, va_list argptr);
t_pb___vsnprintf o_pb___vsnprintf = 0;
ADDRESS_PROC     ra1, ra2_;
int h_pb___vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
   __asm
   {
      pushad
         pushf
   }
   //tret = _vsnprintf(buffer, count, format, argptr);
   tret = o_pb___vsnprintf(buffer, count, format, argptr);
   if (strlen(buffer) > 5)
   {
      ra1 = (ADDRESS_PROC)_ReturnAddress() - dwPBBase;
      if ((ra1 == 0x101A) || (ra1 == 0x1051))      // TODO: sig scan for these offsets
      {
         __asm
         {
            mov eax, [ebp + 32]
            sub eax, [dwPBBase]
            mov ra2_, eax
         }
         //Logf("pb___vsnprintf,%08X,%08X,?,\"%s\"\n", ra1, ra2_, buffer);
         Logf("%s\n", buffer);
      }
      else
      {
         //Logf("pb___vsnprintf,%08X,?,?,\"%s\"\n", ra1, buffer);
         Logf("%s\n", buffer);
      }
   }
   __asm
   {
      popf
         popad
   }
   return(tret);
}


char         *ossz;
void         *o_pb__strncpy;
ADDRESS_PROC ra2;
char *h_pb__strncpy(char *dst, char *src, size_t c)
{
   strncpy(dst, src, c);
   if (ossz != src)
   {
      if ((DWORD)_ReturnAddress() - dwPBBase != 0x1617)          // TODO: sig scan for this offset
      {
         Logf("pb_strncpy,%08X,?,?,\"%s\"\n", (ADDRESS_PROC)_ReturnAddress() - dwPBBase, dst);
      }
      else
      {
         __asm
         {
            mov eax, [ebp + 32]
            sub eax, [dwPBBase]
            mov ra2, eax
         }
         Logf("pb_strncpy,%08X,%08X,?,\"%s\"\n", (ADDRESS_PROC)_ReturnAddress() - dwPBBase, ra2, dst);
      }
   }
   return(dst);
}


char *ssz;
void *o_pb_scramblestring;
void _declspec(naked) h_pb_scramblestring()
{
   __asm
   {
      push ebp
      mov ebp, esp
      pushad
      pushf
      mov ssz, esi
   }
   if (((ADDRESS_PROC)ssz > 0x400000) && (strlen(ssz) > 0))
   {
      Logf("pb_scramblestring,?,?,?,\"%s\"\n", ssz);
   }
   __asm
   {
      popf
      popad
      pop ebp
         jmp [o_pb_scramblestring]
   }
}

void  *o_pb_copy_start;
DWORD peax;
char  *ssz_;
void _declspec(naked) h_pb_copy_start()
{
   //after pb__strcat hook, this will be only hit when _strcpy
   __asm
   {
      mov peax, eax
      mov eax, [esp + 12]
      mov ssz_, eax
      mov eax, peax
      pushad
          pushf
   }
   Logf("multiple_pb_funcs,?,?,?,\"%s\"\n", ssz_);
   //if(ssz_[0] == '<')
   //Logf("%s\n",ssz_);
   __asm
   {
      popf
      popad
         jmp [o_pb_copy_start]
   }
}

void *o_pb_Transform;
void _declspec(naked) h_pb_Transform(ADDRESS_PROC dwStart, ADDRESS_PROC dwSize)
{
   __asm
   {
      pushad
      pushf
      add dwSize, 0x3F
   }
   Logf("MD5Transform,?,?,?,%08X thru %08X\n", dwStart, dwStart + dwSize);
   __asm
   {
      sub dwSize, 0x3F
      popf
      popad
          jmp [o_pb_Transform]
   }
}

void *o_pb_modulescan;
char *ssz__;
void _declspec(naked) h_pb_modulescan()
{
   __asm
   {
      pushad
      pushf
      mov ssz__, ecx
   }
   Logf("pb_modulescan,?,?,?,\"%s\"\n", ssz__);
   __asm
   {
      popf
      pushad
         jmp [o_pb_modulescan]
   }
}

char *h_pb__strcat(char *dst, char *src)
{
   strcat(dst, src);
   Logf("pb__strcat,%08X,?,?,\"%s\"\n", (ADDRESS_PROC)_ReturnAddress() - dwPBBase);
   return(dst);
}


typedef int (__cdecl * t_pb_md5update)(ADDRESS_PROC, ADDRESS_PROC);
t_pb_md5update o_pb_md5update = 0;
ADDRESS_PROC   ra1_, ra2__, _oeax;
ADDRESS_PROC   p_pb_md5update, teax;
void _declspec(naked) h_pb_md5update(ADDRESS_PROC msg, ADDRESS_PROC len)
{
   _asm mov _oeax, eax
   _asm mov eax, [esp]
   _asm mov ra1_, eax
   _asm mov eax, _oeax
   _asm push ebp
   _asm mov ebp, esp
   PRESERVE
            ra1_ -= dwPBBase;

#if 0
   if(ra1_ == 0x2410) // TODO: sig scan for this offset
   {
        __asm
        {
                mov eax, [ebp+20]
                sub eax, [dwPBBase]
                mov ra2__, eax
        }
        Logf("pb_md5update,%08X,%08X,?,%08X thru %08X\n", ra1_, ra2__, msg, msg + len);
   }
   else if(ra1_ == 0x229F) // TODO: sig scan for this offset
   {
        __asm
        {
                mov eax, [ebp+84]
                sub eax, [dwPBBase]
                mov ra2__, eax
        }
        Logf("pb_md5update,%08X,%08X,?,%08X thru %08X\n", ra1_, ra2__, msg, msg + len);
   }
   else
#endif
   Logf("pb_md5update,%08X,?,?,%08X thru %08X\n", ra1_, msg, msg + len);
   _asm pop ebp
   RESTORE
   _asm jmp [o_pb_md5update]


#if 0
   __asm
   {
        //ecx = mdContext
        push ebp
        mov ebp, esp
        pushad
        pushf
        push ecx
   }
   
   RetourFunction((void*)p_pb_md5update, (void*)o_pb_md5update);
   
   _asm pop ecx
   o_pb_md5update(msg,len);
   _asm mov teax, eax
   
   DetourFunctionJmp((void*)p_pb_md5update, (void*)h_pb_md5update, (void*)o_pb_md5update);
   
   __asm
   {
        popf
        popad
        pop ebp
        mov eax, teax
        ret
   }
#endif
}


void PBBypass()
{
   Log("\n---PB Logger initiating---\n");

   ADDRESS_PROC p_pb___vsnprintf    = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x55\x8B\xEC\x83\xEC\x20\x8B\x45\x0C\x56\x8B\x75\x08\x57\xFF\x75\x14\x89\x45\xE4\xFF\x75\x10\x8D\x45\xE0\x50\xC7\x45\x00\x00\x00\x00\x00\x89\x75\xE8\x89\x75\xE0\xE8\x00\x00\x00\x00\x83\xC4\x0C\x85\xF6\x8B\xF8", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx?????xxxxxxx????xxxxxxx");                                                                                                                                                                                                                                                                     //
   ADDRESS_PROC p_pb__strncpy       = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x8B\x4C\x24\x0C\x57\x85\xC9\x0F\x84\x00\x00\x00\x00\x56\x53\x8B\xD9\x8B\x74\x24\x14\xF7\xC6\x00\x00\x00\x00\x8B\x7C\x24\x10\x75\x0B\xC1\xE9\x02\x0F\x85\x00\x00\x00\x00\xEB\x27\x8A\x06\x83\xC6\x01\x88\x07\x83\xC7\x01\x83\xE9\x01", "xxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxxx");                                                                                                                                                                                                                                            //
   ADDRESS_PROC p_pb_scramblestring = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x53\x56\x8B\xF1\x57\xC6\x06\x00\x33\xFF\x8B\xDE\x33\xC0\x8A\x84\x3E\x00\x00\x00\x00\x53\x50\xE8\x00\x00\x00\x00\x47\x43\x59\x43\x83\xFF\x10\x59\x7C\xE6\x5F\x8B\xC6\x5E\x5B\xC3", "xxxxxxxxxxxxxxxxx????xxx????xxxxxxxxxxxxxxxx");                                                                                                                                                                                                                                                                                                             // "
   ADDRESS_PROC p_pb_copy_start     = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x8B\x4C\x24\x0C\xF7\xC1\x00\x00\x00\x00\x74\x1D\x8A\x11\x83\xC1\x01\x84\xD2\x74\x66\x88\x17\x83\xC7\x01\xF7\xC1\x00\x00\x00\x00\x75\xEA\xEB\x05\x89\x17\x83\xC7\x04\xBA\x00\x00\x00\x00\x8B\x01\x03\xD0\x83\xF0\xFF\x33\xC2\x8B\x11\x83\xC1\x04\xA9\x00\x00\x00\x00\x74\xE1\x84\xD2\x74\x34\x84\xF6\x74\x27\xF7\xC2\x00\x00\x00\x00\x74\x12\xF7\xC2\x00\x00\x00\x00\x74\x02\xEB\xC7\x89\x17\x8B\x44\x24\x08\x5F\xC3", "xxxxxx????xxxxxxxxxxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxxxxx????xxxxxxxxxxxx????xxxx????xxxxxxxxxxxx");                //
   ADDRESS_PROC p_pb_Transform      = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x55\x8B\xEC\x83\xEC\x3C\x8B\x4D\x08\x8B\x45\x0C\x53\x8B\x59\x04\x56\x8B\x71\x0C\x57\x8B\x11\x8B\xFB\xF7\xD7\x23\xFE\x8B\x71\x08\x23\xF3\x0B\xFE\x03\x38", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");                                                                                                                                                                                                                                                                                                                                           //
   ADDRESS_PROC p_pb_modulescan     = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x55\x8B\xEC\x83\xEC\x48\xA1\x00\x00\x00\x00\x56\x8D\x90\x00\x00\x00\x00\x8D\xB0\x00\x00\x00\x00\x33\xF2\x8D\x88\x00\x00\x00\x00\x33\x0C\xB5\x00\x00\x00\x00\x57\x33\xCA\x83\x3D\x00\x00\x00\x00\x00\x89\x4D\xF8\x75\x62\x8D\x90\x00\x00\x00\x00\x8D\xB0\x00\x00\x00\x00\x33\xF2\x8D\xB8\x00\x00\x00\x00\x33\x3C\xB5\x00\x00\x00\x00\x6A\x00\xBE\x00\x00\x00\x00\x56\x56\xC6\x05\x00\x00\x00\x00\x00\xC6\x05\x00\x00\x00\x00\x00", "xxxxxxx????xxx????xx????xxxx????xxx????xxxxx?????xxxxxxx????xx????xxxx????xxx????xxx????xxxx?????xx?????"); //
   p_pb_md5update = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x55\x8B\xEC\x83\xEC\x40\x56\x8B\xF1\x8B\x4E\x64\x57\x8B\x7D\x0C\x8B\xC1\xC1\xE8\x03\x8D\x14\xF9\x83\xE0\x3F\x3B\xD1\x73\x03\xFF\x46\x68\x8B\xCF\xC1\xE9\x1D\x01\x4E\x68\x85\xFF\x89\x56\x64\x74\x56\x89\x7D\x0C\x8B\x7D\x08\x53\x8A\x0F", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");                                                                                                                                                                                                                                                         //
   ADDRESS_PROC p_pb__strcat = FindPattern(dwPBBase, dwPBSize, (BYTE *)"\x8B\x4C\x24\x04\x57\xF7\xC1\x00\x00\x00\x00\x74\x13\x8A\x01\x83\xC1\x01\x84\xC0\x74\x3D\xF7\xC1\x00\x00\x00\x00\x75\xEF\x8B\xFF\x8B\x01\xBA\x00\x00\x00\x00\x03\xD0\x83\xF0\xFF\x33\xC2\x83\xC1\x04\xA9\x00\x00\x00\x00\x74\xE8\x8B\x41\xFC\x84\xC0\x74\x23\x84\xE4\x74\x1A\xA9\x00\x00\x00\x00\x74\x0E\xA9\x00\x00\x00\x00\x74\x02\xEB\xCD\x8D\x79\xFF\xEB\x0D\x8D\x79\xFE\xEB\x08", "xxxxxxx????xxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxx????xxx????xxxxxxxxxxxxxx");

   //o_pb___vsnprintf = (t_pb___vsnprintf)DETOURFUNCJ(p_pb___vsnprintf, h_pb___vsnprintf);
   o_pb__strncpy = DETOURFUNCJ(p_pb__strncpy, h_pb__strncpy);
   //o_pb_scramblestring = DETOURFUNCJ(p_pb_scramblestring, h_pb_scramblestring);
   //o_pb_copy_start = DETOURFUNCJ(p_pb_copy_start, h_pb_copy_start);
   //o_pb_Transform = DETOURFUNCJ(p_pb_Transform, h_pb_Transform);
   //o_pb_modulescan = DETOURFUNCJ(p_pb_modulescan, h_pb_modulescan);
   //o_pb_md5update = (t_pb_md5update)DETOURFUNCJ((void *)p_pb_md5update, (void *)h_pb_md5update);

   //Logf("(%08X)pb___vsnprintf     -> %08X\n", p_pb___vsnprintf, h_pb___vsnprintf);
   //Logf("(%08X)pb__strncpy        -> %08X\n", p_pb__strncpy, h_pb__strncpy);
   //Logf("(%08X)pb_scramblestring  -> %08X\n",p_pb_scramblestring,h_pb_scramblestring); <- weird, weird arguments or something that makes it difficult to log, not to mention there are easier functions to hook and the data passed to this function is already caught in other hooks
   //Logf("(%08X)pb_copy_start      -> %08X\n", p_pb_copy_start, h_pb_copy_start);
   //Logf("(%08X)pb_Transform       -> %08X\n",p_pb_Transform,h_pb_Transform); <- hooking other ones are better
   //Logf("(%08X)pb_modulescan      -> %08X\n",p_pb_modulescan,h_pb_modulescan); <- never called
   //Logf("(%08X)pb_md5update       -> %08X\n", p_pb_md5update, h_pb_md5update);

   //Log("Signateurs...\n");
   //Log("\nfuncname,ra1,ra2,ra3,data\n");
}

#endif
