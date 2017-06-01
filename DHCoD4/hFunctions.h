#ifndef HFUNCTIONS_H
#define HFUNCTIONS_H

#include "hConfig.h"
#include "hEsp.h"
#include "hData.h"
#include "hNewCore.h"

//--------------------------------------------------------------------------------------
//---------STATIC DEFINES---------------------------------------------------------------
//-----------these won't be changing any time soon--------------------------------------
//--------------------------------------------------------------------------------------


#define MakePtr(cast, ptr, addValue)       (cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))

#define SAFE_CVAR(cvar)                    if (cvar)
#define SAFE_CVAR_READ_DWORD(cvar, out)    if (cvar) (out) = (cvar)->dwVal
#define SAFE_CVAR_WRITE_DWORD(cvar, to)    if (cvar) (cvar)->dwVal = to
#define SAFE_CVAR_READ_FLOAT(cvar, out)    if (cvar) (out) = (cvar)->fVal
#define SAFE_CVAR_WRITE_FLOAT(cvar, to)    if (cvar) (cvar)->fVal = to

#define PLAY_SOUND(x)                      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hPlaySound, (x), 0, 0)

#define TEXT_HEIGHT    13.0f * cvar_EspFontScale->Value
#define BOX_RADIUS     16.0f

//--------------------------------------------------------------------------------------
//---------DYNAMIC DEFINES--------------------------------------------------------------
//-----------these most likely will change from various Q3 mods-------------------------
//--------------------------------------------------------------------------------------


#define GET_UCMD(x)    ((CUsercmd *)(pUserCmds + UserCmdSize * ((x) & CMD_BACKUP)))
#define GET_OCMD(x)    (&ocmds[(x) & CMD_BACKUP])

#define dwCEntities          0x0084F2D8
#define dwCClients           0x00839270
#define dwPpweapon           0x00736DB8
#define pUserCmds            0x00CC4FF8
#define dwWeaponInfo         0x00748658

#define CWeaponSize          0x44
#define ENTITY_SIZE          0x1DC
#define CLIENT_SIZE          0x4CC
#define UserCmdSize          0x20

//#define pSpreadMultiplier	0x00794A9C
#define pSpreadMultiplier    0x0084CAA0

#define pCurrentCmdNum       (DWORD *)0x00CC5FF8
#define NewServerTime        (int *)0x00794474
#define CurServerTime        (int *)0x00794464
#define PhysicsTime          (int *)0x00794460
#define pWeaponX             0x0079E76C
#define pWeaponY             0x0079E770

#define pcl_packetdup        (DWORD *)0x00C5F8D8
#define pcl_packetdup_eip    0x0046408A    // set ECX to [ocl_packetdup] when exception occurs at this eip
#define pcg_drawFPS          (DWORD *)0x00748634
// TODO: might as well remove these defines for EIP's, cos they will change from game to game and possibly version to version.
#define pcg_drawFPS_eip1     0x0042BB96    // set EAX to [ocg_drawFPS] when exception occurs at this eip
#define pcg_drawFPS_eip2     0x0042C029    // set ECX to [ocg_drawFPS] when exception occurs at this eip
#define pr_logfile           (DWORD *)0x0D5697F8
#define pcg_debugevents      (DWORD *)0x00455A80
#define psv_clientSideBullets (DWORD *)0x0185C434
#define bulletHit_eip        0x455A26


//--------------------------------------------------------------------------------------
//---------PREDECLARATIONS--------------------------------------------------------------
//--------------------------------------------------------------------------------------


extern char       pBuffer[1024];
extern DWORD GPTR dwRegisterFont;
extern DWORD GPTR dwRegisterShader;
extern DWORD GPTR dwConsoleCommand;
extern DWORD GPTR dwCG_EchoToConsolef;
extern DWORD GPTR dwBG_EvaluateTrajectory;
extern DWORD GPTR dwBG_EvaluateTrajectoryDelta;
extern DWORD GPTR dwGetRandomFloats;
extern DWORD GPTR dwDrawGlowTextLng;
extern DWORD GPTR dwCG_DvarCreate;
extern DWORD GPTR dwDrawNameTag;
extern DWORD GPTR dwRegisterTag;
extern DWORD GPTR dwDrawStretchPic;
extern DWORD GPTR dwDrawStringEngine;
extern DWORD GPTR dwDrawRotatedPic2;
extern DWORD GPTR dwPscreenMatrix;
extern DWORD GPTR dwCom_Frame;
extern DWORD GPTR dwCG_GetPlayerViewPoint;
extern DWORD GPTR dwGetPlayerTag;
extern DWORD GPTR dwGetPlayerTagAccurate;
extern DWORD GPTR dwCG_Trace;
extern DWORD GPTR dwDrawBulletTrajectory;
extern DWORD GPTR dwCG_GetPlayerViewPoint;
extern DWORD GPTR dwNewPstate;
extern DWORD GPTR dwCalculateWeaponSpread;

extern CColor<_color_prec> Shadow;
extern Vector3<_prec>      _local;
extern Vector3<_prec>      _vright;
extern Vector3<_prec>      _vup;
extern Vector3<_prec>      _vfwd;

