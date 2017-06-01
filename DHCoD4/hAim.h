#ifndef HAIMING_H
#define HAIMING_H

#define FNONCE    -1000.0f

#include "hLog.h"
#include "h47.h"
#include "controlfp.h"

extern BOOL           FlyCamEnabled;
extern Vector3<_prec> OldViewOrigin, AimOrigin, ViewOrigin;

extern Vector3<_prec> GetLocalViewpoint();
extern bool AimAtPlayerEntity(CEntity *pEnt);
void Rotate(bool bFrame = false);
extern void TouchUserCmds();

extern Vector3<_prec> GetHeadPos(CEntity *cent);
void GetEntityVelocity(CEntity *pEnt, Vector3<double> *out);

bool GetStealthAnglesToTarget(CEntity *Target, vec2_t Angles);

extern vec2_t lazyness;

extern BOOL bKnifing;

extern Vector3<_prec> LocalPredicted, BestAngles, AbsBestAngles, ovel[MAX_PLAYERS];
extern Vector3<_prec> Head, LocalPrediction, HeadBackup;
extern float          PingCorrection, TimeCorrection, HighestThreat, BestPower;
extern int            dwTargetID;
extern int            ofx_draw;
extern int            ofx_enable, odynEnt_active, osv_clientSideBullets;
extern float          ocg_tracerwidth;
extern bool           bShouldRotate;
extern bool           bShouldFire;
extern WORD           aimTags[20];

bool IsInFOV(Vector3<_prec>& angl, float fFOV = cvar_AimFOV->Value);

CInline void AimbotAutowallFix()
{
   SAFE_CVAR_READ_DWORD(real_fx_enable, ofx_enable);
   SAFE_CVAR_WRITE_DWORD(real_fx_enable, 0);

   SAFE_CVAR_READ_DWORD(real_fx_draw, ofx_draw);
   SAFE_CVAR_WRITE_DWORD(real_fx_draw, 0);

   SAFE_CVAR_READ_FLOAT(real_cg_tracerwidth, ocg_tracerwidth);
   SAFE_CVAR_WRITE_FLOAT(real_cg_tracerwidth, 0);

   SAFE_CVAR_READ_DWORD(real_dynEnt_active, odynEnt_active);
   SAFE_CVAR_WRITE_DWORD(real_dynEnt_active, 0);

   //SAFE_CVAR_READ_DWORD(real_sv_clientSideBullets, osv_clientSideBullets);
   //SAFE_CVAR_WRITE_DWORD(real_sv_clientSideBullets, 0);
}


CInline bool bAimbotIsReady()
{
#ifdef COMPILE_DEBUG
   if (cvar_Debug->iValue == 1)
   {
      CG_EchoToConsolef(CONSOLE, "^1[%i]^5NULL weapon\n", *CurServerTime);
      Logf("[%i]NULL weapon\n", *CurServerTime);

      if (!bCanFire(cg->CurrentSnap->WeaponState))
      {
         CG_EchoToConsolef(CONSOLE, "^1[%i]^5not aiming due aim test ID 0 - ARG %i\n", *CurServerTime, cg->CurrentSnap->WeaponState);
         Logf("[%i]not aiming due aim test ID 0 - ARG %i\n", *CurServerTime, cg->CurrentSnap->WeaponState);
      }

      if (!GetAmmoInClip())
      {
         CG_EchoToConsolef(CONSOLE, "^1[%i]^5not aiming due aim test ID 1\n", *CurServerTime);
         Logf("[%i]not aiming due aim test ID 1\n", *CurServerTime);
      }

      if (bKnifing)
      {
         CG_EchoToConsolef(CONSOLE, "^1[%i]^5not aiming due aim test ID 2\n", *CurServerTime);
         Logf("[%i]not aiming due aim test ID 2\n", *CurServerTime);
      }

      CWeapon *pwep = GetWeaponByIndex(pLocalEnt->WeaponId);

      if (pwep)
      {
         int deltaTime = pwep->fireTime - predictedPlayerState->weaponTime;

         if (deltaTime < 0)
         {
            CG_EchoToConsolef(CONSOLE, "^1[%i]^5not aiming due to aim test ID 3 - ARG %i(%i - %i)\n", *CurServerTime, deltaTime, pwep->fireTime, predictedPlayerState->weaponTime);
         }
      }
   }
#endif // COMPILE_DEBUG

   if (cvar_AimKey->iValue != 0)
   {
	   bool pressed = GetAsyncKeyState(cvar_AimKey->iValue) & 1;

	   if (!pressed)
	   {
		   return false;
	   }
   }

   if ((cg->CurrentSnap->WeaponState == 0 || cg->CurrentSnap->WeaponState == 5) && GetAmmoInClip() && !bKnifing)
   {
      CWeapon *pwep = GetWeaponByIndex(pLocalEnt->WeaponId);

#if 0
      if (pwep)
      {
         int deltaTime = pwep->fireTime - predictedPlayerState->weaponTime;

         if (deltaTime < 0)
         {
            return(false);
         }
      }
#endif

      if ((cvar_NoAimProne->iValue == 1) && (pLocalEnt->State & FLAGS_PRONE))
      {
         return(false);
      }

      if (cvar_NoAimExplosive->iValue == 1)
      {
         if (pwep && pwep->displayName)
         {
            if (bIsExplosive(pwep->displayName))
            {
               return(false);
            }
         }
      }

      if ((cvar_NoAimClimbing->iValue == 1) && (pLocalEnt->State & FLAGS_CLIMBING))
      {
         return(false);
      }

      return(true);
   }

   return(false);
}


