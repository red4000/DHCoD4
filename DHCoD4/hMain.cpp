// TODO: try to exit threads if the hooking has been completed.
// TODO: make a condition to break the TimerThread loop for X_VERSION so that it can get VMProtected
// TODO: fix normal version(aka non X_VERSION)
// TODO: create a hAbstract.h/.cpp and create abstract CEntity/etc classes and pointers etc.  and create methods for filling these things with game functions etc... maybe create multiple hAbstract files, like hAbstractFunctions, hAbstractClasses, hAbstractVariables

#pragma warning ( disable : 4530 )
#include <windows.h>
#include <tlhelp32.h>
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hConfig.h"
#include "hBot.h"
#include "hAim.h"
#include "hChams.h"
#include "hObituary.h"
#include "hDetour.h"
#include "hDefines.h"
#include "hAntiPB.h"
#include "CCloak.h"
#include "hRegistry.h"
#include "hLog.h"
#include "hDefineWarnings.h"
#include "hAPI.h"
#include "hDebug.h"
#include "hAutoWall.h"

#include "xde.h"


#include "VMProtectSDK.h"

#pragma comment(lib, "User32.lib")

#define SAFE_EXIT(x)                         \
   TerminateProcess(GetCurrentProcess(), x); \
   ExitProcess(x);                           \
   abort();                                  \
   exit(x);                                  \
   _exit(x)

#define BUFSIZE    256

extern DWORD dwRenderScene, dwObituary, dwPredictPlayerState, dwSendCommandToConsole, dwWritePacket;
extern void  *dwpECSRenderSceneReturnAddress;

MFUNC(BOOL WINAPI, DllMain(HMODULE hinstModule, DWORD dwReason, LPVOID lpvReserved));

typedef void (__cdecl * tWritePacket)();
typedef void (__fastcall * tRenderFrame)();
typedef void (*tCG_SendCommandToConsole)(int, int, char *);
typedef bool (__stdcall * tGetThreadContext)(HANDLE, LPCONTEXT);
typedef void (WINAPI * tEnterCriticalSection)(LPCRITICAL_SECTION);
typedef void (*tCG_PredictPlayerState)(int);

tWritePacket              oWritePacket              = NULL;
tRenderFrame              oRenderFrame              = NULL;
DWORD                     oRenderScene              = NULL;
tGetThreadContext         oGetThreadContext         = NULL;
tEnterCriticalSection     oEnterCriticalSection     = NULL;
tCG_SendCommandToConsole  oCG_SendCommandToConsole  = NULL;
tCG_PredictPlayerState    oCG_PredictPlayerState    = NULL;
DWORD                     or_logfile    = NULL;
DWORD                     ocg_drawFPS   = NULL;
DWORD                     ocl_packetdup = NULL;
DWORD                     nr_logfile    = NULL;
DWORD                     ncg_drawFPS   = NULL;
DWORD                     ncl_packetdup = NULL;

DWORD dwSafetyTime = 0;

#ifdef GHOST_CVARS
char tmpCvar[512], tmpCvar2[256], tmpCvar3[256], tmpCvar4[256], szClass[256] = "heavygunner_mp,0";
#endif // GHOST_CVARS

char ConfigPath[MAX_PATH];
char HackPath[MAX_PATH];

BOOL           bRet;
BOOL           FlyCamEnabled = FALSE;
bool           bFwd          = false, bBwd = false, bLft = false, bRgt = false, bRemoved = false, bIgnoreNextCmd = false;
Vector3<_prec> FlyCamPos, FlyCamDelta, tmpAngles, OldViewOrigin;
CRefdef        *hRefdef;
DWORD          pbBaseAddress;
DWORD          RetAddr;
MODULEENTRY32  _lpme;
DWORD          _PID;
char           _chModName[256];
HANDLE         _hSnapshotModule;
BOOL           _isMod;
DWORD          _dwSize;

int classMenuID, teamMenuID;

HMODULE hHackModule;

