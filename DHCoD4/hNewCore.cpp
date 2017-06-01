#include "hNewCore.h"
#include "hClasses.h"
#include "hConfig.h"
#include "hBot.h"

cg_s *_cg = (cg_s*)0x0074E338;
clientActive_t *_cl = (clientActive_t*)0x00C5F930;

#if 0
void GetEntityVelocity(int time, CEntity *pEnt, Vector3<_prec> *out)
{
   Vector3<_prec> accel;
#if 0
   entityState_s *newEntity = &_cg->nextSnap->entities[pEnt->ClientNumber], *entity = &_cg->snap->entities[pEnt->ClientNumber];

   *out = newEntity->lerp.pos.trBase - entity->lerp.pos.trBase;
#endif
#if 0
   float divisor,scalar;
   divisor = (float)(_cg->nextSnap->serverTime - _cg->snap->serverTime);
   scalar = (float)((_cg->nextSnap->serverTime - _cg->snap->serverTime) + (_cg->nextSnap->serverTime - time));
   
   out->X = ((pEnt->vNewOrigin.X - pEnt->vOldOrigin.X) / divisor) * scalar;
   out->Y = ((pEnt->vNewOrigin.Y - pEnt->vOldOrigin.Y) / divisor) * scalar;
   out->Z = ((pEnt->vNewOrigin.Z - pEnt->vOldOrigin.Z) / divisor) * scalar;
#endif
   *out = pEnt->vNewOrigin - pEnt->vOldOrigin;
#if 0
   // consider never interpolating(time < cg->nextSnap->serverTime), but only extrapolating(time > cg->nextSnap->serverTime)
   // i don't know if the extrapolation condition would ever be true though...
   float scalar;
   scalar = (float)((_cg->nextSnap->serverTime - _cg->snap->serverTime) + (_cg->nextSnap->serverTime - time)) * (1.f / (float)(_cg->nextSnap->serverTime - _cg->snap->serverTime));
   
   out->X = (pEnt->vNewOrigin.X - pEnt->vOldOrigin.X) * scalar;
   out->Y = (pEnt->vNewOrigin.Y - pEnt->vOldOrigin.Y) * scalar;
   out->Z = (pEnt->vNewOrigin.Z - pEnt->vOldOrigin.Z) * scalar;
#endif

   //accel = *out - (entity->nextState.lerp.pos.trBase - entity->currentState.pos.trBase);
   accel = 0.0f;

   out->X = ((out->X * cvar_AimPredictScalarX->Value) + (accel.X * cvar_AimAcceleration->Value)) * cvar_AimPrediction->Value;
   out->Y = ((out->Y * cvar_AimPredictScalarY->Value) + (accel.Y * cvar_AimAcceleration->Value)) * cvar_AimPrediction->Value;
   out->Z = ((out->Z * cvar_AimPredictScalarZ->Value) + (accel.Z * cvar_AimAcceleration->Value)) * cvar_AimPrediction->Value;
}
#endif
