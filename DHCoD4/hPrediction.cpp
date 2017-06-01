
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

class CFrame
{
public:
	DWORD         time;
	Vector3<_prec> pos[MAX_PLAYERS][4];
};

CFrame frames[32], *curFrame, *lastFrame, *lastFrame2;
DWORD currentFrameNum;

/*
v23 = *(_DWORD *)(ent + 0xCC);
if ( CG_GetBoneIndex(localClientNum, boneName, v23, (unsigned int *)clientNum)
&& FX_GetBoneOrientation(v23, (unsigned __int8)clientNum[0], localClientNum, (int)&orientation) )
{
*/

#pragma pack(push, 1)
typedef struct
{
	float origin[3];
	float axis[3][3];
} orientation_t;
#pragma pack(pop)

void *GetEntityDObj(CEntity *cent)
{
	return NULL;
}
void CG_GetBoneIndex();
void FX_GetBoneOrientation();
void CG_GetTagOrientation(CEntity *cent, unsigned int boneName, orientation_t *orientation);

void ApplyPrediction3(bool update, CEntity *ent, Vector3<double> *to)
{
	if (cvar_AimPrediction3->Value == 0.0f &&
		cvar_AimAcceleration->Value == 0.0f)
	{
		return;
	}
	/*
	DWORD i             = ent->ClientNumber;
	int   lastFrameNum  = currentFrameNum - 1;
	int   lastFrameNum2 = currentFrameNum - 1;
	DWORD curTime       = GetTickCount();

	if (lastFrameNum == -1)
	{
		lastFrameNum = 63;
	}

	if (lastFrameNum2 < 0)
	{
		lastFrameNum2 += 64;
	}

	if ((NULL == curFrame ||
		NULL == lastFrame ||
		curTime != curFrame->time) &&
		update)
	{
		curFrame  = &frames[currentFrameNum++];
		lastFrame = &frames[lastFrameNum];

		if (currentFrameNum == 64)
		{
			currentFrameNum = 0;
		}

		curFrame->time = curTime;

		memset((void*)&curFrame->pos[0][0].X, 0, sizeof(curFrame->pos));
	}
	else
	{
		if (update == false &&
			NULL == curFrame)
		{
			return;
		}
	}
	
	if (curFrame->pos[i][0] == 0.0f)
	{
		curFrame->pos[i][0] = GetPlayerTagAccurateSafe(aimTags[0], ent); // head
		curFrame->pos[i][1] = GetPlayerTagAccurateSafe(aimTags[18], ent); // helmet
		curFrame->pos[i][2] = GetPlayerTagAccurateSafe(aimTags[17], ent); // neck
		curFrame->pos[i][3] = GetPlayerTagAccurateSafe(aimTags[19], ent); // eye
	}

	if ((curTime - lastFrame->time) < 100 &&
		NULL != to)
	{
		int predictionTime = (*(int*)0xC628D0) + cvar_TimeAdditive->Value;

		if (predictionTime > _cg->nextSnap->serverTime)
		{
			predictionTime = _cg->nextSnap->serverTime;
		}
		else if (predictionTime < _cg->snap->serverTime)
		{
			predictionTime = _cg->snap->serverTime;
		}

#if 0
		to->X = cvar_AimPrediction3->Value * ((((curFrame->pos[i][0].X - lastFrame->pos[i][0].X) + (curFrame->pos[i][1].X - lastFrame->pos[i][1].X) + (curFrame->pos[i][2].X - lastFrame->pos[i][2].X) + (curFrame->pos[i][3].X - lastFrame->pos[i][3].X)) / 4.0f) / (curFrame->time - lastFrame->time)) * (float)((predictionTime) - _cg->snap->serverTime);
		to->Y = cvar_AimPrediction3->Value * ((((curFrame->pos[i][0].Y - lastFrame->pos[i][0].Y) + (curFrame->pos[i][1].Y - lastFrame->pos[i][1].Y) + (curFrame->pos[i][2].Y - lastFrame->pos[i][2].Y) + (curFrame->pos[i][3].Y - lastFrame->pos[i][3].Y)) / 4.0f) / (curFrame->time - lastFrame->time)) * (float)((predictionTime) - _cg->snap->serverTime);
		to->Z = cvar_AimPrediction3->Value * ((((curFrame->pos[i][0].Z - lastFrame->pos[i][0].Z) + (curFrame->pos[i][1].Z - lastFrame->pos[i][1].Z) + (curFrame->pos[i][2].Z - lastFrame->pos[i][2].Z) + (curFrame->pos[i][3].Z - lastFrame->pos[i][3].Z)) / 4.0f) / (curFrame->time - lastFrame->time)) * (float)((predictionTime) - _cg->snap->serverTime);
#endif
		if (lastFrame->pos[i][0].X != 0.0f)
		{
			if (cvar_AimPrediction3Mode->iValue == 0)
			{
				to->X += cvar_AimPrediction3->Value * (((curFrame->pos[i][0].X - lastFrame->pos[i][0].X) + (curFrame->pos[i][1].X - lastFrame->pos[i][1].X) + (curFrame->pos[i][2].X - lastFrame->pos[i][2].X) + (curFrame->pos[i][3].X - lastFrame->pos[i][3].X)) / 4.0f);
				to->Y += cvar_AimPrediction3->Value * (((curFrame->pos[i][0].Y - lastFrame->pos[i][0].Y) + (curFrame->pos[i][1].Y - lastFrame->pos[i][1].Y) + (curFrame->pos[i][2].Y - lastFrame->pos[i][2].Y) + (curFrame->pos[i][3].Y - lastFrame->pos[i][3].Y)) / 4.0f);
				to->Z += cvar_AimPrediction3->Value * (((curFrame->pos[i][0].Z - lastFrame->pos[i][0].Z) + (curFrame->pos[i][1].Z - lastFrame->pos[i][1].Z) + (curFrame->pos[i][2].Z - lastFrame->pos[i][2].Z) + (curFrame->pos[i][3].Z - lastFrame->pos[i][3].Z)) / 4.0f);
			}
			else
			{
				to->X += cvar_AimPrediction3->Value * (((curFrame->pos[i][0].X - lastFrame->pos[i][0].X) * cvar_AimHeadScalar->Value) + ((curFrame->pos[i][1].X - lastFrame->pos[i][1].X) * cvar_AimHelmetScalar->Value) + ((curFrame->pos[i][2].X - lastFrame->pos[i][2].X) * cvar_AimNeckScalar->Value) + ((curFrame->pos[i][3].X - lastFrame->pos[i][3].X) * cvar_AimEyeScalar->Value));
				to->Y += cvar_AimPrediction3->Value * (((curFrame->pos[i][0].Y - lastFrame->pos[i][0].Y) * cvar_AimHeadScalar->Value) + ((curFrame->pos[i][1].Y - lastFrame->pos[i][1].Y) * cvar_AimHelmetScalar->Value) + ((curFrame->pos[i][2].Y - lastFrame->pos[i][2].Y) * cvar_AimNeckScalar->Value) + ((curFrame->pos[i][3].Y - lastFrame->pos[i][3].Y) * cvar_AimEyeScalar->Value));
				to->Z += cvar_AimPrediction3->Value * (((curFrame->pos[i][0].Z - lastFrame->pos[i][0].Z) * cvar_AimHeadScalar->Value) + ((curFrame->pos[i][1].Z - lastFrame->pos[i][1].Z) * cvar_AimHelmetScalar->Value) + ((curFrame->pos[i][2].Z - lastFrame->pos[i][2].Z) * cvar_AimNeckScalar->Value) + ((curFrame->pos[i][3].Z - lastFrame->pos[i][3].Z) * cvar_AimEyeScalar->Value));
			}

			if (cvar_AimAcceleration->Value > 0.0f &&
				(curTime - lastFrame2->time) < 100 &&
				lastFrame2->pos[i][0].X != 0.0f)
			{
				if (cvar_AimPrediction3Mode->iValue == 0)
				{
					to->X += cvar_AimAcceleration->Value * ((((curFrame->pos[i][0].X - lastFrame->pos[i][0].X) + (curFrame->pos[i][1].X - lastFrame->pos[i][1].X) + (curFrame->pos[i][2].X - lastFrame->pos[i][2].X) + (curFrame->pos[i][3].X - lastFrame->pos[i][3].X)) / 4.0f) - (((lastFrame->pos[i][0].X - lastFrame2->pos[i][0].X) + (lastFrame->pos[i][1].X - lastFrame2->pos[i][1].X) + (lastFrame->pos[i][2].X - lastFrame2->pos[i][2].X) + (lastFrame->pos[i][3].X - lastFrame2->pos[i][3].X)) / 4.0f));
					to->Y += cvar_AimAcceleration->Value * ((((curFrame->pos[i][0].Y - lastFrame->pos[i][0].Y) + (curFrame->pos[i][1].Y - lastFrame->pos[i][1].Y) + (curFrame->pos[i][2].Y - lastFrame->pos[i][2].Y) + (curFrame->pos[i][3].Y - lastFrame->pos[i][3].Y)) / 4.0f) - (((lastFrame->pos[i][0].Y - lastFrame2->pos[i][0].Y) + (lastFrame->pos[i][1].Y - lastFrame2->pos[i][1].Y) + (lastFrame->pos[i][2].Y - lastFrame2->pos[i][2].Y) + (lastFrame->pos[i][3].Y - lastFrame2->pos[i][3].Y)) / 4.0f));
					to->Z += cvar_AimAcceleration->Value * ((((curFrame->pos[i][0].Z - lastFrame->pos[i][0].Z) + (curFrame->pos[i][1].Z - lastFrame->pos[i][1].Z) + (curFrame->pos[i][2].Z - lastFrame->pos[i][2].Z) + (curFrame->pos[i][3].Z - lastFrame->pos[i][3].Z)) / 4.0f) - (((lastFrame->pos[i][0].Z - lastFrame2->pos[i][0].Z) + (lastFrame->pos[i][1].Z - lastFrame2->pos[i][1].Z) + (lastFrame->pos[i][2].Z - lastFrame2->pos[i][2].Z) + (lastFrame->pos[i][3].Z - lastFrame2->pos[i][3].Z)) / 4.0f));
				}
				else
				{
					to->X += cvar_AimAcceleration->Value * ((((curFrame->pos[i][0].X - lastFrame->pos[i][0].X) * cvar_AimHeadScalar->Value) + ((curFrame->pos[i][1].X - lastFrame->pos[i][1].X) * cvar_AimHelmetScalar->Value) + ((curFrame->pos[i][2].X - lastFrame->pos[i][2].X) * cvar_AimNeckScalar->Value) + ((curFrame->pos[i][3].X - lastFrame->pos[i][3].X) * cvar_AimEyeScalar->Value)) - (((lastFrame->pos[i][0].X - lastFrame2->pos[i][0].X) * cvar_AimHeadScalar->Value) + ((lastFrame->pos[i][1].X - lastFrame2->pos[i][1].X) * cvar_AimHelmetScalar->Value) + ((lastFrame->pos[i][2].X - lastFrame2->pos[i][2].X) * cvar_AimNeckScalar->Value) + ((lastFrame->pos[i][3].X - lastFrame2->pos[i][3].X) * cvar_AimEyeScalar->Value)));
					to->Y += cvar_AimAcceleration->Value * ((((curFrame->pos[i][0].Y - lastFrame->pos[i][0].Y) * cvar_AimHeadScalar->Value) + ((curFrame->pos[i][1].Y - lastFrame->pos[i][1].Y) * cvar_AimHelmetScalar->Value) + ((curFrame->pos[i][2].Y - lastFrame->pos[i][2].Y) * cvar_AimNeckScalar->Value) + ((curFrame->pos[i][3].Y - lastFrame->pos[i][3].Y) * cvar_AimEyeScalar->Value)) - (((lastFrame->pos[i][0].Y - lastFrame2->pos[i][0].Y) * cvar_AimHeadScalar->Value) + ((lastFrame->pos[i][1].Y - lastFrame2->pos[i][1].Y) * cvar_AimHelmetScalar->Value) + ((lastFrame->pos[i][2].Y - lastFrame2->pos[i][2].Y) * cvar_AimNeckScalar->Value) + ((lastFrame->pos[i][3].Y - lastFrame2->pos[i][3].Y) * cvar_AimEyeScalar->Value)));
					to->Z += cvar_AimAcceleration->Value * ((((curFrame->pos[i][0].Z - lastFrame->pos[i][0].Z) * cvar_AimHeadScalar->Value) + ((curFrame->pos[i][1].Z - lastFrame->pos[i][1].Z) * cvar_AimHelmetScalar->Value) + ((curFrame->pos[i][2].Z - lastFrame->pos[i][2].Z) * cvar_AimNeckScalar->Value) + ((curFrame->pos[i][3].Z - lastFrame->pos[i][3].Z) * cvar_AimEyeScalar->Value)) - (((lastFrame->pos[i][0].Z - lastFrame2->pos[i][0].Z) * cvar_AimHeadScalar->Value) + ((lastFrame->pos[i][1].Z - lastFrame2->pos[i][1].Z) * cvar_AimHelmetScalar->Value) + ((lastFrame->pos[i][2].Z - lastFrame2->pos[i][2].Z) * cvar_AimNeckScalar->Value) + ((lastFrame->pos[i][3].Z - lastFrame2->pos[i][3].Z) * cvar_AimEyeScalar->Value)));
				}
			}
		}
	}
	*/
}