extern Vector3<_prec> LocalPredicted;
extern Vector3<_prec> _Zero;
extern CTrace         _Trace;

extern float BaseSpread, MaxSpread;

//--------------------------------------------------------------------------------------
//---------DYNAMIC FUNCTIONS------------------------------------------------------------
//-----------these may change from various Q3 mods--------------------------------------
//--------------------------------------------------------------------------------------


static int   (__cdecl *Com_Frame)() = (int (__cdecl *)())dwCom_Frame;
static CCVar * (__cdecl * DvarCreate)(char *name, int type, DWORD flag, char *explain, float defaultvalue, float defaultValue2, float defaultValue3, float defaultValue4, int minValue, int maxValue) = (CCVar * (__cdecl *)(char *, int, DWORD, char *, float, float, float, float, int, int))dwCG_DvarCreate;
static short (__cdecl *RegisterTag)(char *tag, DWORD type, size_t taglen)    = (short (__cdecl *)(char *, DWORD, size_t))dwRegisterTag;
static void  (__cdecl *DrawNameTag)(int screen, CEntity *cent, float alpha)  = (void (__cdecl *)(int, CEntity *, float))dwDrawNameTag;
static int   (__cdecl *RegisterFont)(char *FontName, signed int Unknown)     = (int (__cdecl *)(char *, signed int))dwRegisterFont;
static int   (__cdecl *RegisterShader)(char *ShaderName, signed int Unknown) = (int (__cdecl *)(char *, signed int))dwRegisterShader;
static void  (__cdecl *ConsoleCommand)(int x, int x2, char *cvar)            = (void (__cdecl *)(int, int, char *))dwConsoleCommand;
static void  (__cdecl *CG_EchoToConsolef)(int pos, const char *name, ...)    = (void (__cdecl *)(int, const char *, ...))dwCG_EchoToConsolef;
static float (__cdecl *GetRandomFloats)(int Time, float *Right, float *Up)   = (float (__cdecl *)(int, float *, float *))dwGetRandomFloats;
static float (__cdecl *DrawGlowTextLng)(char *text, int len, int font, float x, float y, float scaleX, float scaleY, float rotation, float *color, int style, float *glowColor, int a12, int a13, int a14, int a15, int a16, int a17) = (float (__cdecl *)(char *text, int len, int font, float x, float y, float scaleX, float scaleY, float rotation, float *color, int style, float *glowColor, int a12, int a13, int a14, int a15, int a16, int a17))dwDrawGlowTextLng;

float GetRemainingPower(CEntity *ent, Vector3<_prec>& vieworg, Vector3<_prec>& origin);

CInline void DrawStretchPic(float x, float y, float w, float h, float uk1, float uk2, float uk3, float uk4, float *pColor, int iMaterial)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwDrawStretchPic)
   {
      return;
   }
#endif
   __asm
   {
      push pColor
      push uk4
      push uk3
      push uk2
      push uk1
      push h
      push w
      push y
      push x
      mov eax, iMaterial
          call    [dwDrawStretchPic]
      add esp, 0x24
   }
}


CInline int DrawStringEngine(float x, float y, DWORD pFont, char *pText, float *pColor, float sX, float sY, float f1, float f2)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwDrawStringEngine)
   {
      return;
   }
#endif
   __asm
   {
      push f2
      push f1
      push sY
      push sX
      push y
      push x
      push pFont
      push 0x7FFFFFFF
      push pText
      mov ecx, pColor
      call [dwDrawStringEngine]
      xor eax, eax
      add esp, 0x28
   }
}


CInline CCVar *CG_GetDVarByName(char *name)
{
   CCVar* pdvar;
   __asm
   {
      push edi
      push ebx
      mov edi, name
      mov ebx, 0x56B5D0 // TODO: move this in a variable
      call ebx
      pop ebx
      mov pdvar, eax
      pop edi
   }
   return pdvar;
}


CInline void DrawRotatedPic(float x, float y, float w, float h, float angle, float *color, int Shader)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwDrawRotatedPic2)
   {
      return;
   }
#endif
   _asm
   {
      mov edx, dwPscreenMatrix

      push Shader
      push color
      push angle
      push h
      push w
      push y
      push x
          call    [dwDrawRotatedPic2]
      add esp, 0x1C
   }
}


CInline Vector3<_prec> GetPlayerTag(WORD Tag, CEntity *pEnt)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwGetPlayerTag)
   {
      return(Vector3(0, 0, 0));
   }
#endif
   Vector3<_prec> Origin;
   Vector3<_prec> *pOrigin = &Origin;
   __asm
   {
      push pOrigin
      movzx esi, Tag
      mov ecx, pEnt
      movzx eax, byte ptr [ecx + 4]

      call    [dwGetPlayerTag]
      add esp, 4
   }
   return(Origin);
}


CInline void GetPlayerTag(WORD Tag, CEntity *pEnt, Vector3<_prec> *pOrigin)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwGetPlayerTag)
   {
      return;
   }
