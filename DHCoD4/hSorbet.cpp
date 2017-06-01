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

#define S_MAX_BONES 3

char *sBones[S_MAX_BONES] =
{
	0, // reserved for entity pos
	"j_head",
	"j_spineupper"
};

short bones[S_MAX_BONES];
#define NUM_SNAPS 3
Vector3<float> pos[MAX_PLAYERS][S_MAX_BONES][NUM_SNAPS], ppos[MAX_PLAYERS][S_MAX_BONES][NUM_SNAPS];
DWORD time[MAX_PLAYERS][S_MAX_BONES][NUM_SNAPS];

void Invalidate_Player(size_t i)
{
	for (size_t j = 0; j < S_MAX_BONES; j++)
	{
		for (size_t k = 0; k < NUM_SNAPS; k++)
		{
			pos[i][j][k]  = FNONCE;
			ppos[i][j][k] = FNONCE;
			time[i][j][k] = -1;
		}
	}
}

void Init_Sorbet()
{
	for (size_t i = 1; i < S_MAX_BONES; i++)
	{
		bones[i] = R_RegisterTag(sBones[i]);
	}

	for (size_t i = 0; i < MAX_PLAYERS; i++)
	{
		Invalidate_Player(i);
	}
}

void Update_Sorbet()
{
	DWORD now = GetTickCount();

	// information gathering step
	for (size_t i = 0; i < MAX_PLAYERS; i++)
	{
		CEntity     *pEnt    = GetEntityByIndex(i);
		CClientInfo *pClient = GetClientByIndex(i);

		if (pEnt->Alive && pEnt->IsValidEnt && (i != cg->clientNum) && (pEnt->EntityType == TYPE_HUMAN) && ((pClient->Team == TEAM_FFA) || (pClient->Team != pLocalClient->Team)) && !cg->demoPlayback)
		{
			pos[i][0][0] = pEnt->vOldOrigin;
			pos[i][0][1] = pEnt->vOrigin;
			pos[i][0][2] = pEnt->vNewOrigin;

			time[i][0][0] = time[i][0][1] = time[i][0][2] = now;

			for (size_t j = 1, k; j < S_MAX_BONES; j++)
			{
				for (k = 0; k < NUM_SNAPS; k++)
				{
					if (pos[i][j][k] == FNONCE)
					{
						break;
					}
				}

				if (k == NUM_SNAPS)
				{
					k = 0; // FIXME: k should instead be set to the oldest snap
				}

				GetPlayerTag(bones[j], pEnt, &pos[i][j][k]);
				time[i][j][k] = now;
			}
		}
		else
		{
			Invalidate_Player(i);
		}
	}

	// now parsing step - prioritize targets with more information, calculate & extract data, etc
	for (size_t i = 0; i < MAX_PLAYERS; i++)
	{
		for (size_t j = 0, k; j < S_MAX_BONES; j++)
		{
			DWORD  latest = 0;
			size_t latestIndex = 0;

			for (k = 0; k < NUM_SNAPS; k++)
			{
				if (time[i][j][k] > latest)
				{
					latest      = time[i][j][k];
					latestIndex = k;
				}
			}

			for (k = 0; k < NUM_SNAPS; k++)
			{

			}
			// after snaps are ordered, we can calculate delta/velocity/acceleration.
			// we also calculate target selection values here - particularly how many snaps have been took, how recent they are, general velocity, distance, etc
		}
	}

	// final step is now simple: choose best target and aim
}