void hCG_PredictPlayerState(int a1) // TODO: move to hGameHooks
{
   //PRESERVE

   ANTI_CRASH_BEGIN
   FirstBulletFix();
   ANTI_CRASH_END

   oCG_PredictPlayerState(a1);

   ANTI_CRASH_BEGIN
   FirstBulletFix();

   TouchUserCmds();
   ANTI_CRASH_END

   //RESTORE
}


void hCG_SendCommandToConsole(int x1, int x2, char *szString) // TODO: move to hGameHooks
{
#ifdef GHOST_CVARS
   ANTI_CRASH_BEGIN
   //PRESERVE

   // Anti Death
   if (bIgnoreNextCmd)     // set to true in antideath to make the hook ignore antideath cmd
   {
      bIgnoreNextCmd = false;
      goto xReturn;
   }

   // New map is loaded (can happen with other events too!)
   if (strstr(szString, "selectStringTableEntryInDvar"))
   {
      ResetStats();
   }

#ifndef X_VERSION
   if (cvar_FlyCam && cvar_FlyCam->iValue == 1)
   {
      if (!strcmp(szString, "fcFwd"))
      {
         bBwd = false;
         bFwd = !bFwd;
         //RESTORE
         return;
      }

      else if (!strcmp(szString, "fcBwd"))
      {
         bFwd = false;
         bBwd = !bBwd;
         //RESTORE
         return;
      }

      else if (!strcmp(szString, "fcLft"))
      {
         bRgt = false;
         bLft = !bLft;
         //RESTORE
         return;
      }

      else if (!strcmp(szString, "fcRgt"))
      {
         bLft = false;
         bRgt = !bRgt;
         //RESTORE
         return;
      }
   }
#endif // X_VERSION

   if (cvar_GhostCvars->iValue == 1)
   {
      strcpy_s(tmpCvar, szString);
      UpToLow(tmpCvar);
      char *split = strstr(tmpCvar, "bot_");

      if (!strcmp(tmpCvar, "bot_list_secret_cvars"))
      {
         for (int i = 0; i < Mngr.Count; i++)
         {
            if (Mngr.Vars[i]->Type == Completely_Invisible)
            {
               sprintf_s(tmpCvar2, "^2Secret ^5Ghost CVAR %s=%f\n", Mngr.Vars[i]->Name, Mngr.Vars[i]->GetFValue());
               CG_EchoToConsolef(CONSOLE, tmpCvar2);
            }
         }

         //RESTORE
         return;
      }

      if (!strcmp(tmpCvar, "bot_list_cvars"))
      {
         for (int i = 0; i < Mngr.Count; i++)
         {
            if (Mngr.Vars[i]->Type != Completely_Invisible)
            {
               strcpy_s(tmpCvar4, Mngr.Vars[i]->Name);
               UpToLow(tmpCvar4);
               SpaceToUnderscore(tmpCvar4);
               sprintf_s(tmpCvar2, "^3Ghost ^5CVAR bot_%s=%f\n", tmpCvar4, Mngr.Vars[i]->GetFValue());
               CG_EchoToConsolef(CONSOLE, tmpCvar2);
            }
         }

         //RESTORE
         return;
      }

      if (split)
      {
         split += 4;

         for (int i = 0; i < Mngr.Count; i++)
         {
            strcpy_s(tmpCvar2, Mngr.Vars[i]->Name);
            UpToLow(tmpCvar2);
            strcpy_s(tmpCvar3, tmpCvar2);
            SpaceToUnderscore(tmpCvar3);

            if (!strncmp(split, tmpCvar2, strlen(Mngr.Vars[i]->Name) + 1) || !strncmp(split, tmpCvar3, strlen(Mngr.Vars[i]->Name) + 1))
            {
               Mngr.Vars[i]->Decrement();

			   // this block was commented out... beware
               split += strlen(Mngr.Vars[i]->Name) + 1;
               if(Mngr.Vars[i]->bInt)
                    Mngr.Vars[i]->iValue = (int)atof(split);
               else
                    Mngr.Vars[i]->Value = (float)atof(split);
               
#ifdef X_VERSION
               sprintf_s(tmpCvar, "Ghost CVAR \"%s\" was toggled to %.2f\n", Mngr.Vars[i]->Name, Mngr.Vars[i]->GetFValue());
#else
               sprintf_s(tmpCvar, "Ghost CVAR \"%s\" was toggled to %i\n", Mngr.Vars[i]->Name, Mngr.Vars[i]->GetIValue());
#endif
               CG_EchoToConsolef(CONSOLE, tmpCvar);
               //RESTORE
               return;
            }

            // TODO: FIXME: you can't change cvars like bot_crosshair_shadow 0 because it detects bot_crosshair first.
            if (!strncmp(split, tmpCvar2, strlen(Mngr.Vars[i]->Name)) || !strncmp(split, tmpCvar3, strlen(Mngr.Vars[i]->Name)) && ((split[strlen(Mngr.Vars[i]->Name)] == ' ') || (split[strlen(Mngr.Vars[i]->Name)] == '=')))
            {
               split += strlen(Mngr.Vars[i]->Name) + 1;
               if (Mngr.Vars[i]->bInt)
               {
                  Mngr.Vars[i]->iValue = (int)atof(split);
               }
               else
               {
                  Mngr.Vars[i]->Value = (float)atof(split);
               }
#ifdef X_VERSION
               sprintf_s(tmpCvar, "Ghost CVAR \"%s\" was toggled to %.2f\n", Mngr.Vars[i]->Name, Mngr.Vars[i]->GetFValue());
#else
               sprintf_s(tmpCvar, "Ghost CVAR \"%s\" was toggled to %i\n", Mngr.Vars[i]->Name, Mngr.Vars[i]->GetIValue());
#endif
               CG_EchoToConsolef(CONSOLE, tmpCvar);
               //RESTORE
               return;
            }
         }
      }
   }

   // Antideath
   if (strstr(szString, "mr"))
   {
      if (strstr(szString, "autoassign") || strstr(szString, "allies") || strstr(szString, "axis"))
      {
         sscanf_s(szString, "%*s %*s %*i %i %*s", &teamMenuID);
      }

      else if (strstr(szString, "assault_mp"))
      {
         strcpy_s(szClass, "assault_mp,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "specops_mp"))
      {
         strcpy_s(szClass, "specops_mp,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "heavygunner_mp"))
      {
         strcpy_s(szClass, "heavygunner_mp,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "demolitions_mp"))
      {
         strcpy_s(szClass, "demolitions_mp,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "sniper_mp"))
      {
         strcpy_s(szClass, "sniper_mp,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "custom1"))
      {
         strcpy_s(szClass, "custom1,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "custom2"))
      {
         strcpy_s(szClass, "custom2,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "custom3"))
      {
         strcpy_s(szClass, "custom3,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "custom4"))
      {
         strcpy_s(szClass, "custom4,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }

      else if (strstr(szString, "custom5"))
      {
         strcpy_s(szClass, "custom5,0");
         sscanf_s(szString, "%*s %*s %*i %i %*s", &classMenuID);
      }
   }
   ANTI_CRASH_END

xReturn:

   //RESTORE
#endif
   oCG_SendCommandToConsole(x1, x2, szString);
}


void doFlycam() // TODO: move to hGameHooks
{
   if (cvar_FlyCam && cvar_FlyCam->iValue == 1)
   {
      if (FlyCamEnabled == FALSE)          // first flycam frame - setup flycam
      {
         CG_ConsoleCommand("bind W \"fcFwd\"");
         CG_ConsoleCommand("bind A \"fcLft\"");
         CG_ConsoleCommand("bind S \"fcBwd\"");
         CG_ConsoleCommand("bind D \"fcRgt\"");
         CG_ConsoleCommand("cg_drawgun 0");
         FlyCamPos     = hRefdef->vOrigin;
         OldViewOrigin = FlyCamPos;
         FlyCamEnabled = TRUE;
      }

      hRefdef->vOrigin = FlyCamPos;
      //Refdef->vOrigin = FlyCamPos;
   }

   else
   {
      if (FlyCamEnabled == TRUE)          // last flycam frame - reset things
      {
         CG_ConsoleCommand("bind W \"+forward\"");
         CG_ConsoleCommand("bind A \"+moveleft\"");
         CG_ConsoleCommand("bind S \"+back\"");
         CG_ConsoleCommand("bind D \"+moveright\"");
         CG_ConsoleCommand("cg_drawgun 1");
         FlyCamEnabled = FALSE;
      }
   }
}


__declspec(naked) void hRenderScene() // TODO: move to hGameHooks
{
   PRESERVE

      __asm
   {
      mov hRefdef, eax
   }

   doFlycam();

   RESTORE

      __asm
   {
      jmp [oRenderScene]
   }
}

DWORD dword_10016F58;

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *pExceptionInfo)
{
#ifdef EXCEPTION_LOGGER
#ifdef NORMAL_VERSION
   if (pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP)
   {
#endif // NORMAL_VERSION
#ifdef X_VERSION
   if ((pExceptionInfo->ContextRecord->Eip != pcg_drawFPS_eip1) && (pExceptionInfo->ContextRecord->Eip != pcg_drawFPS_eip2) && (pExceptionInfo->ContextRecord->Eip != pcl_packetdup_eip))
   {
#endif // X_VERSION
   CrashHandler(pExceptionInfo);

#ifdef X_VERSION
}


#endif // X_VERSION
#ifdef NORMAL_VERSION
}
#endif // NORMAL_VERSION
#endif // EXCEPTION_LOGGER


#ifndef X_VERSION
   if ((DWORD)pExceptionInfo->ContextRecord->Eip == 0x4A69BC)
   {
	   pExceptionInfo->ContextRecord->Ecx = dwRestore;
	   return EXCEPTION_CONTINUE_EXECUTION;
   }

   if ((DWORD)pExceptionInfo->ContextRecord->Eip == bulletHit_eip)
   {
	   if (*(DWORD*)pExceptionInfo->ContextRecord->Esp != 0x4560FD)
	   {
		   entNum = pExceptionInfo->ContextRecord->Eax;
		   Vector3<float> *end = *(Vector3<float>**)(pExceptionInfo->ContextRecord->Esp + 16);
		   endPos = *end;

		   if (entNum == traceEnt->ClientNumber)
		   {
			   hitTarget = true;
		   }

		   tmp = endPos;
		   tmp -= ViewOrigin;

		   if (tmp.Length() >= length)
		   {
			   hitTarget = true;
		   }
	   }

	   pExceptionInfo->ContextRecord->Eip = 0x455A7A;
	   return EXCEPTION_CONTINUE_EXECUTION;
   }

   if (pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP)
   {
      return(EXCEPTION_CONTINUE_SEARCH);
   }

   if ((DWORD)pExceptionInfo->ContextRecord->Eip == dwRenderScene)
   {
      pExceptionInfo->ContextRecord->Eip = (DWORD)hRenderScene;
      return(EXCEPTION_CONTINUE_EXECUTION);
   }

   if ((DWORD)pExceptionInfo->ContextRecord->Eip == dwWritePacket)
   {
      ANTI_CRASH_BEGIN
      FirstBulletFix();
      TouchUserCmds();
      ANTI_CRASH_END
      pExceptionInfo->ContextRecord->Eip = (DWORD)oWritePacket;
      return(EXCEPTION_CONTINUE_EXECUTION);
   }

   if ((DWORD)pExceptionInfo->ContextRecord->Eip == dwPredictPlayerState)
   {
      pExceptionInfo->ContextRecord->Eip = (DWORD)hCG_PredictPlayerState;
      return(EXCEPTION_CONTINUE_EXECUTION);
   }

   if ((DWORD)pExceptionInfo->ContextRecord->Eip == dwSendCommandToConsole)
   {
      pExceptionInfo->ContextRecord->Eip = (DWORD)hCG_SendCommandToConsole;
      return(EXCEPTION_CONTINUE_EXECUTION);
   }

   if ((DWORD)pExceptionInfo->ContextRecord->Eip == dwObituary)
   {
      ANTI_CRASH_BEGIN
      hCG_Obituary((CEntityState *)pExceptionInfo->ContextRecord->Eax);
      ANTI_CRASH_END
      pExceptionInfo->ContextRecord->Eip = (DWORD)oCG_Obituary;
      return(EXCEPTION_CONTINUE_EXECUTION);
   }
#else
   switch (pExceptionInfo->ExceptionRecord->ExceptionCode)
   {
   case EXCEPTION_ACCESS_VIOLATION:
	   pLocalEnt    = GetEntityByIndex(cg->clientNum);
	   pLocalClient = GetClientByIndex(cg->clientNum);
      switch (pExceptionInfo->ContextRecord->Eip)
      {
#ifdef VISUALS
      case pcg_drawFPS_eip1:
         pExceptionInfo->ContextRecord->Eax = ocg_drawFPS;
         return(EXCEPTION_CONTINUE_EXECUTION);

      case pcg_drawFPS_eip2:
         pExceptionInfo->ContextRecord->Ecx = ocg_drawFPS;
         ANTI_CRASH_BEGIN
         Frame();
         ANTI_CRASH_END
         return(EXCEPTION_CONTINUE_EXECUTION);

#if 0
      case 0x00613F04:                   // stupid crash fix, this case is probably incorrect.
         pExceptionInfo->ContextRecord->Esi = ocg_drawFPS;
         return(EXCEPTION_CONTINUE_EXECUTION);
#endif
#endif   // VISUALS
      case pcl_packetdup_eip:
         pExceptionInfo->ContextRecord->Ecx = ocl_packetdup;
         ANTI_CRASH_BEGIN
         FirstBulletFix();
         TouchUserCmds();
         ANTI_CRASH_END
         return(EXCEPTION_CONTINUE_EXECUTION);

		/*
		case 0x00435CF8:
			ex->ContextRecord->Eax = ocg_debugevents;
			return EXCEPTION_CONTINUE_EXECUTION;

		case 0x00435D5D:
		case 0x00435D82:
			ex->ContextRecord->Ecx = ocg_debugevents;
			return EXCEPTION_CONTINUE_EXECUTION;

		*/

	  case 0x4A69BC:
		  pExceptionInfo->ContextRecord->Ecx = dwRestore;
		  return EXCEPTION_CONTINUE_EXECUTION;

	  case bulletHit_eip:
		  if (*(DWORD*)pExceptionInfo->ContextRecord->Esp != 0x4560FD)
		  {
			  entNum = pExceptionInfo->ContextRecord->Eax;
			  Vector3<float> *end = *(Vector3<float>**)(pExceptionInfo->ContextRecord->Esp + 16);
			  endPos = *end;

			  if (entNum == traceEnt->ClientNumber)
			  {
				  hitTarget = true;
			  }

			  tmp = endPos;
			  tmp -= ViewOrigin;

			  if (tmp.Length() >= length)
			  {
				  hitTarget = true;
			  }
		  }

		  pExceptionInfo->ContextRecord->Eip = 0x455A7A;
		  return EXCEPTION_CONTINUE_EXECUTION;
      }

      break;
   }
#endif // X_VERSION

#ifdef ANTI_CRASH
   pExceptionInfo->ContextRecord->Eip += xde((unsigned char *)pExceptionInfo->ContextRecord->Eip);
   return(EXCEPTION_CONTINUE_EXECUTION);

#else
   return(EXCEPTION_CONTINUE_SEARCH);
#endif
}

CONTEXT ctx;
void Set_SEH_and_BreakPoints()
{
   ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
   ctx.Dr6          = 0x00000000;
   // TODO: randomize what hooks are placed in what debug registers.
#ifdef VISUALS
   ctx.Dr0 = dwRenderScene;
#endif // VISUALS
   if (cvar_UseAltHook->iValue == 0)
   {
      ctx.Dr1 = dwWritePacket;
   }
   else
   {
      ctx.Dr1 = dwPredictPlayerState;
   }
   ctx.Dr2 = dwObituary;
   ctx.Dr3 = dwSendCommandToConsole;
   ctx.Dr7 = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6);

   SetThreadContext(GetCurrentThread(), &ctx);
}


CInline BOOL AreHWBPsSet()
{
   ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

   if (oGetThreadContext != NULL)
   {
      oGetThreadContext(GetCurrentThread(), &ctx);
   }
   else
   {
      GetThreadContext(GetCurrentThread(), &ctx);
   }

   if ((ctx.Dr0 != dwRenderScene) ||
       ((ctx.Dr1 != dwWritePacket) && (ctx.Dr1 != dwPredictPlayerState)) ||
       (ctx.Dr2 != dwObituary) ||
       (ctx.Dr3 != dwSendCommandToConsole))
   {
      return(FALSE);
   }

   return(TRUE);
}


void hEnterCriticalSection();


void __declspec(naked) safe_hEnterCriticalSection()
{
   __asm
   {
      cmp dword ptr [esp], 0x5FA369
      je hEnterCriticalSection
      jmp [oEnterCriticalSection]
   }
}


void __declspec(naked) hEnterCriticalSection() // TODO: move to hGameHooks
{
   __asm
   {
      pushf
      pusha
   }
#ifdef NORMAL_VERSION
   if (!AreHWBPsSet())
   {
      Set_SEH_and_BreakPoints();
   }
#endif
   Frame();
   __asm
   {
      popa
      popf
      jmp [oEnterCriticalSection]
   }
}


BOOL WINAPI hGetThreadContext(HANDLE hThread, LPCONTEXT lpContext)
{
   BOOL ret = oGetThreadContext(hThread, lpContext);

   if (lpContext->ContextFlags & CONTEXT_DEBUG_REGISTERS)
   {
      lpContext->Dr1 = 0;
      lpContext->Dr2 = 0;
      lpContext->Dr6 = 0;
      lpContext->Dr0 = 0;
      lpContext->Dr7 = 0;
      lpContext->Dr3 = 0;
   }

   return(ret);
}

char CfgPath[MAX_PATH];


DWORD WINAPI AutoSave(DWORD)
{
	while(1)
	{
		Sleep(5000);
		Mngr.Save(CfgPath);
	}
	return 0;
}


extern HANDLE hProcess;

MFUNC(DWORD, TimerThread(LPVOID));
MFBEG(DWORD, TimerThread(LPVOID));
DWORD TimerThread(LPVOID)
{
#ifdef EXCEPTION_LOGGER
   hProcess = GetCurrentProcess();
   PVOID hCrashHandler = AddVectoredExceptionHandler(1, CrashHandler);
#endif // EXCEPTION_LOGGER

   // TODO: write custom path retrievals for mnaually mapped version.  will include using a registry key, as seen below \/
#ifdef COMPILE_FOR_LOADER
#if 0
   strcpy(ConfigPath, ReadRegistryString("k"));

   appendTrailSlash(ConfigPath);

   if (inl_strlen(HackPath) < 3)
   {
      GetModuleFileNameA(hHackModule, ConfigPath, MAX_PATH);
      char *Slash = strrchr(ConfigPath, '\\');
      ConfigPath[Slash - ConfigPath + 1] = '\0';
   }
#endif
   GetModuleFileNameA(hHackModule, ConfigPath, MAX_PATH);
   char *Slash = strrchr(ConfigPath, '\\');
   if (Slash)
   {
      ConfigPath[Slash - ConfigPath + 1] = '\0';
   }
#else
   GetModuleFileNameA(hHackModule, ConfigPath, MAX_PATH);
   char *Slash = strrchr(ConfigPath, '\\');
   if (Slash)
   {
      ConfigPath[Slash - ConfigPath + 1] = '\0';
   }
#endif // COMPILE_FOR_LOADER
   strcpy(HackPath, ConfigPath);
   strcat_s(ConfigPath, CONFIG_FILENAME);

#ifdef LOG
   CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LogThread, NULL, NULL, NULL);
#endif // LOG

   SeedRandom();

   RegisterCvar();
   Mngr.Load(ConfigPath);
#if HVERSION != 3
#ifdef FORCE_INFOBAR
   cvar_InfoBar->iValue = 1; // force infobar on
#endif
   strcpy(CfgPath, ConfigPath);
   CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AutoSave, NULL, NULL, NULL);
#endif

   FORCE_RANDOMIZE_STR(CONFIG_FILENAME);
#ifndef VISUALS
   Mngr.RandomizeData();
   RANDOMIZE(ConfigPath, sizeof(ConfigPath));
   FORCE_RANDOMIZE_STR("Default"); // TODO: update this with the string encryption... may end up making custom crypt macroes for encrypted strings
   FORCE_RANDOMIZE_STR("Threat");
   FORCE_RANDOMIZE_STR("Distance");
   FORCE_RANDOMIZE_STR("Distance from crosshair");
   FORCE_RANDOMIZE_STR("Best Aimed");
#endif // VISUALS
#ifndef LOG
   FORCE_RANDOMIZE(HackPath, sizeof(HackPath));
#endif // LOG

#ifdef PBSS_CLEANER
   CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AntiPBSSThread, NULL, NULL, NULL);
#endif // PBSS_CLEANER
#ifdef CHAMS
   CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ChamThread, NULL, NULL, NULL);
#endif // CHAMS
#ifdef DISABLED3D9
   return 0;
#endif

#ifndef MANUAL_MAPPED
   CloakDll(hHackModule);      // TODO: call CloakDll even if manually mapped, only in hCloak, only unlink it if NOT manually mapped // TODO: rename CCloak to hCloak // TODO: rename CloakDll to ...
   MF(CloakDll);
#endif // MANUAL_MAPPED

   while (!cg->NextSnap)
   {
      Sleep(10);
   }

   for(size_t i=0;i<MAX_PLAYERS;i++)
   {
      ovel[i] = 0.0f;
   }

#ifdef EXCEPTION_LOGGER
   RemoveVectoredExceptionHandler(hCrashHandler);
#endif // EXCEPTION_LOGGER

   switch (GetOSType())
   {
   case OS_XP:
      SetUnhandledExceptionFilter(ExceptionHandler);
      break;

   default:
      AddVectoredExceptionHandler(1, ExceptionHandler);
      break;
   }

      if (VMProtectIsDebuggerPresent(TRUE))
      {
         SAFE_EXIT(0);
      }
#ifdef NORMAL_VERSION
      oRenderScene = (DWORD)CREATE_1INSTR_TRAMP((void *)dwRenderScene);

      oWritePacket = (tWritePacket)CREATE_1INSTR_TRAMP((void *)dwWritePacket);

      oCG_SendCommandToConsole = (tCG_SendCommandToConsole)CREATE_1INSTR_TRAMP((void *)dwSendCommandToConsole);

      oCG_PredictPlayerState = (tCG_PredictPlayerState)CREATE_1INSTR_TRAMP((void *)dwPredictPlayerState);

      oCG_Obituary = (tCG_Obituary)CREATE_1INSTR_TRAMP((void *)dwObituary);

      //DetourFunction((void*)GetThreadContext, (void*)hGetThreadContext, (void**)&oGetThreadContext);

      DetourFunction((void *)EnterCriticalSection, (void *)safe_hEnterCriticalSection, (void **)&oEnterCriticalSection);
#endif // NORMAL_VERSION
#ifdef X_VERSION
#ifdef VISUALS
      ncg_drawFPS = (DWORD)ALLOCATE_UNREADABLE_PAGE;
      if (*pcg_drawFPS != ncg_drawFPS)
      {
         ocg_drawFPS  = *pcg_drawFPS;
         *pcg_drawFPS = ncg_drawFPS;
      }
#endif // VISUALS
      ncl_packetdup = (DWORD)ALLOCATE_UNREADABLE_PAGE;
      if (*pcl_packetdup != ncl_packetdup)
      {
         ocl_packetdup  = *pcl_packetdup;
         *pcl_packetdup = ncl_packetdup;
      }
#endif // X_VERSION
#ifndef NORMAL_VERSION
#ifndef X_VERSION
      void *pFake = VirtualAlloc(0, 0x290000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
      memcpy(pFake, (void *)0x00401000, 0x290000);

      oRenderScene = (DWORD)DetourFunctionJmp((void *)dwRenderScene, hRenderScene);

      //oWritePacket = (tWritePacket)DetourFunction((void*)dwWritePacket);

      oCG_SendCommandToConsole = (tCG_SendCommandToConsole)DetourFunctionJmp((void *)dwSendCommandToConsole, hCG_SendCommandToConsole);

      oCG_PredictPlayerState = (tCG_PredictPlayerState)DetourFunctionJmp((void *)dwPredictPlayerState, hCG_PredictPlayerState);

      //oCG_Obituary = (tCG_Obituary)DetourFunction((void*)dwObituary);

      //oGetThreadContext = (tGetThreadContext)DetourFunction((void*)GetThreadContext, (void*)hGetThreadContext);

      oEnterCriticalSection = (tEnterCriticalSection)DetourFunction((void *)EnterCriticalSection, (void *)safe_hEnterCriticalSection);

      ADD_CLOAK(GetCurrentProcessId(), 0x4010000, 0x691000, pFake, MakePtr(void *, pFake, 0x691000));
#endif
#endif

   MFEX(TimerThread,1000);
   return(0);
}


MFEND(DWORD, TimerThread(LPVOID));

MFBEG(BOOL WINAPI, DllMain(HMODULE hinstModule, DWORD dwReason, LPVOID lpvReserved));
BOOL WINAPI DllMain(HMODULE hinstModule, DWORD dwReason, LPVOID lpvReserved)
{
   UNREFERENCED_PARAMETER(lpvReserved);

   if (VMProtectIsDebuggerPresent(TRUE))
   {
      SAFE_EXIT(0);
      return(TRUE);
   }

   if (dwReason == DLL_PROCESS_ATTACH)
   {
#ifdef COMPILE_FOR_LOADER
      HANDLE hSingleInstanceMutex = CreateMutexA(NULL, TRUE, FDS("b1720159a5617183a17cb253c20ef3fc"));           // TODO: move to hProtection
      DWORD  le = GetLastError();

      if (le != ERROR_ALREADY_EXISTS)
      {
         SAFE_EXIT(0);
         return(TRUE);
      }

#if 0
      if (!FindWindow(0, "Hack - Load Hack"))
      {
         SAFE_EXIT(0);
         return(TRUE);
      }
#endif
#endif // COMPILE_FOR_LOADER

      hHackModule = hinstModule;

#if 0
      SYSTEMTIME ast;
      GetLocalTime(&ast);

      if ((ast.wMonth != 5) || (ast.wYear != 2012) || (ast.wDay != 5))
      {
         MessageBoxA(GetForegroundWindow(), "this version has expired", "error", 0);
         SAFE_EXIT(0);
         return(TRUE);
      }
#endif

      CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)TimerThread, NULL, NULL, NULL);
   }
   return(TRUE);
}


MFEND(BOOL WINAPI, DllMain(HMODULE hinstModule, DWORD dwReason, LPVOID lpvReserved));