#endif
   __asm
   {
      push pOrigin
      movzx esi, Tag
      mov ecx, pEnt
      movzx eax, byte ptr [ecx + 4]

      call    [dwGetPlayerTag]
      add esp, 4
   }
}


CInline Vector3<_prec> GetPlayerTagAccurate(WORD Tag, CEntity *pEnt)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwGetPlayerTagAccurate)
   {
      return(Vector3(0, 0, 0));
   }
#endif
   Vector3<_prec> Origin;
   Vector3<_prec> *pOrigin = &Origin;
   __asm
   {
      push pOrigin
      movzx esi, Tag
      mov ecx, pEnt

            call    [dwGetPlayerTagAccurate]
      add esp, 4
   }
   return(Origin);
}


CInline void CG_Trace(Vector3<_prec> *vStart, CTrace *trace, Vector3<_prec> *vPoint, Vector3<_prec> *vNull, int iNum, DWORD dwFlags)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwCG_Trace)
   {
      return;
   }
#endif
   __asm
   {
      xor             eax, eax
      push eax
      push eax
      push dwFlags
      push iNum
      push vPoint
      push vNull
      push vStart
      mov edi, trace
      mov eax, vNull
      call    [dwCG_Trace]
      add esp, 0x1C
   }
}


CInline void BulletFirePenetrate(CEntity *cent, int localClientNum, CBulletTrace *bulletTrace, CWeapon *weapon, Vector3<_prec> *vieworg, int serverTimeDelta)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwDrawBulletTrajectory)
   {
      return;
   }
#endif
   __asm
   {
      push    [serverTimeDelta];
      push    [vieworg];
      push    [weapon];
      push    [bulletTrace];
      push    [localClientNum];
      mov     edi, [cent];
      call    [dwDrawBulletTrajectory];
      add     esp, 0x14;
   }
}


CInline void GetPlayerViewPoint(Vector3<_prec> *pReturnViewPoint)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwCG_GetPlayerViewPoint)
   {
      return;
   }
#endif
   __asm
   {
      mov eax, dwNewPstate
      mov edx, dword ptr ds : [pReturnViewPoint]
      push edx
      call [dwCG_GetPlayerViewPoint]
      add esp, 4
   }
}


CInline void BG_EvaluateTrajectory(trajectory_t *tr, int atTime, Vector3<_prec> result)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwBG_EvaluateTrajectory)
   {
      return;
   }
#endif
   __asm
   {
	  push eax
	  push ecx

	  mov eax, atTime
	  mov ecx, result
	  push tr
      call [dwBG_EvaluateTrajectory]
	  add esp, 4

	  pop ecx
	  pop eax
   }
}


CInline void BG_EvaluateTrajectoryDelta(trajectory_t *tr, int atTime, Vector3<_prec> result)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwBG_EvaluateTrajectoryDelta)
   {
      return;
   }
#endif
   __asm
   {
	  push eax
	  push edi
	  push esi

	  mov eax, atTime
	  mov edi, result
	  mov esi, tr
      call [dwBG_EvaluateTrajectoryDelta]

	  pop esi
	  pop edi
	  pop eax
   }
}


CInline void GetWeaponSpread(CWeapon *pWeapon, float *WeaponSpread)
{
#ifdef VERIFY_POINTERS
   if (NULL == dwCalculateWeaponSpread)
   {
      return;
   }
#endif

#if 0
   float MaxSpread=0.0f;
   float BaseSpread=0.0f;

   __asm
   {
        mov ecx, dword ptr ds: [ pWeapon ]
        mov edx, dwNewPstate

        lea edi, dword ptr ds: [ BaseSpread ]
        lea esi, dword ptr ds: [ MaxSpread ]
  
        call [ dwCalculateWeaponSpread ]
   }
  
   if(1.0f == cg->CurrentSnap->Zoom)
   WeaponSpread = (float)((MaxSpread - * (float*)((DWORD)pWeapon + 0x71C)) * *(float*)pSpreadMultiplier / 255.0 + *(float*)((DWORD)pWeapon + 0x71C));
   else
   WeaponSpread = (float)((MaxSpread - BaseSpread) * *(float*)pSpreadMultiplier / 255.0 + BaseSpread);
#endif

   DWORD dwCall = 0x416C70;

   float MaxSpread        = 0.0f;
   float BaseSpread       = 0.0f;
   float SpreadMultiplier = 0.0f;

   _asm
   {
      mov ecx, dword ptr ds : [pWeapon];
      mov edx, 0x794474
      lea edi, dword ptr ds : [BaseSpread];
      lea            esi, dword ptr ds : [MaxSpread];
      call dword ptr ds : [dwCall];
      fld dword ptr  ds : [0x84CAA0];
      fdiv qword ptr ds : [0x70AEE0];
      fstp dword ptr ds : [SpreadMultiplier];
   }

   *WeaponSpread = BaseSpread + ((MaxSpread - BaseSpread) * SpreadMultiplier);
}


//#define pSpreadMultiplier	0x00794A9C
#define pSpreadMultiplier    0x0084CAA0

