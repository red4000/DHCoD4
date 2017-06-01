// TODO: fix H47 bug... seems to aim 180 degrees off for some reason

#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hDefines.h"

#ifdef H47

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hConfig.h"
#include "hBot.h"
#include "hAim.h"
#include "h47.h"
#include "hLog.h"

#define EXTREME_VEL    50.0f
#define ACCEL_SCALE    1.05f
#define VEL_SCALE      0.9

HVictim vic[64];

void Logvec(char *vecname, Vector3<double> *pvec)
{
   Logf("%s(%f, %f, %f)\n", vecname, pvec->X, pvec->Y, pvec->Z);
}


void NullTrajectory(HTrajectory *out)
{
   out->accel = out->pos = out->vel = out->time = 0.0f;
}


void NullVic(HVictim *pvic)
{
   NullTrajectory(&pvic->old);
   NullTrajectory(&pvic->nw);
   NullTrajectory(&pvic->pred);
   for (unsigned int i = 0; i < H_MAX_BONES; ++i)
   {
      NullTrajectory(&pvic->bone[i].old);
      NullTrajectory(&pvic->bone[i].nw);
      NullTrajectory(&pvic->bone[i].pred);
   }
}


void FillTrajectory(HTrajectory *out, Vector3<H47_PREC> *pos)
{
   out->time = (H47_PREC)(*PhysicsTime);
   out->pos  = *pos;
}


void FillTrajectory(HTrajectory *out, Vector3<float> *pos)
{
   out->time = (H47_PREC)(*PhysicsTime);
   out->pos  = Vector3<H47_PREC>((H47_PREC)pos->X, (H47_PREC)pos->Y, (H47_PREC)pos->Z);
}


void UpdateTrajectory(HVictim *pvic, HTrajectory *old, HTrajectory *nw, HTrajectory *out) // call with out = nw
{
   if (&pvic->nw != out)
   {
      out->vel = (nw->pos - old->pos) - pvic->nw.vel;
   }
   else
   {
      out->vel = nw->pos - old->pos;
   }

   if (out->vel.Length() >= EXTREME_VEL)
   {
      out->vel = old->vel;
      if (out->vel.Length() >= EXTREME_VEL)
      {
         old->vel = 0.0f;
         out->vel = 0.0f;
      }
   }

   out->accel = nw->vel - old->vel;

   out->accel *= ACCEL_SCALE;
   out->vel   *= VEL_SCALE;
}


void PredictTrajectory(HVictim *pvic, HTrajectory *old, HTrajectory *nw, HTrajectory *out)
{
   Vector3<H47_PREC> acceldelta = nw->accel - old->accel;
   Vector3<H47_PREC> veldelta   = nw->vel - old->vel;

   out->accel = nw->accel + acceldelta;
   out->vel   = nw->vel + veldelta;

   if (out != &pvic->pred)
   {
      out->pos = nw->pos + ((pvic->nw.vel + pvic->nw.accel) + (out->vel + out->accel));
   }
   else
   {
      out->pos = nw->pos + (out->vel + out->accel);
   }
}


void UpdateBone(HVictim *pvic, HBone *pbone)
{
   CEntity *pEnt = GetEntityByIndex(pvic->i);

   Vector3<float>    tfloatvec = GetPlayerTagAccurateSafe(pbone->id, pEnt);
   Vector3<H47_PREC> t47vec((H47_PREC)tfloatvec.X, (H47_PREC)tfloatvec.Y, (H47_PREC)tfloatvec.Z);

   pbone->old = pbone->nw;

   FillTrajectory(&pbone->nw, &t47vec);

   UpdateTrajectory(pvic, &pbone->old, &pbone->nw, &pbone->nw);
   PredictTrajectory(pvic, &pbone->old, &pbone->nw, &pbone->pred);
}


void UpdateVic(HVictim *pvic, CEntity *pEnt)
{
   pvic->old = pvic->nw;      // TODO: think of some rule to prevent data from getting lost between the various calls...

   if (pvic->old.pos == 0.0f)
   {
      FillTrajectory(&pvic->old, &pEnt->vOrigin);  // TODO: decide if using vOldOrigin would be better here
   }
   FillTrajectory(&pvic->nw, &pEnt->vOrigin);      // not vNewOrigin, because animations are interpolated between old & new, and so their velocity are dependant on the interpolation

   UpdateTrajectory(pvic, &pvic->old, &pvic->nw, &pvic->nw);
   PredictTrajectory(pvic, &pvic->old, &pvic->nw, &pvic->pred);

   for (unsigned int i = 0; i < H_MAX_BONES; ++i)
   {
      pvic->bone[i].id = aimTags[i];
      UpdateBone(pvic, &pvic->bone[i]);
   }

   Vector3<float> tmpentvel = (pEnt->vNewOrigin - pEnt->vOldOrigin);
   pvic->pred.vel *= 0.3;
   pvic->pred.vel += Vector3<H47_PREC>((H47_PREC)tmpentvel.X, (H47_PREC)tmpentvel.Y, (H47_PREC)tmpentvel.Z) * 0.7;
   //pvic->pred.vel = (pEnt->vNewOrigin - pEnt->vOldOrigin);
   // TODO: more accurate predicting of REAL entity velocity
}


void Update_47()
{
   for (unsigned int i = 0; i < 64; ++i)
   {
      vic[i].i = i;
      CEntity     *pEnt    = GetEntityByIndex(i);
      CClientInfo *pClient = GetClientByIndex(i);

      if (pEnt->Alive && pEnt->IsValidEnt && (pEnt->EntityType == TYPE_HUMAN) && ((pClient->Team == TEAM_FFA) || (pClient->Team != pLocalClient->Team) || (i == cg->clientNum)))
      {
         UpdateVic(&vic[i], pEnt);
      }
      else
      {
         NullVic(&vic[i]);
      }
   }
}


#endif // H47