CInline Vector3<_prec> GetHeadPos(CEntity *cent) // TODO: take an OUT argument instead of returning in retval.
{
   Vector3<_prec> helmet, neck, eye;
   Head   = GetPlayerTagAccurateSafe(aimTags[0], cent);  // head
   helmet = GetPlayerTagAccurateSafe(aimTags[18], cent); // helmet
   neck   = GetPlayerTagAccurateSafe(aimTags[17], cent); // neck
   eye    = GetPlayerTagAccurateSafe(aimTags[19], cent); // eye

   Head.X = (Head.X * cvar_AimHeadScalar->Value) + (helmet.X * cvar_AimHelmetScalar->Value) + (neck.X * cvar_AimNeckScalar->Value) + (eye.X * cvar_AimEyeScalar->Value);
   Head.Y = (Head.Y * cvar_AimHeadScalar->Value) + (helmet.Y * cvar_AimHelmetScalar->Value) + (neck.Y * cvar_AimNeckScalar->Value) + (eye.Y * cvar_AimEyeScalar->Value);
   Head.Z = (Head.Z * cvar_AimHeadScalar->Value) + (helmet.Z * cvar_AimHelmetScalar->Value) + (neck.Z * cvar_AimNeckScalar->Value) + (eye.Z * cvar_AimEyeScalar->Value);

   return(Head);
}


CInline void AimbotPreCalc(bool bFrame = false)
{
   set64bitpc();
   AimbotAutowallFix();
   DECRYPTSTREX(countdown, 10);
   DECRYPTSTREX(bog, 4);
   DECRYPTSTREX(backlot, 8);
   DECRYPTSTREX(killhouse, 10);

   if (cvar_PingCorrection->iValue == 1)
   {
      PingCorrection = LerpFloat(1.0f, ((cg->CurrentSnap->Ping + cg->NextSnap->Ping) / 2) / cvar_PingDivisor->Value, cvar_PingScalar->Value);
   }
   else
   {
      PingCorrection = 1.0f;
   }

   if (cvar_TimeCorrection->iValue == 1)
   {
      float f = (float)(cg->NextSnap->ServerTime - cg->CurrentSnap->ServerTime);
      TimeCorrection = LerpFloat(1.0f, 1.0f + ((cg->NextSnap->ServerTime - *PhysicsTime) / f), cvar_TimeCorrectionScalar->Value);
   }

   else
   {
      TimeCorrection = 1.0f;
   }

   LocalPrediction = (predictedPlayerState->vel) * (cvar_LocalPrediction->Value * PingCorrection);
   ViewOrigin      = GetLocalViewpoint();
   LocalPredicted  = ViewOrigin + LocalPrediction;
   AimOrigin       = LocalPredicted;
   bShouldFire     = false;
   bShouldRotate   = false;
   HighestThreat   = FNONCE;

   if (cvar_AutowallUseAltSort->iValue == 1)
   {
      BestPower = HighestThreat;
   }
   else
   {
      BestPower = 0.0f;
   }

   BestAngles = 0.0f;

   if (dwTargetID != -1)
   {
      if ((cvar_AimTargetLock->iValue == 1) || bFrame)
      {
         CEntity     *pTEnt = GetEntityByIndex(dwTargetID);
         CClientInfo *pTCi  = GetClientByIndex(dwTargetID);

         float fDist = (refdef->vOrigin - pTEnt->vOrigin).Length() / DIST_DIVISOR;

         if (((cvar_AimRange->Value == 0.0f) || (fDist <= cvar_AimRange->Value)) && pTEnt->Alive && pTEnt->IsValidEnt && ((pTCi->Team != pLocalClient->Team) || (pTCi->Team == TEAM_FFA)) && bAimbotIsReady())
         {
            if (!AimAtPlayerEntity(pTEnt))
            {
               dwTargetID = -1;
            }
         }

         else
         {
            dwTargetID = -1;
         }
      }
      else
      {
         dwTargetID = -1;
      }
   }
}


CInline void AimbotEndCalcs()
{
   if (dwTargetID != -1)
   {
      AbsBestAngles = (HeadBackup - AimOrigin).DAngles();
      BestAngles    = AbsBestAngles - refdef->ViewAngles;
      BestAngles.Z  = 0.0f;
      BestAngles.Normalize180();
   }

   set24bitpc();
   ENCRYPTSTREX(countdown, 10);
   ENCRYPTSTREX(bog, 4);
   ENCRYPTSTREX(backlot, 8);
   ENCRYPTSTREX(killhouse, 10);
   SAFE_CVAR_WRITE_DWORD(real_fx_enable, ofx_enable);
   SAFE_CVAR_WRITE_DWORD(real_fx_draw, ofx_draw);
   SAFE_CVAR_WRITE_FLOAT(real_cg_tracerwidth, ocg_tracerwidth);
   SAFE_CVAR_WRITE_DWORD(real_dynEnt_active, odynEnt_active);
   //SAFE_CVAR_WRITE_DWORD(real_sv_clientSideBullets, osv_clientSideBullets);
}


CInline void GetAimAngles(Vector3<_prec> *outa)
{
   if (cvar_SmoothAim->iValue == 0)
   {
      *outa = BestAngles;
      return;
   }

   vec2_t lazyness;
   if (GetStealthAnglesToTarget(GetEntityByIndex(dwTargetID), lazyness))
   {
      outa->X = lazyness[0];
      outa->Y = lazyness[1];
      outa->Z = 0.0f;
   }

   else
   {
      *outa = 0.0f;
   }
}


#endif