CInline void FirstBulletFix()
{
   if (cvar_NoSpreadDebug1->iValue == 1)
   {
      if (*(float *)0x79E818 != 0.0f)
      {
         *(float *)0xC84F8C = *(float *)0x79E818 * *(float *)0x79D6D4;
      }

      else
      {
         *(float *)0xC84F8C = *(float *)0x79D6D4;
      }

      *(DWORD *)0xC84FD0 |= *(DWORD *)0x84CA8C;
      *(DWORD *)0xC84F84  = *(DWORD *)0x79E6A4;
      *(DWORD *)0xC84F88  = *(DWORD *)0x79E6B4;
      *(DWORD *)0x84CA8C  = 0;
      *(float *)0xC84F98  = *(float *)0x79E754 + *(float *)0x79E760;
      *(float *)0xC84F9C  = *(float *)0x79E764 + *(float *)0x79E758;
      *(float *)0xC84FA0  = *(float *)0x79E768 + *(float *)0x79E75C;
      *(float *)0xC5B6EC  = *(float *)0x797618;
      *(float *)0xC5B6F0  = *(float *)0x79761C;
      *(float *)0xC5B6F4  = *(float *)0x797620;
   }

   if (cvar_NoSpreadDebug2->iValue == 1)
   {
      __asm
      {
         // let's just call sub_455890 instead of pasting code from it.
         xor eax, eax
         mov esi, 0x455890
         call esi
      }
   }

   if (cvar_NoSpreadDebug3->iValue == 1)
   {
      __asm
      {
         // let's call sub_40C6E0 - looks like this may be detrimental?
         xor eax, eax
         mov ecx, 0x7974A4
         push 0
         push 0x794474
         mov esi, 0x40C6E0
         call esi
         add esp, 8
      }
   }

   if (cvar_NoSpreadDebug4->iValue == 1)
   {
      memcpy((void *)0x79743C, (void *)0x7974AC, 0x68u);         // from sub_447980
   }

   if (cvar_NoSpreadDebug5->iValue == 1)
   {
      __asm
      {
         push 0x794520
         push 0x79447C
         push 0x794490
         mov ecx, 0x79449C
         mov eax, 0x79457C
         mov esi, 0x45AD30
         call esi
         add esp, 0xC
      }
   }

   if (cvar_NoSpreadDebug6->iValue == 1)
   {
      __asm
      {
         mov eax, 0x450890
         call eax
      }
   }

   if (cvar_NoSpreadDebug7->iValue == 1)
   {
      __asm
      {
         mov eax, 0x450580
         call eax
      }
   }

   if (cvar_NoSpreadDebug8->iValue == 1)
   {
      //from sub_447980 - spread multiplier fix
      *(float *)0x84CAA0 = *(float *)0x794A9C;
   }
}


CInline int GetAmmoInClip()
{
   DWORD Weapon     = *(DWORD*)((cg->CurrentSnap->curWeapon * 4) + dwPpweapon);
   DWORD WeaponInfo = *(DWORD*)(Weapon + 0x330);

   return (*(DWORD*)((DWORD)cg->CurrentSnap + 0x368 + (WeaponInfo * 4)));
}


CInline void ApplyMassMurder()
{
   for (size_t i = 0; i < 115; i++)
   {
      *(float *)(0x7373B8 + (4 * i)) = cvar_MassMurderValue->Value;
   }
}


CInline bool bIsExplosive(char *wepname)
{
   bool b;

   DECRYPTSTREX(WEAPON_M2FRAGGRENADE, 21);
   DECRYPTSTREX(WEAPON_CLAYMORE, 16);
   DECRYPTSTREX(WEAPON_FLASH_GRENADE, 21);
   DECRYPTSTREX(WEAPON_CONCUSSION_GRENADE, 26);
   DECRYPTSTREX(WEAPON_C4, 10);
   DECRYPTSTREX(WEAPON_SMOKE_GRENADE, 21);
   DECRYPTSTREX(WEAPON_M203, 12);
   DECRYPTSTREX(WEAPON_RPG, 11);
   if (!strcmp(wepname, WEAPON_M2FRAGGRENADE) ||
       !strcmp(wepname, WEAPON_CLAYMORE) ||
       !strcmp(wepname, WEAPON_FLASH_GRENADE) ||
       !strcmp(wepname, WEAPON_CONCUSSION_GRENADE) ||
       !strcmp(wepname, WEAPON_C4) ||
       !strcmp(wepname, WEAPON_SMOKE_GRENADE) ||
       !strcmp(wepname, WEAPON_M203) ||
       !strcmp(wepname, WEAPON_RPG))
   {
      b = true;
   }
   else
   {
      b = false;
   }
   ENCRYPTSTREX(WEAPON_RPG, 11);
   ENCRYPTSTREX(WEAPON_M203, 12);
   ENCRYPTSTREX(WEAPON_SMOKE_GRENADE, 21);
   ENCRYPTSTREX(WEAPON_C4, 10);
   ENCRYPTSTREX(WEAPON_CONCUSSION_GRENADE, 26);
   ENCRYPTSTREX(WEAPON_FLASH_GRENADE, 21);
   ENCRYPTSTREX(WEAPON_CLAYMORE, 16);
   ENCRYPTSTREX(WEAPON_M2FRAGGRENADE, 21);
   return(b);
}


