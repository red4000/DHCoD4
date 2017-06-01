// TODO: Time Correction 2.  this is essentially the same as 1, but instead of using a scalar, it gets the delta(newOrigin - oldOrigin), then gets the interpolated delta(newOrigin - vOrigin), then gets the time correction velocity(delta-interpolatedDelta)
// time correction 2 velocity calc formula: (newOrigin-oldOrigin)-(vOrigin-oldOrigin) or something
//maybe switch it up and make a time correction 3.

// TODO: new smooth aim techniques( COMPLETELY new ):
// - one that uses shitty shitty scaling like josh used to have

#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hData.h"
#include "hFunctions.h"
#include "hConfig.h"
#include "hBot.h"
#include "hAim.h"
#include "h47.h"
#include "hNewCore.h"
#include "hPrediction.h"

Vector3<_prec> BestAngles(FNONCE, FNONCE, 0.0f), AbsBestAngles, ovel[MAX_PLAYERS];
float          BestPower     = 0.0f, PingCorrection = 1.0f, TimeCorrection = 1.0f;
bool           bShouldRotate = false, bShouldFire = false;

Vector3<_prec> LocalPredicted, LocalPrediction, SpreadDisplacement, SpreadHit, Head, HeadBackup, ret, AimOrigin, ViewOrigin, aZ;
int            dwTargetID    = -1;
DWORD          dwTargetTag   = 0;
float          HighestThreat = FNONCE;

int          ofx_draw;
int          ofx_enable, odynEnt_active, osv_clientSideBullets;
float        ocg_tracerwidth, oMouseSensivity = FNONCE;
morelazies_t p;

Usercmd_t ocmds[CMD_BACKUP];


CInline float GetThreat(CEntity *cent, Vector3<_prec> *tHead, Vector3<_prec> *tAngToHead)
{
   if (fDistance[cent->ClientNumber] <= cvar_AimDistanceOverride->Value)
   {
      return(fDistance[cent->ClientNumber] * 0.001f);
   }

   Vector3<_prec> tAngles2, iAngles;

   switch (cvar_AimSort->iValue)
   {
   case 1:      // threat sorting
                // TODO: see if warning sorting in hEsp is better than this, also, improve this either way.
      tAngles2 = cent->vAngles;
      tAngles2.Normalize360();

      iAngles = (GetLocalViewpoint() - *tHead).Angles() - tAngles2;
      iAngles.Normalize180();

      return(iAngles.LengthFast2D());

   case 2:      // distance sorting
      return(fDistance[cent->ClientNumber]);

   case 3:      // distance from crosshair sorting
      return(tAngToHead->LengthFast2D());

   case 4:      // lowest velocity sorting ( easiest to hit )
	   return((cent->vNewOrigin - cent->vOldOrigin).LengthFast());

   default:
      return(0.0f);
   }
}


bool IsInFOV(Vector3<_prec>& angl, float fFOV)
{
   if ((fFOV == 360.0f) || (fFOV == 0.0f))
   {
      return(true);
   }

   //angl.Normalize180();

   /*
    * float fFOV = cvar_AimFOV->Value / 2.0f;
    * if((angl.X <= fFOV && angl.X >= -fFOV) &&
    *              (angl.Y <= fFOV && angl.Y >= -fFOV))
    * {
    *      return true;
    * }
    */
   if (angl.LengthFast2D() <= fFOV * fFOV)
   {
      return(true);
   }

   return(false);
}


CInline bool IsInFOVEx(Vector3<_prec>& angl, float fMinFov)
{
   if ((cvar_AimFOV->Value == 360.0f) || (cvar_AimFOV->Value == 0.0f))
   {
      return(true);
   }

   angl.Normalize180();

   float fFOV = cvar_AimFOV->Value;
   if (fFOV < fMinFov)
   {
      fFOV = fMinFov;
   }

   return(IsInFOV(angl, fFOV));
}


Vector3<_prec> GetLocalViewpoint()
{
   if (FlyCamEnabled)
   {
      return(OldViewOrigin);
   }

   Vector3<_prec> tmp;
   GetPlayerViewPoint(&tmp);
   return(tmp);
}


clientState_s *GetClientStateByIndex(snapshot_s *snap, int clientNumber)
{
	for (size_t i = 0; i < snap->numEntities; i++)
	{
		if (snap->clients[i].clientIndex == clientNumber && (snap->clients[i].team != pLocalClient->Team || snap->clients[i].team == TEAM_FFA))
		{
			return &snap->clients[i];
		}
	}

	return NULL;
}


entityState_s *GetEntityStateByIndex(snapshot_s *snap, int clientNumber)
{
	if (NULL == snap)
	{
		return NULL;
	}

	for (size_t i = 0; i < snap->numEntities; i++)
	{
		if (snap->entities[i].number == clientNumber/* && snap->entities[i].eType == TYPE_HUMAN && GetClientStateByIndex(snap, clientNumber)*/)
		{
			return &snap->entities[i];
		}
	}

	return NULL;
}