CInline bool bCanFire(int state)
{
   if ((state == 0) || (state == 5))
   {
      return(true);
   }
   return(false);
}


//--------------------------------------------------------------------------------------
//---------STATIC FUNCTIONS-------------------------------------------------------------
//-----------e.g. wrappers, traps-------------------------------------------------------
//--------------------------------------------------------------------------------------


CInline void CG_ConsoleCommand(char *cmd)
{
   ConsoleCommand(0, 0, cmd);
}


CInline void CG_ConsoleCommandf(char *fmt, ...)
{
   va_list valist;

   va_start(valist, fmt);
   vsprintf_s(pBuffer, fmt, valist);
   va_end(valist);

   CG_ConsoleCommand(pBuffer);
}


CInline void DrawGlowText(char *text, int len, int font, float x, float y, float scaleX, float scaleY, float rotation, CColor<_color_prec>& color, int style, CColor<_color_prec>& glowColor)
{
#ifdef VERIFY_POINTERS
   if (NULL == DrawGlowTextLng)
   {
      return;
   }
#endif
   DrawGlowTextLng(text, len, font, x, y, scaleX, scaleY, rotation, (float *)&color, style, (float *)&glowColor, 0, 0, 0, 0, 0, 0);
}


CInline void DrawGlowText(int font, float x, float y, float scaleX, float scaleY, float rotation, CColor<_color_prec>& color, int style, CColor<_color_prec>& glowColor, char *fmt)
{
#ifdef VERIFY_POINTERS
   if (NULL == DrawGlowTextLng)
   {
      return;
   }
#endif
   DrawGlowTextLng(fmt, strlen(fmt), font, x, y, scaleX, scaleY, rotation, (float *)&color, style, (float *)&glowColor, 0, 0, 0, 0, 0, 0);
}


CInline void DrawGlowTextf(int font, float x, float y, float scaleX, float scaleY, float rotation, CColor<_color_prec>& color, int style, CColor<_color_prec>& glowColor, char *fmt, ...)
{
   va_list valist;

   va_start(valist, fmt);
   vsprintf_s(pBuffer, fmt, valist);
   va_end(valist);

   DrawGlowText(font, x, y, scaleX, scaleY, rotation, color, style, glowColor, pBuffer);
}


CInline int R_TextWidth(char *text, int maxChars, int font)
{
  int res;

  __asm
  {
    mov		eax, text
    mov		ebx, 0x5F1EE0

    push	font
    push	maxChars
    call	ebx
    add		esp, 8

    mov		res, eax
  }

  return res;
}

CInline void DrawCenteredGlowText(int font, float x, float y, float scaleX, float scaleY, float rotation, CColor<_color_prec>& color, int style, CColor<_color_prec>& glowColor, char *fmt)
{
#ifdef VERIFY_POINTERS
   if (NULL == DrawGlowTextLng)
   {
      return;
   }
#endif
   int fmtlen = strlen(fmt);
   DrawGlowTextLng(fmt, fmtlen, font, x - (0.5f * R_TextWidth(fmt, fmtlen, font)), y, scaleX, scaleY, rotation, (float *)&color, style, (float *)&glowColor, 0, 0, 0, 0, 0, 0);
}

CInline void DrawCenteredGlowTextf(int font, float x, float y, float scaleX, float scaleY, float rotation, CColor<_color_prec>& color, int style, CColor<_color_prec>& glowColor, char *fmt, ...)
{
   va_list valist;

   va_start(valist, fmt);
   vsprintf_s(pBuffer, fmt, valist);
   va_end(valist);

   DrawCenteredGlowText(font, x, y, scaleX, scaleY, rotation, color, style, glowColor, pBuffer);
}

CInline void Draw2DLine(float x1, float y1, float x2, float y2, float thickness, CColor<_color_prec>& colour)
{
   float dx = x1 - x2;
   float dy = y1 - y2;

   float fLength = (float)sqrt((dx * dx) + (dy * dy));
   float fAngle  = RAD2DEGF((float)atan((y2 - y1) / (x2 - x1)));

   DrawRotatedPic(x1 + ((x2 - x1) * 0.5f) - (fLength * 0.5f), y1 + ((y2 - y1) * 0.5f), fLength, thickness, fAngle, (float *)&colour, sWhite);
}


CInline void DrawString(float X, float Y, DWORD Font, CColor<_color_prec>& Color, float Scale, char *pText)
{
   DrawStringEngine(X, Y, Font, pText, (float *)&Color, Scale, Scale, 0.0f, 0.0f);
}


CInline void DrawStringf(float X, float Y, DWORD Font, CColor<_color_prec>& Color, float Scale, char *pText, ...)
{
   va_list valist;

   va_start(valist, pText);
   vsprintf_s(pBuffer, pText, valist);
   va_end(valist);

   DrawString(X, Y, Font, Color, Scale, pBuffer);
}


CInline void DrawCenteredString(float X, float Y, DWORD Font, CColor<_color_prec>& Color, float Scale, char *pText, ...)
{
   va_list valist;

   va_start(valist, pText);
   vsprintf_s(pBuffer, pText, valist);
   va_end(valist);

   DrawStringEngine(X - (Scale * 3.0f * (float)strlen(pBuffer)), Y, Font, pBuffer, (float *)&Color, Scale, Scale, 0.0f, 0.0f);
}


CInline void DrawEspText(float X, float Y, DWORD Font, CColor<_color_prec> Color, float Scale, char *pText)
{
   switch (cvar_EspShadow->iValue)
   {
   case 1:      // shadow
      DrawStringEngine(X + (cvar_EspFontScale->Value), Y + (cvar_EspFontScale->Value), Font, pText, (float *)&Shadow, Scale, Scale, 0.0f, 0.0f);
      break;

   case 2:      // outline

      /*
       * DrawStringEngine(X + (cvar_EspFontScale->Value), Y + (cvar_EspFontScale->Value), Font, pText, (float*)&Shadow, Scale, Scale, 0.0f, 0.0f);
       * DrawStringEngine(X - (cvar_EspFontScale->Value), Y - (cvar_EspFontScale->Value), Font, pText, (float*)&Shadow, Scale, Scale, 0.0f, 0.0f);
       * DrawStringEngine(X + (cvar_EspFontScale->Value), Y - (cvar_EspFontScale->Value), Font, pText, (float*)&Shadow, Scale, Scale, 0.0f, 0.0f);
       * DrawStringEngine(X - (cvar_EspFontScale->Value), Y + (cvar_EspFontScale->Value), Font, pText, (float*)&Shadow, Scale, Scale, 0.0f, 0.0f);
       */
      DrawStringEngine(X - (cvar_EspFontScale->Value), Y, Font, pText, (float *)&Shadow, Scale, Scale, 0.0f, 0.0f);
      DrawStringEngine(X, Y - (cvar_EspFontScale->Value), Font, pText, (float *)&Shadow, Scale, Scale, 0.0f, 0.0f);
      DrawStringEngine(X + (cvar_EspFontScale->Value), Y, Font, pText, (float *)&Shadow, Scale, Scale, 0.0f, 0.0f);
      DrawStringEngine(X, Y + (cvar_EspFontScale->Value), Font, pText, (float *)&Shadow, Scale, Scale, 0.0f, 0.0f);
      break;

   default:      // no shadow or outline
      break;
   }

   DrawStringEngine(X, Y, Font, pText, (float *)&Color, Scale, Scale, 0.0f, 0.0f);
}


CInline void DrawEspTextf(float X, float Y, DWORD Font, CColor<_color_prec> Color, float Scale, char *pText, ...)
{
   va_list valist;

   va_start(valist, pText);
   vsprintf_s(pBuffer, pText, valist);
   va_end(valist);

   DrawEspText(X, Y, Font, Color, Scale, pBuffer);
}


CInline void DrawBox(float X, float Y, float W, float H, CColor<_color_prec>& Color, int Shader = sWhite)
{
	DrawStretchPic(X, Y, W, H, 0.0f, 0.0f, 1.0f, 1.0f, (float *)&Color, Shader);
}


extern CColor<float> RUWhite, RUBlue, RURed;


CInline void DrawRUFlag(float x, float y, float w, float h)
{
	float third = h / 3.f;
	DrawBox(x, y, w, third, RUWhite);
	DrawBox(x, y + third, w, third, RUBlue);
	DrawBox(x, y + third * 2.f, w, third, RURed);
}


CInline void DrawRect(float x, float y, float w, float h, float fThick, CColor<_color_prec>& pColor)
{
   DrawBox(x, y, w, fThick, pColor);

   y += fThick;

   DrawBox(x, y, fThick, h - (2.f * fThick), pColor);

   y += (h - (2.f * fThick));

   DrawBox(x, y, w, fThick, pColor);

   y -= (h - (2.f * fThick));
   x += (w - fThick);

   DrawBox(x, y, fThick, h - (2.f * fThick), pColor);
}


CInline void DrawOutline(float x, float y, float w, float h, float fThick, CColor<_color_prec>& pColor)
{
   if (cvar_RoundOutlines->iValue == 0)
   {
      x -= fThick;
      y -= fThick;
      w += fThick * 2.0f;
      h += fThick * 2.0f;
      DrawBox(x, y, w, fThick, pColor);

      y += fThick;

      DrawBox(x, y, fThick, h - (2.0f * fThick), pColor);

      y += (h - (2.0f * fThick));

      DrawBox(x, y, w, fThick, pColor);

      y -= (h - (2.0f * fThick));
      x += (w - fThick);

      DrawBox(x, y, fThick, h - (2.0f * fThick), pColor);
   }
   else
   {
      x -= fThick;
      y -= fThick;
      w += fThick * 2.0f;
      h += fThick * 2.0f;
      DrawBox(x + 1.0f, y, w - 2.0f, fThick, pColor);

      y += fThick;

      DrawBox(x, y, fThick, (h - (2.0f * fThick)), pColor);

      y += (h - (2.0f * fThick));

      DrawBox(x + 1.0f, y, w - 2.0f, fThick, pColor);

      y -= (h - (2.0f * fThick));
      x += (w - fThick);

      DrawBox(x, y, fThick, (h - (2.0f * fThick)), pColor);
   }
}