void GetEntityVelocity(CEntity *pEnt, Vector3<double> *pout)
{
#ifdef H47
   *out   = ((Vector3<float>((vic[pEnt->ClientNumber].pred.vel.X + vic[pEnt->ClientNumber].pred.accel.Z), (vic[pEnt->ClientNumber].pred.vel.Y + vic[pEnt->ClientNumber].pred.accel.Y), (vic[pEnt->ClientNumber].pred.vel.Z + vic[pEnt->ClientNumber].pred.accel.Z))) * (cvar_AimPrediction->Value * PingCorrection * TimeCorrection));
   out->X = (out->X * cvar_AimPredictScalarX->Value) - ((vic[cg->clientNum].pred.vel.X + vic[cg->clientNum].pred.accel.X) * cvar_LocalPrediction->Value);
   out->Y = (out->Y * cvar_AimPredictScalarY->Value) - ((vic[cg->clientNum].pred.vel.Y + vic[cg->clientNum].pred.accel.Y) * cvar_LocalPrediction->Value);
   out->Z = (out->Z * cvar_AimPredictScalarZ->Value) - ((vic[cg->clientNum].pred.vel.Z + vic[cg->clientNum].pred.accel.Z) * cvar_LocalPrediction->Value);
#else
	
   entityState_s *newEntity = GetEntityStateByIndex(_cg->nextSnap, pEnt->ClientNumber);
   entityState_s *curEntity = GetEntityStateByIndex(_cg->snap, pEnt->ClientNumber);

   int predictionMode = 0;
   Vector3<_prec> vel,vel2=pEnt->vNewOrigin-pEnt->vOldOrigin;

   if (NULL == newEntity || NULL == curEntity)
   {
	   if (NULL != curEntity)
	   {
           CG_EchoToConsolef(CONSOLE, "newEntity == NULL\n");
		   predictionMode = 1;
		   vel = curEntity->lerp.pos.trDelta;
	   }
	   else if (NULL != newEntity)
	   {
           CG_EchoToConsolef(CONSOLE, "curEntity == NULL\n");
		   predictionMode = 2;
		   vel = newEntity->lerp.pos.trDelta;
	   }
	   else
	   {
	       CG_EchoToConsolef(CONSOLE, "newEntity == NULL && curEntity == NULL\n");
		   predictionMode = 3;
		   vel = pEnt->vNewOrigin - pEnt->vOldOrigin;
	   }
   }
   else
   {
		CG_EchoToConsolef(CONSOLE, "newEntity && curEntity\n");
		predictionMode = 4;

		int predictionTime = (*(int*)0xC628D0) + cvar_TimeAdditive->Value;

		if (predictionTime > _cg->nextSnap->serverTime)
		{
			predictionTime = _cg->nextSnap->serverTime;
		}
		else if (predictionTime < _cg->snap->serverTime)
		{
			predictionTime = _cg->snap->serverTime;
		}

		vel = curEntity->lerp.pos.trBase.Lerp(newEntity->lerp.pos.trBase, (float)((predictionTime) - _cg->snap->serverTime) / (float)(_cg->nextSnap->serverTime - _cg->snap->serverTime)) - curEntity->lerp.pos.trBase;
		//vel = curEntity->lerp.pos.trBase.Lerp(newEntity->lerp.pos.trBase, (float)(predictionTime) / (float)_cg->nextSnap->serverTime) - curEntity->lerp.pos.trBase;
		CG_EchoToConsolef(CONSOLE, "curServerTime: %i | newServerTime: %i | curBaseTime: %i..%i | newBaseTime: %i..%i | cmdTime %i",_cg->snap->serverTime,_cg->nextSnap->serverTime,curEntity->lerp.pos.trTime,curEntity->lerp.pos.trDuration,newEntity->lerp.pos.trTime,newEntity->lerp.pos.trDuration,*(int*)0x839248);
   }

   CG_EchoToConsolef(CONSOLE, "%i: %f, %f, %f -- cent velocity = %f, %f, %f\n", predictionMode, vel.X, vel.Y, vel.Z, vel2.X, vel2.Y, vel2.Z);
  pout->X = (((pEnt->vNewOrigin.X - pEnt->vOldOrigin.X) * cvar_AimPrediction->Value) + (vel.X * cvar_AimPrediction2->Value)) * PingCorrection * TimeCorrection * cvar_AimPredictScalarX->Value;
  pout->Y = (((pEnt->vNewOrigin.Y - pEnt->vOldOrigin.Y) * cvar_AimPrediction->Value) + (vel.Y * cvar_AimPrediction2->Value)) * PingCorrection * TimeCorrection * cvar_AimPredictScalarY->Value;
  pout->Z = ((((pEnt->vNewOrigin.Z - pEnt->vOldOrigin.Z) * cvar_AimPrediction->Value) + (vel.Z * cvar_AimPrediction2->Value)) * PingCorrection * TimeCorrection * cvar_AimPredictScalarZ->Value) + cvar_AimVecZ->Value;

#if 0
	__asm
	{
		push  eax
		push  edi
		push  edx

		mov   eax, cvar_AimPrediction
		fld   dword ptr [eax]
		fmul  PingCorrection
		fmul  TimeCorrection
		fld   st
		fld   st

		mov   eax, pEnt
		mov   edi, pout
		fld   dword ptr [eax + 228]
		fsub  dword ptr [eax + 116]
		fmulp st(1), st
		mov   edx, cvar_AimPredictScalarX
		fmul  dword ptr [edx]
		fadd  dword ptr [edi]
		fstp  dword ptr [edi]

		fld   dword ptr [eax + 228 + 4]
		fsub  dword ptr [eax + 116 + 4]
		fmulp st(1), st
		mov   edx, cvar_AimPredictScalarY
		fmul  dword ptr [edx]
		fadd  dword ptr [edi + 4]
		fstp  dword ptr [edi + 4]

		fld   dword ptr [eax + 228 + 8]
		fsub  dword ptr [eax + 116 + 8]
		fmulp st(1), st
		mov   edx, cvar_AimPredictScalarZ
		fmul  dword ptr [edx]
		mov   edx, cvar_AimVecZ
		fadd  dword ptr [edx]
		fadd  dword ptr [edi + 8]
		fstp  dword ptr [edi + 8]

		pop   edx
		pop   edi
		pop   eax
	}
#endif
#endif // H47
}


//#pragma optimize("", off)