CInline void DrawWindow(char *title, float x, float y, float w, float h)
{
	float thickness = cvar_GUIOutlineThickness->Value;
	DrawOutline(x, y - 20, w, h + 20, thickness, background);
	DrawBox(x, y - 20, w, h + 20, foreground);
	//DrawRUFlag(x, y - 20, w, h + 20);
	DrawBox(x, y, w, 1, background);
	DrawCenteredString(x + (w * 0.5f), y - 3, NormalFont, CColor<_color_prec>(0.8f, 0.8f, 0.8f, 0.7f), 1.0f, title);
}


CInline void DrawBar(int style, float x, float y, float w, float h, float percent, CColor<_color_prec>& col) // TODO: implement
{
   UNREFERENCED_PARAMETER(x);
   UNREFERENCED_PARAMETER(y);
   UNREFERENCED_PARAMETER(w);
   UNREFERENCED_PARAMETER(h);
   UNREFERENCED_PARAMETER(percent);
   UNREFERENCED_PARAMETER(col);

   switch (style)
   {
   case 1:      // an actual bar..

      break;

   case 2:      // the fake bar like there is now

      break;
   }
}


CInline void WorldToScreen(Vector3<_prec>& origin, Vector3<_prec> *out)
{
   _vright = refdef->ViewAxis[1];
   _vfwd   = refdef->ViewAxis[0];
   _vup    = refdef->ViewAxis[2];
   _local  = origin - refdef->vOrigin;
   out->X  = _vright.Dot(_local, _vright);
   out->Y  = _vright.Dot(_local, _vup);
   out->Z  = _vright.Dot(_local, _vfwd);

   out->X = (refdef->iWidth / 2) * (1.f - (out->X / refdef->FovX / out->Z));
   out->Y = (refdef->iHeight / 2) * (1.f - (out->Y / refdef->FovY / out->Z));
}


CInline Vector3<_prec> WorldToScreen(Vector3<_prec>& origin)
{
   Vector3<_prec> Returned;

   WorldToScreen(origin, &Returned);

   return(Returned);
}


CInline void DrawBox3D(Vector3<_prec> Min, Vector3<_prec> Max, CColor<_color_prec> col)
{
   Vector3<_prec> v1, v2, v3, v4, v5, v6, v7, v8;

   v1 = Min;
   v8 = Max;
   v2 = Min;
   v3 = Min;
   v4 = Min;

   v2.X = Max.X;
   v3.Y = Max.Y;
   v4.X = Max.X;
   v4.Y = Max.Y;

   v5   = v1;
   v5.Z = Max.Z;
   v6   = v2;
   v6.Z = Max.Z;
   v7   = v3;
   v7.Z = Max.Z;

   v1 = WorldToScreen(v1);
   v8 = WorldToScreen(v8);

   if (!(v8.Z < 0.01f) && !(v1.Z < 0.01f))
   {
      v2 = WorldToScreen(v2);
      if (!(v2.Z < 0.01f))
      {
         v3 = WorldToScreen(v3);
         if (!(v3.Z < 0.01f))
         {
            v4 = WorldToScreen(v4);
            if (!(v4.Z < 0.01f))
            {
               v5 = WorldToScreen(v5);
               if (!(v5.Z < 0.01f))
               {
                  v6 = WorldToScreen(v6);
                  if (!(v6.Z < 0.01f))
                  {
                     v7 = WorldToScreen(v7);
                     if (!(v7.Z < 0.01f))
                     {
                        Draw2DLine(v1.X, v1.Y, v2.X, v2.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v1.X, v1.Y, v3.X, v3.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v1.X, v1.Y, v5.X, v5.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v2.X, v2.Y, v4.X, v4.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v2.X, v2.Y, v6.X, v6.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v3.X, v3.Y, v4.X, v4.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v3.X, v3.Y, v7.X, v7.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v4.X, v4.Y, v8.X, v8.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v5.X, v5.Y, v6.X, v6.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v5.X, v5.Y, v7.X, v7.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v7.X, v7.Y, v8.X, v8.Y, cvar_BoxEspThickness->Value, col);
                        Draw2DLine(v6.X, v6.Y, v8.X, v8.Y, cvar_BoxEspThickness->Value, col);
                     }
                  }
               }
            }
         }
      }
   }
}


CInline void AddViewAngleDelta(Vector3<_prec> delta)
{
   camera->ViewAngles.Normalize180();
   camera->ViewAngles += delta;
   camera->ViewAngles.Normalize180();
}


CInline void SetViewAngleZ(float z)
{
   camera->ViewAngles.Z = z;
}


CInline void GetViewAngles(Vector3<_prec> *out)
{
   *out = camera->ViewAngles;
}


CInline short R_RegisterTag(char *TagName)
{
#ifdef VERIFY_POINTERS
   if (NULL == RegisterTag)
   {
      return(0);
   }
#endif
   return(RegisterTag(TagName, TAG_TYPE, strlen(TagName) + 1));
}


CInline int R_RegisterFont(char *FontName)
{
#ifdef VERIFY_POINTERS
   if (NULL == RegisterFont)
   {
      return(0);
   }
#endif
   return(RegisterFont(FontName, FONT_TYPE));
}


CInline int R_RegisterShader(char *ShaderName)
{
#ifdef VERIFY_POINTERS
   if (NULL == R_RegisterShader)
   {
      return(0);
   }
#endif
   return(RegisterShader(ShaderName, SHADER_TYPE));
}


CInline CCVar *CG_DvarCreate(char *name, char *script, float *defaultValue, float *minValue, float *maxValue)
{
#ifdef VERIFY_POINTERS
   if (NULL == DvarCreate)
   {
      return(NULL);
   }
#endif
   return(DvarCreate(name, 0, 0, script, *defaultValue, 0.0f, 0.0f, 0.0f, *(int *)minValue, *(int *)maxValue));
}


CInline Vector3<_prec> GetPlayerTagAccurateSmart(WORD Tag, CEntity *pEnt)
{
   Vector3<_prec> Origin = GetPlayerTagAccurate(Tag, pEnt);

   if ((pEnt->vOrigin - Origin).Length() > 200.0f)
   {
      return(GetPlayerTag(Tag, pEnt));
   }

   return(Origin);
}


CInline Vector3<_prec> GetPlayerTagAccurateSafe(WORD Tag, CEntity *pEnt)
{
#if 0
   Vector3<_prec> tfovc;
   tfovc = (pEnt->vOrigin - LocalPredicted).Angles() - refdef->ViewAngles;
   tfovc.Normalize180();

   if(tfovc.X < 80 && tfovc.X > -80)
   {
        return GetPlayerTagAccurateSmart(Tag, pEnt);
   }
#endif

   return(GetPlayerTag(Tag, pEnt));
}


CInline void GetWeaponViewAngles(Vector3<_high_prec>& pReturnAngle)
{
   /*if(cg->CurrentSnap->Zoom > 0.0f)
    * {*/
   pReturnAngle.X = (_high_prec) *(float *)pWeaponX;
   pReturnAngle.Y = (_high_prec) *(float *)pWeaponY;
   pReturnAngle.Z = 0.0f;

   /*}
    * else
    * {
    *      pReturnAngle.X = (_high_prec) refdef->ViewAngles.X;
    *      pReturnAngle.Y = (_high_prec) refdef->ViewAngles.Y;
    *      pReturnAngle.Z = 0.0;
    * }
    */
}


CInline void UsercmdCopy(CUsercmd *from, Usercmd_t *to)
{
   to->serverTime = from->serverTime;
}


CInline bool VisTrace(Vector3<_prec>& Start, Vector3<_prec>& End, int Target)
{
   memset((void *)&_Trace, 0, sizeof(CTrace));
   CG_Trace(&Start, &_Trace, &End, &_Zero, cg->clientNum, VISIBLE_TRACE);

   if ((Target == _Trace.EntityNumber) || (_Trace.Fraction >= 1.0f))
   {
      return(true);
   }

   return(false);
}


/*
 * CInline bool VisTrace(Vector3<_prec>* Start, Vector3<_prec>* End, int Target)
 * {
 *      memset((void*)&_Trace,0,sizeof(CTrace));
 *      CG_Trace(&Start,&_Trace,&End,&_Zero,cg->clientNum,VISIBLE_TRACE);
 *
 *      End = Start + ((End-Start)*_Trace.Fraction);
 *      if (Target == _Trace.EntityNumber || _Trace.Fraction >= 1.0f || (End-Start).Length() < 10.0f)
 *              return true;
 *      return false;
 * }
 */
CInline CEntity *const GetEntityByIndex(int i)
{
   return((CEntity *)(dwCEntities + (ENTITY_SIZE * i)));
}


CInline CClientInfo *const GetClientByIndex(int i)
{
   return((CClientInfo *)(dwCClients + (CLIENT_SIZE * i)));
}


CInline CWeapon *const GetWeaponByIndex(int iWeaponID)
{
   if (iWeaponID < 0)
   {
      return(NULL);
   }
   return((CWeapon *)*(DWORD *)((iWeaponID * 4) + dwPpweapon));
}


CInline char *GetWeaponNameByIndex(char *Buffer, int WeaponNumber)
{
   if (!WeaponNumber)
   {
      return("(null)");
   }

   CWeaponInfo *pWeapon = (CWeaponInfo *)((DWORD)dwWeaponInfo + WeaponNumber * CWeaponSize);

   if (!pWeapon || !pWeapon->Name)
   {
      return("(null)");
   }

   strcpy_s(Buffer, 256, pWeapon->Name);
   return(Buffer);
}


extern DWORD sWhite;

#endif