bool IsHit(CEntity *pEnt, Vector3<_prec> *pos)
{
      if ((cvar_AimVisChecks->iValue == 0) || VisTrace(ViewOrigin, Head, pEnt->ClientNumber))
      {
		  return true;
	  }
      else if (cvar_AutoWall->iValue >= 1)
      {
         float Power = GetRemainingPower(pEnt, ViewOrigin, Head);

         if (Power >= cvar_AutoWallTolerance->Value)
         {
			 return true;
		 }
	  }

	  return false;
}

bool AimAtPlayerEntity(CEntity *pEnt)
{
   Vector3<_prec> tmpHeadAngles, tmpHeadPredicted, tmpAbsAngles, entPrediction, theAimPos, accel;
   float          threat;

   Head = GetHeadPos(pEnt);

   tmpHeadAngles = (Head - ViewOrigin).Angles() - refdef->ViewAngles;

   threat = GetThreat(pEnt, &Head, &tmpHeadAngles);

   if ((HighestThreat != FNONCE) && (threat > HighestThreat))
   {
      return(false);
   }

   if (!IsInFOVEx(tmpHeadAngles, 45.0f))
   {
      return(false);
   }

   WORD Tag;

   for (int i = cvar_AimBoneStartIndex->iValue; i < cvar_AimBoneQuantity->iValue; ++i)
   {
      Tag         = aimTags[i];
      dwTargetTag = i;

	  Vector3<double> velocity;
	  GetEntityVelocity(pEnt, &velocity);

      if (i != 0)
      {
         Head = GetPlayerTagAccurateSafe(Tag, pEnt);
#ifdef H47
         Vector3<float> bpred(vic[pEnt->ClientNumber].bone[i].pred.vel.X + vic[pEnt->ClientNumber].bone[i].pred.accel.X, vic[pEnt->ClientNumber].bone[i].pred.vel.Y + vic[pEnt->ClientNumber].bone[i].pred.accel.Y, vic[pEnt->ClientNumber].bone[i].pred.vel.Z + vic[pEnt->ClientNumber].bone[i].pred.accel.Z);
         bpred.X       *= cvar_AimPredictScalarX->Value;
         bpred.Y       *= cvar_AimPredictScalarY->Value;
         bpred.Z       *= cvar_AimPredictScalarZ->Value;
         entPrediction += bpred;
#endif   // H47
      }
	  else
	  {
		  ApplyPrediction3(false, pEnt, &velocity);
	  }

	  Head.X += velocity.X;
	  Head.Y += velocity.Y;
	  Head.Z += velocity.Z;
	  
	  Vector3<_prec> points[50];

	  for (size_t j = 0; j < cvar_AimMultiPoint->Value; j++)
	  {
		  if (j == 0)
		  {
			  points[j] = Head;
		  }
		  else
		  {
			  switch (j % 10)
			  {
			  case 1:
				  points[j].X = points[0].X + (j * velocity.X * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y + (j * velocity.Y * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z + (j * velocity.Y * cvar_AimMultiPointScalar->Value);
				  break;

			  case 2:
				  points[j].X = points[0].X - (j * velocity.X * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y - (j * velocity.Y * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z - (j * velocity.Y * cvar_AimMultiPointScalar->Value);
				  break;

			  case 3:
				  points[j].X = points[0].X - (j * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y;
				  points[j].Z = points[0].Z;

			  case 4:
				  points[j].X = points[0].X + (j * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y;
				  points[j].Z = points[0].Z;
				  break;

			  case 5:
				  points[j].X = points[0].X;
				  points[j].Y = points[0].Y + (j * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z;
				  break;

			  case 6:
				  points[j].X = points[0].X;
				  points[j].Y = points[0].Y - (j * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z;
				  break;

			  case 7:
				  points[j].X = points[0].X + (j * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y + (j * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z;
				  break;

			  case 8:
				  points[j].X = points[0].X + (j * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y - (j * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z;
				  break;

			  case 9:
				  points[j].X = points[0].X - (j * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y + (j * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z;
				  break;

			  case 0:
				  points[j].X = points[0].X - (j * cvar_AimMultiPointScalar->Value);
				  points[j].Y = points[0].Y - (j * cvar_AimMultiPointScalar->Value);
				  points[j].Z = points[0].Z;
				  break;
			  }
		  }

		  Head      = points[j];
		  theAimPos = Head;

		  if ((cvar_AimVisChecks->iValue == 0) || VisTrace(ViewOrigin, Head, pEnt->ClientNumber))
		  {
			 tmpAbsAngles = (Head - AimOrigin).Angles();
			 Head         = tmpAbsAngles - refdef->ViewAngles;
			 Head.Z       = 0.0f;
			 Head.Normalize180();

			 if (IsInFOV(Head))
			 {
				BestPower = 1.0f;
				goto ChooseTarget;
			 }
		  }
		  else if (cvar_AutoWall->iValue >= 1)
		  {
			 float Power = GetRemainingPower(pEnt, ViewOrigin, Head);

			 if (Power >= cvar_AutoWallTolerance->Value)
			 {
				tmpAbsAngles = (Head - AimOrigin).Angles();
				Head         = tmpAbsAngles - refdef->ViewAngles;
				Head.Z       = 0.0f;
				Head.Normalize180();

				if (IsInFOV(Head))
				{
					if (Power > BestPower)
					{
						BestPower = Power;
						goto ChooseTarget;
					}
				}
			 }
		  }
		  
		  if (i != 0)
		  {
			  break;
		  }
	  }
   }

   return(false);

ChooseTarget:
   HeadBackup    = theAimPos;
   BestAngles    = Head;
   bShouldRotate = true;
   dwTargetID    = pEnt->ClientNumber;
   HighestThreat = threat;
   return(true);
}


//#pragma optimize("", on)

bool GetStealthAnglesToTarget(CEntity *Target, vec_t *Angles)
{
   static CEntity *LastTarget;
   static float   fDivisions;
   static float   fDeltaAngle;
   static float   fAngle;
   static vec2_t  fLastKick;
   static vec2_t  fAim;
   static float   fAbsDelta;
   static float   fAmplitude;

   Vector3<float> unitvec;

   if (cvar_SmoothAimMethod->iValue == 5)
   {
      float amount = cvar_SmoothAimAmplitude->Value;
      if (BestAngles.LengthFast2D() > amount * amount)
      {
         unitvec   = BestAngles.Unitize();
         Angles[0] = unitvec.X * amount;
         Angles[1] = unitvec.Y * amount;
      }
      else
      {
         Angles[0] = BestAngles.X;
         Angles[1] = BestAngles.Y;
      }
      return(true);
   }

   if (!LastTarget || (Target != LastTarget))
   {
      fDivisions = cvar_SmoothAimDivisions->Value;
      fAmplitude = DEG2RADF(cvar_SmoothAimAmplitude->Value);

      fDeltaAngle = M_PIF / fDivisions;
      //fAngle = 0.0f;
      fAngle = 2.0f * M_PIF;

      fLastKick[0] = 0.0f;
      fLastKick[1] = 0.0f;

      fAim[0] = BestAngles.X;
      fAim[1] = BestAngles.Y;

      // Calculate the aim speed according to how far away the target is initially
      fAbsDelta = DEG2RADF((float)_hypot(fAim[0], fAim[1]) / fDivisions);

      LastTarget = Target;

      return(GetStealthAnglesToTarget(Target, Angles));
   }
   else
   {
      //fAngle += fDeltaAngle;
      fAngle -= fDeltaAngle;
      float fKick;

      switch (cvar_SmoothAimMethod->iValue)
      {
      case 1:
         fKick = fAmplitude * fAbsDelta * logf(fabs(fAngle)) * sin(fAngle);
         break;

      case 2:
         fKick = fAmplitude * fAbsDelta * logf(fabs(fAngle)) * log(fAngle) * sin(fAngle);
         break;

      case 3:
         fKick = fAmplitude * fAbsDelta * logf(fabs(fAngle)) * pow(log(fAngle), 2) * sin(fAngle);
         break;

      default:
         fKick = fAmplitude * fAbsDelta * sin(fAngle);
         break;
      }

      fAim[0] = BestAngles.X;
      fAim[1] = BestAngles.Y;

      float fTarget = (float)_hypot(fAim[0], fAim[1]);
      float fRoll   = acos(fAim[PITCH] / fTarget);           // Calculate the roll angle from the vertical

      if (fAim[YAW] < 0.0f)
      {
         fRoll = -fRoll;                   // -M_PI <= fRoll <= M_PI
      }

      vec2_t fKicks;
      fKicks[YAW]   = RAD2DEGF(-fKick * cos(fRoll));
      fKicks[PITCH] = RAD2DEGF(fKick * sin(fRoll));

      vec2_t fDelta;
      fDelta[YAW]   = RAD2DEGF(fAbsDelta * sin(fRoll));
      fDelta[PITCH] = RAD2DEGF(fAbsDelta * cos(fRoll));

      fAbsDelta = (float)_hypot(fDelta[0], fDelta[1]);

      // CRAP IDEA:
      // When we get too close to the target, reset for smaller angles
      if (fTarget < 0.5f * fAbsDelta)
      {
         LastTarget = NULL;                //Start aiming for smaller angles.
         return(GetStealthAnglesToTarget(Target, Angles));
      }

      else
      {
         Angles[0] = fDelta[0] + fKicks[0] - fLastKick[0];
         Angles[1] = fDelta[1] + fKicks[1] - fLastKick[1];

         fLastKick[0] = fKicks[0];
         fLastKick[1] = fKicks[1];
      }

      // When we get too far away from the target, reset for larger angles
      if (fTarget >= fDivisions * fAbsDelta)
      {
         LastTarget = NULL;
         // calling GetStealthAnglesToTarget here would result in infinite recursion
      }

      return(true);
   }
}


void Rotate(bool bFrame)
{
   if (bShouldRotate)
   {
      if (cvar_AutoFire->iValue == 1)
      {
         if (IsInFOV(BestAngles, cvar_AutoFireFOV->Value))
         {
            if (bFrame)
            {
               CG_ConsoleCommand(DS("+attack"));
               CG_ConsoleCommand(DS("-attack"));
            }

            bShouldFire = true;
         }
      }

      if (cvar_AimViewLock->iValue == 1)
      {
         if (*CMouseSensivity != 0.0f)
         {
            oMouseSensivity  = *CMouseSensivity;
            *CMouseSensivity = 0.0f;
         }
      }
      else
      {
         if (oMouseSensivity != FNONCE)
         {
            *CMouseSensivity = oMouseSensivity;
            oMouseSensivity  = FNONCE;
         }
      }

      if (cvar_AutoSight->iValue == 1)
      {
         *Zoom = 1;
         //cg->CurrentSnap->Zoom = 1.0f;
         //cg->NextSnap->Zoom = 1.0f;
      }
   }
   else
   {
      if (oMouseSensivity != FNONCE)
      {
         *CMouseSensivity = oMouseSensivity;
         oMouseSensivity  = FNONCE;
      }
   }
}


CFInline Vector3<_prec> GetAngleDisplacement(int WeaponId, int time)
{
   CWeapon    *pWeapon;
   float      fSpread;
   float dSpread;
   float      minDamageRange;

   Vector3<float> WeaponAngles;
   Vector3<float> _Forward, _Up, h__Right;
   float               fRight, fUp, minSpread, maxSpread, spreadMultiplier;
   float          dRight, dUp;
   pWeapon = GetWeaponByIndex(WeaponId);

   FirstBulletFix();

   /*
   if (bShouldRotate == false && cvar_AntiAim->iValue)
   {
	   return(Vector3<_prec>(((rand() % 81) - 40), 0, 0));
   }
   */

   if (!pWeapon || cvar_NoSpread->iValue == 0)
   {
      return(Vector3<_prec>(0, 0, 0));
   }

   if (cg->CurrentSnap->Zoom > 0.0f)
   {
      if (cvar_NoSway->iValue == 1)
      {
#if 0
         GetWeaponViewAngles(WeaponAngles);
         WeaponAngles.Normalize360();
         SpreadHit = Refdef->ViewAngles;
         SpreadHit.Normalize360();
         SpreadHit -= WeaponAngles;
         //SpreadHit = Refdef->ViewAngles - WeaponAngles;
         SpreadHit.Normalize180();
         (SpreadHit + Refdef->ViewAngles).AngleVectors(&SpreadDisplacement, NULL, NULL);
         return(SpreadHit);
#endif
         SpreadHit = 0.0f;
         return(SpreadHit);
      }
      else
      {
         refdef->ViewAngles.AngleVectors(&SpreadDisplacement, NULL, NULL);
         SpreadHit = 0.0f;
         return(SpreadHit);
      }
   }

   if (cvar_NoSpread->iValue == 1)
   {
	  __asm
	  {
		  push  eax
		  push  edi
		  push  edx
		  push  esi

		  // if (pWeapon->weaponClass == 3) {
		  mov   eax, pWeapon
		  cmp   dword ptr [eax + 0x130], 3
		  jne   ld8192
		  //   minDamageRange = pWeapon->minDamage
		  fld   dword ptr [eax + 0x80C]
		  jmp   store
		  // } else {
		  //   minDamageRange = 8192.0;
ld8192:   fld   dword ptr ds:[0x70BD00]
store:    fstp  minDamageRange
		  // }

		  mov   edx, 0x794474
		  mov   ecx, pWeapon
		  lea   edi, minSpread
		  lea   esi, maxSpread
		  mov   eax, 0x416C70
		  call  eax
		  fld   dword ptr ds:[0x84CAA0]
		  fdiv  qword ptr ds:[0x70AEE0]

		  // fSpread = BaseSpread + ((MaxSpread - BaseSpread) * SpreadMultiplier);
		  fld   maxSpread
		  fsub  minSpread
		  fmulp st(1), st
		  fadd  minSpread

		  // fSpread *= 0.01745329238474369;
		  fmul  qword ptr ds:[0x70B2D0]

		  // dSpread = tan(dSpread);
		  mov   eax, 0x68BC70 // __CItan
		  call  eax
      fld   st
		  fstp  dSpread

		  // dSpread *= minDamageRange;
		  fmul  minDamageRange
		  fstp  dSpread

		  // WeaponAngles.X = *(float*)pWeaponX;
		  fld   dword ptr ds:[pWeaponX]
		  fstp  WeaponAngles.X

		  // WeaponAngles.Y = *(float*)pWeaponY;
		  fld   dword ptr ds:[pWeaponY]
		  fstp  WeaponAngles.Y

		  // WeaponAngles.Z = 0.0f;
		  fldz
		  fstp  WeaponAngles.Z

		  // AngleVectors(WeaponAngles, h__Right, _Forward, _Up);
		  lea   edx, WeaponAngles.X
		  lea   edi, h__Right.X
		  lea   esi, _Forward.X
		  lea   eax, _Up.X
		  push  eax
		  mov   eax, 0x563460
		  call  eax
		  add   esp, 4

		  // GetRandomFloats(time, &fRight, &fUp);
		  lea    eax, fUp
		  push   eax
		  lea    eax, fRight
		  push   eax
		  push   time
		  mov    eax, 0x4A66E0
		  call   eax
		  add    esp, 0xC

		  // dRight = (_high_prec)fRight * dSpread;
		  // dUp    = (_high_prec)fUp * dSpread;
		  fld    fRight
		  fld    dSpread
		  fld    st
		  fmulp  st(2), st
		  fxch   st(1)
		  fstp   dRight
		  fmul   fUp
		  fstp   dUp

		  // SpreadDisplacement.X = (_prec)((_Forward.X * minDamageRange));
		  fld    _Forward.X
		  fmul   minDamageRange
		  fstp   SpreadDisplacement.X

		  // SpreadDisplacement.Y = (_prec)((_Forward.Y * minDamageRange));
		  fld    _Forward.Y
		  fmul   minDamageRange
		  fstp   SpreadDisplacement.Y

		  // SpreadDisplacement.Z = (_prec)((minDamageRange * _Forward.Z));
		  fld    _Forward.Z
		  fmul   minDamageRange
		  fstp   SpreadDisplacement.Z

		  // SpreadDisplacement.X = (_prec)((h__Right.X * dRight) + SpreadDisplacement.X);
		  fld    h__Right.X
		  fmul   dRight
		  fadd   SpreadDisplacement.X
		  fstp   SpreadDisplacement.X

		  // SpreadDisplacement.Y = (_prec)((h__Right.Y * dRight) + SpreadDisplacement.Y);
		  fld    h__Right.Y
		  fmul   dRight
		  fadd   SpreadDisplacement.Y
		  fstp   SpreadDisplacement.Y

		  // SpreadDisplacement.Z = (_prec)((dRight * h__Right.Z) + SpreadDisplacement.Z);
		  fld    h__Right.Z
		  fmul   dRight
		  fadd   SpreadDisplacement.Z
		  fstp   SpreadDisplacement.Z

		  // SpreadDisplacement.X = (_prec)((_Up.X * dUp) + SpreadDisplacement.X);
		  fld    _Up.X
		  fmul   dUp
		  fadd   SpreadDisplacement.X
		  fstp   SpreadDisplacement.X

		  // SpreadDisplacement.Y = (_prec)((_Up.Y * dUp) + SpreadDisplacement.Y);
		  fld    _Up.Y
		  fmul   dUp
		  fadd   SpreadDisplacement.Y
		  fstp   SpreadDisplacement.Y

		  // SpreadDisplacement.Z = (_prec)((dUp * _Up.Z) + SpreadDisplacement.Z);
		  fld    _Up.Z
		  fmul   dUp
		  fadd   SpreadDisplacement.Z
		  fstp   SpreadDisplacement.Z
      
      /*
		  if (pWeapon->weaponClass == 3) {
		    minDamageRange = pWeapon->minDamage
		  } else {
		    minDamageRange = 8192.0;
		  }

		  mov   edx, 0x794474
		  mov   ecx, pWeapon
		  lea   edi, minSpread
		  lea   esi, maxSpread
		  mov   eax, 0x416C70
		  call  eax
		  fld   dword ptr ds:[0x84CAA0]
		  fdiv  qword ptr ds:[0x70AEE0]

		  fSpread = BaseSpread + ((MaxSpread - BaseSpread) * SpreadMultiplier);
		  fSpread *= 0.01745329238474369;
		  dSpread = tan(dSpread);
		  dSpread *= minDamageRange;
		  WeaponAngles.X = *(float*)pWeaponX;
		  WeaponAngles.Y = *(float*)pWeaponY;
		  WeaponAngles.Z = 0.0f;
		  AngleVectors(WeaponAngles, h__Right, _Forward, _Up);
		  GetRandomFloats(time, &fRight, &fUp);
		  dRight = (_high_prec)fRight * dSpread;
		  dUp    = (_high_prec)fUp * dSpread;
		  SpreadDisplacement.X = (_prec)((_Forward.X * minDamageRange));
		  SpreadDisplacement.Y = (_prec)((_Forward.Y * minDamageRange));
		  SpreadDisplacement.Z = (_prec)((minDamageRange * _Forward.Z));
		  SpreadDisplacement.X = (_prec)((h__Right.X * dRight) + SpreadDisplacement.X);
		  SpreadDisplacement.Y = (_prec)((h__Right.Y * dRight) + SpreadDisplacement.Y);
		  SpreadDisplacement.Z = (_prec)((dRight * h__Right.Z) + SpreadDisplacement.Z);
		  SpreadDisplacement.X = (_prec)((_Up.X * dUp) + SpreadDisplacement.X);
		  SpreadDisplacement.Y = (_prec)((_Up.Y * dUp) + SpreadDisplacement.Y);
		  SpreadDisplacement.Z = (_prec)((dUp * _Up.Z) + SpreadDisplacement.Z);
      */
		  
		  pop   esi
		  pop   edx
		  pop   edi
		  pop   eax
	  }
	 }
   else if (cvar_NoSpread->iValue == 2)
   {
	  __asm
	  {
		  push  eax
		  push  edi
		  push  edx
		  push  esi

		  // if (pWeapon->weaponClass == 3) {
		  mov   eax, pWeapon
		  cmp   dword ptr [eax + 0x130], 3
		  jne   ld81922
		  //   minDamageRange = pWeapon->minDamage
		  fld   dword ptr [eax + 0x80C]
		  jmp   store2
		  // } else {
		  //   minDamageRange = 8192.0;
ld81922:   fld   dword ptr ds:[0x70BD00]
store2:    fstp  minDamageRange
		  // }

		  // AngleVectors(WeaponAngles, h__Right, _Forward, _Up);
		  lea   edx, WeaponAngles.X
		  lea   edi, h__Right.X
		  lea   esi, _Forward.X
		  lea   eax, _Up.X
		  push  eax
		  mov   eax, 0x563460
		  call  eax
		  add   esp, 4

		  // GetRandomFloats(time, &fRight, &fUp);
		  lea    eax, fUp
		  push   eax
		  lea    eax, fRight
		  push   eax
		  push   time
		  mov    eax, 0x4A66E0
		  call   eax
		  add    esp, 0xC

		  // WeaponAngles.X = *(float*)pWeaponX;
		  fld   dword ptr ds:[pWeaponX]
		  fstp  WeaponAngles.X

		  // WeaponAngles.Y = *(float*)pWeaponY;
		  fld   dword ptr ds:[pWeaponY]
		  fstp  WeaponAngles.Y

		  // WeaponAngles.Z = 0.0f;
		  fldz
		  fstp  WeaponAngles.Z

		  mov   edx, 0x794474
		  mov   ecx, pWeapon
		  lea   edi, minSpread
		  lea   esi, maxSpread
		  mov   eax, 0x416C70
		  call  eax
		  fld   dword ptr ds:[0x84CAA0]
		  fdiv  qword ptr ds:[0x70AEE0]

		  // fSpread = BaseSpread + ((MaxSpread - BaseSpread) * SpreadMultiplier);
		  fld   maxSpread
		  fsub  minSpread
		  fmulp st(1), st
		  fadd  minSpread
		  //fstp  fSpread

		  // fSpread *= 0.01745329238474369;
		  //fld   fSpread
		  fmul  qword ptr ds:[0x70B2D0]
		  //fstp  dSpread

		  // dSpread = tan(dSpread);
		  //fld   dSpread
		  mov   eax, 0x68BC70 // __CItan
		  call  eax
		  //fstp  dSpread

		  // dSpread *= minDamageRange;
		  //fld   dSpread
		  fmul  minDamageRange
      fld   st
		  //fstp  dSpread

			  fld   st
			  fmul  fRight
			  fstp  fRight
			  fmul  fUp
			  fstp  fUp
		  // dRight = (_high_prec)fRight * dSpread;
		  /* dUp    = (_high_prec)fUp * dSpread;
		  fld    fRight
		  fld    dSpread
		  fld    st
		  fmulp  st(2), st
		  fxch   st(1)
		  fstp   dRight
		  fmul   fUp
		  fstp   dUp
      */

		  // SpreadDisplacement.X = (_prec)((_Forward.X * minDamageRange));
		  fld    _Forward.X
		  fmul   minDamageRange
		  fstp   SpreadDisplacement.X

		  // SpreadDisplacement.Y = (_prec)((_Forward.Y * minDamageRange));
		  fld    _Forward.Y
		  fmul   minDamageRange
		  fstp   SpreadDisplacement.Y

		  // SpreadDisplacement.Z = (_prec)((minDamageRange * _Forward.Z));
		  fld    _Forward.Z
		  fmul   minDamageRange
		  fstp   SpreadDisplacement.Z

		  // SpreadDisplacement.X = (_prec)((h__Right.X * dRight) + SpreadDisplacement.X);
		  fld    h__Right.X
		  fmul   dRight
		  fadd   SpreadDisplacement.X
		  fstp   SpreadDisplacement.X

		  // SpreadDisplacement.Y = (_prec)((h__Right.Y * dRight) + SpreadDisplacement.Y);
		  fld    h__Right.Y
		  fmul   dRight
		  fadd   SpreadDisplacement.Y
		  fstp   SpreadDisplacement.Y

		  // SpreadDisplacement.Z = (_prec)((dRight * h__Right.Z) + SpreadDisplacement.Z);
		  fld    h__Right.Z
		  fmul   dRight
		  fadd   SpreadDisplacement.Z
		  fstp   SpreadDisplacement.Z

		  // SpreadDisplacement.X = (_prec)((_Up.X * dUp) + SpreadDisplacement.X);
		  fld    _Up.X
		  fmul   dUp
		  fadd   SpreadDisplacement.X
		  fstp   SpreadDisplacement.X

		  // SpreadDisplacement.Y = (_prec)((_Up.Y * dUp) + SpreadDisplacement.Y);
		  fld    _Up.Y
		  fmul   dUp
		  fadd   SpreadDisplacement.Y
		  fstp   SpreadDisplacement.Y

		  // SpreadDisplacement.Z = (_prec)((dUp * _Up.Z) + SpreadDisplacement.Z);
		  fld    _Up.Z
		  fmul   dUp
		  fadd   SpreadDisplacement.Z
		  fstp   SpreadDisplacement.Z
		  
		  pop   esi
		  pop   edx
		  pop   edi
		  pop   eax
	  }
	  }

      set64bitpc();
      WeaponAngles.Normalize360();
      SpreadHit = WeaponAngles - SpreadDisplacement.DAngles();
      SpreadHit.Normalize180();

      return(SpreadHit);
}

float aaFix=0.0f;
DWORD        dwCG_CalcEntityLerpPositions = 0x00434FE0;
DWORD        dwCG_SetFrameInterpolation = 0x433910;

void TouchUserCmds()
{
   pLocalWeapon = GetWeaponByIndex(pLocalEnt->WeaponId);
   if (pLocalEnt && pLocalWeapon && pLocalEnt->Alive && pLocalEnt->IsValidEnt && (pLocalEnt->EntityType == TYPE_HUMAN) && cg->CurrentSnap && cg->NextSnap && !cg->demoPlayback)
   {
#ifndef VISUALS
      InitializeGame();
#endif // VISUALS
      DWORD    currentCmdNum = *pCurrentCmdNum;
      CUsercmd *curUserCmd   = GET_UCMD(currentCmdNum);

      if (!curUserCmd || (GET_OCMD(currentCmdNum)->serverTime == curUserCmd->serverTime))
      {
#ifdef COMPILE_DEBUG
         if (cvar_Debug->iValue == 1)
         {
            Log("BASE TEST ID 0 FAIL\n");
         }
#endif   // COMPILE_DEBUG
         return;
      }

      UsercmdCopy(curUserCmd, GET_OCMD(currentCmdNum));

#ifdef H47
      Update_47();
#endif // H47

      AimbotPreCalc();

      if ((dwTargetID == -1) && bAimbotIsReady())
      {
         if (cvar_PlayerAim->iValue == 1)
         {
            for (int i = 0; i < MAX_PLAYERS; ++i)
            {
               CEntity     *pEnt    = GetEntityByIndex(i);
               CClientInfo *pClient = GetClientByIndex(i);

			   // get oldpos, where cg_time = cg->snap->serverTime
			   // then get newpos, where cg_time = cg->nextsnap->servertime
			   // and ppos, where cg_time = hittime

               if (pEnt->Alive && pEnt->IsValidEnt && (i != cg->clientNum) && (pEnt->EntityType == TYPE_HUMAN) && ((pClient->Team == TEAM_FFA) || (pClient->Team != pLocalClient->Team)) && !cg->demoPlayback)
               {
                  float fDist = fDistance[i];

                  if (((cvar_AimRange->Value == 0.0f) || (fDist <= cvar_AimRange->Value)))
				  {
					  DWORD oldtime = *(DWORD*)0x794460;
					  *(DWORD*)0x794460 = (*(DWORD*)0xC628D0) + cvar_TimeAdditive->Value;
					  DWORD li = cg->clientNum;
					__asm
					{
						call [dwCG_SetFrameInterpolation]
						push pEnt
						push li
						call [dwCG_CalcEntityLerpPositions]
						add esp, 8
					}

					AimAtPlayerEntity(pEnt);

					*(DWORD*)0x794460 = oldtime;

					__asm
					{
						call [dwCG_SetFrameInterpolation]
						push pEnt
						push li
						call [dwCG_CalcEntityLerpPositions]
						add esp, 8
					}
                  }
               }
            }
         }
      }

      AimbotEndCalcs();
      Rotate();

      CUsercmd *lastUserCmd = GET_UCMD(currentCmdNum - 1);
      UsercmdCopy(lastUserCmd, GET_OCMD(currentCmdNum - 1));

      CUsercmd *userCmdBeforeLast = GET_UCMD(currentCmdNum - 2);
      UsercmdCopy(userCmdBeforeLast, GET_OCMD(currentCmdNum - 2));

      Vector3<_prec> New, CmdAng((float)SHORT2ANGLE(curUserCmd->angles[0]), (float)SHORT2ANGLE(curUserCmd->angles[1]), (float)SHORT2ANGLE(curUserCmd->angles[2]));

      CmdAng.Normalize180();
      New = CmdAng;
	  bool hackedCmds = false;

      if (bShouldFire || curUserCmd->buttons & 1)          // TODO: #define BUTTON_FIRE 1
      {
         if ((cvar_FullAuto->iValue == 1) || bShouldFire &&
			 (cvar_FullAutoFOV->iValue == 0 || bShouldFire))
         {
            lastUserCmd->buttons &= ~1;                   // TODO: #define BUTTON_FIRE 1
            curUserCmd->buttons  |= 1;                    // TODO: #define BUTTON_FIRE 1
         }

         DWORD weaponIndex;
         if(_cg->predictedPlayerState.weapFlags & 2)
            weaponIndex = _cg->predictedPlayerState.offHandIndex;
         else
            weaponIndex = _cg->predictedPlayerState.weapon;

		  if (cvar_NoSpread->iValue >= 1 && bShouldRotate)
		  {
			  Vector3<_prec> spreadDisplacement = GetAngleDisplacement(weaponIndex, *(int*)0x839248);

			  __asm
			  {
				  // New.X += BestAngles.X + spreadDisplacement.X
				  fld    spreadDisplacement.X
				  fadd   BestAngles.X
				  fadd   New.X
				  fstp   New.X

				  // New.Y += BestAngles.Y + spreadDisplacement.Y
				  fld    spreadDisplacement.Y
				  fadd   BestAngles.Y
				  fadd   New.Y
				  fstp   New.Y

				  // New.Z = 0.0f
				  fldz
				  fstp   New.Z
			  }

			 New.Normalize180();

			 SAFE_CVAR_WRITE_DWORD(real_cl_maxpackets,100);
			 SAFE_CVAR_WRITE_DWORD(real_snaps,30);
			 SAFE_CVAR_WRITE_DWORD(real_cl_packetdup,5);
		  }
         else if (bShouldRotate)
         {
            set64bitpc();
            New += BestAngles;
            New.Z = 0.0f;
            New.Normalize180();
#ifndef VISUALS
            if (cvar_SilentAim->iValue == 0)
            {
               AddViewAngleDelta(BestAngles);
            }
#endif
         }
		 else if (cvar_NoSpread->iValue >= 1)
		 {
            New += GetAngleDisplacement(weaponIndex, *(int*)0x839248);
            New.Z = 0.0f;
            New.Normalize180();
		 }

		 set24bitpc();

		 if (cvar_NoRecoil &&
			 cvar_NoRecoil->iValue != 0)
		 { 
			*(float*)0x79E748 = 0;
			*(float*)0x79E74C = 0;
			*(float*)0x79E750 = 0;
			*(float*)0x79E754 = 0;
			*(float*)0x79E758 = 0;
			*(float*)0x79E75C = 0;
			*(float*)0x79E760 = 0;
			*(float*)0x79E764 = 0;
			*(float*)0x79E768 = 0;
		 }

         lastUserCmd->angles[0]  = ANGLE2SHORT(New.X);
         lastUserCmd->angles[1]  = ANGLE2SHORT(New.Y);
         lastUserCmd->angles[2]  = ANGLE2SHORT(New.Z);
		 curUserCmd->serverTime  = (*(int *)0xC628D0);
         lastUserCmd->serverTime = curUserCmd->serverTime - 1;
         GET_OCMD(currentCmdNum - 1)->serverTime = lastUserCmd->serverTime;

		 hackedCmds = true;

         if (cvar_SilentAim->iValue == 0
#ifdef VISUALS
             && cvar_SmoothAim->iValue == 0
#endif
             )
         {
            New = CmdAng + BestAngles;
            New.Normalize180();
            curUserCmd->angles[0] = ANGLE2SHORT(New.X);
            curUserCmd->angles[1] = ANGLE2SHORT(New.Y);
            curUserCmd->angles[2] = ANGLE2SHORT(New.Z);
         }
      }

	  if (hackedCmds == false &&
		  cvar_AntiAim &&
		  cvar_AntiAim->iValue == 1)
	  {
		 CmdAng.X = (float)SHORT2ANGLE(curUserCmd->angles[0]);
		  CmdAng.Y = (float)SHORT2ANGLE(curUserCmd->angles[1]);
		  CmdAng.Z = (float)SHORT2ANGLE(curUserCmd->angles[2]);
		  aaFix = CmdAng.X;
		 CmdAng.Normalize180();
		 New = CmdAng;
		 New.X += 9.f * ((rand() % 61) - 30);
		 New.Normalize180();
         curUserCmd->angles[0] = ANGLE2SHORT(New.X);
	  }
	  else
	  {
		  aaFix = 0.0f;
	  }

#ifdef VISUALS
      if (hackedCmds == false &&
		  cvar_Spin->iValue == 1)
      {
         aZ.Z += cvar_SpinDegrees->Value;
         aZ.Normalize180();

         //lastUserCmd->angles[2] = ANGLE2SHORT(aZ.Z);
         curUserCmd->angles[2] = ANGLE2SHORT(aZ.Z);
      }
#endif // VISUALS
   }
}
