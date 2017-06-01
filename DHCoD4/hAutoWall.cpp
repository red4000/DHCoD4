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


static float (__cdecl*BG_ComparePower)(float f1, float f2 )=(float(__cdecl*)(float,float))0x409400;
static char (__cdecl*Com_MemCpy)(void* bt, void* tr, int count)=(char(__cdecl*)(void*,void*,int))0x564DA0;

void SetupBulletTrace2(CBulletTrace* btr, CTrace* tr, Vector3<_prec>& start, Vector3<_prec>& end, int skipNum);
bool BG_AdvanceTrace(CBulletTrace* bulletTrace, CTrace* trace, float factor);
float BG_GetSurfacePenetrationDepth(int materialType);
unsigned char BulletTrace(CTrace* trace, CBulletTrace* bulletTrace, CEntity* cent, int materialType);
bool CheckPerkDeepImpact();

Vector3<float> nulvec(0,0,0);
bool hitTarget;
CEntity *traceEnt;
float length;
DWORD entNum, dwRestore;
Vector3<float> endPos, tmp;

CFInline void SetupBulletTrace(CBulletTrace *btr, Vector3<_prec>& start, Vector3<_prec>& end, int skipNum, CWeapon *wep)
{
   btr->weaponEntIndex   = 1022;
   btr->ignoreEntIndex   = skipNum;
   btr->damageMultiplier = 1.0f;
   btr->methodOfDeath    = wep->impactType;
   btr->origStart        = btr->start = start;
   btr->end = end;
   btr->dir = (end - start).Unitize();
}


float BG_GetSurfacePenetrationDepth2( CWeapon* weapon, int materialType )
{
	float*m_flDamageTable = reinterpret_cast< float* >( *reinterpret_cast< PDWORD_PTR >( weapon )  + 0x600 );
	int m_iDamageIndex = ( ( materialType >> 0x14 ) & 0x1F ) + ( weapon->penetrateType * 0x1D );
	BOOL m_bDeepImpact = ( LOBYTE( *( PDWORD_PTR )( ( *( PDWORD_PTR )( 0x84F2D8 + 0x158 ) * 0x4CC ) + 0x83929C ) >> 0x5 ) == 1 );

	if( m_bDeepImpact )
		return ( m_flDamageTable[m_iDamageIndex] * *( FLOAT* )( *( PDWORD )0x736B90 + 0xC ) );
	return m_flDamageTable[m_iDamageIndex];
}

int GetSourceEntityNum( CBulletTrace* bp )
{
	trace_t trace;
	CG_Trace(&bp->start, (CTrace*)&trace, &bp->end, &nulvec, bp->ignoreEntIndex, 0x2806831|0x803003|0x810011);

	if(trace.fraction == 1.00f)
	{
		return true;
	}

	int iSourceEntityNum = 1023;

	if ( trace.hitType == TRACE_HITTYPE_DYNENT_MODEL || trace.hitType == TRACE_HITTYPE_DYNENT_BRUSH )
	{
		iSourceEntityNum = 1022;
	} else {
		if ( trace.hitType == TRACE_HITTYPE_ENTITY )
			iSourceEntityNum = trace.hitId;
		else
			iSourceEntityNum = 1023;
	}

	return iSourceEntityNum;
}

float GetRemainingPower(CEntity *ent, Vector3<_prec>& vieworg, Vector3<_prec>& origin)
{
	   CBulletTrace bulletTrace;
	   CWeapon      *pWep = GetWeaponByIndex(pLocalEnt->WeaponId);

	   SetupBulletTrace(&bulletTrace, vieworg, origin, cg->clientNum, pWep);

	   bulletTrace.dir = origin;
	   bulletTrace.dir -= vieworg;
	   length = bulletTrace.dir.Length();
	   bulletTrace.dir.Unitize();

	   bulletTrace.end[0] = vieworg[0] + bulletTrace.dir[0] * 8192.0f;
	   bulletTrace.end[1] = vieworg[1] + bulletTrace.dir[1] * 8192.0f;
	   bulletTrace.end[2] = vieworg[2] + bulletTrace.dir[2] * 8192.0f;

	   hitTarget = false;
	   traceEnt  = ent;

	   int oweaponType = pWep->weaponType;
	   //pWep->weaponType = 1;
	   dwRestore = *(DWORD*)0x185C434;
	   *(DWORD*)0x185C434 = NULL;
	   BulletFirePenetrate((CEntity*)0x7973D8, cg->clientNum, &bulletTrace, pWep, &vieworg, 0);
	   *(DWORD*)0x185C434 = dwRestore;
	   //pWep->weaponType = oweaponType;

	   if (hitTarget)
	   {
		   return 0.99f;
	   }

	   if (cvar_AutoWall->iValue == 1)
	   {
		   CClientInfo *hitclient = GetClientByIndex(bulletTrace.ignoreEntIndex);
		   if ((bulletTrace.ignoreEntIndex < MAX_PLAYERS) && ((hitclient->Team == TEAM_FFA) || (hitclient->Team != pLocalClient->Team)))
		   {
			   return(bulletTrace.damageMultiplier);
		   }
		   else
		   {
			   return(0.0f);
		   }
	   }
	   else
	   {
		   if ((bulletTrace.damageMultiplier <= 0.0f) || (bulletTrace.damageMultiplier == 144.0f))
		   {
			   return(0.0f);
		   }
		   return(bulletTrace.damageMultiplier);
	   }
#if 0
	   float outPower;
		CTrace tr;
		CTrace tr2;
		CBulletTrace bt;
		CBulletTrace bt2;

		memset(&tr,0,sizeof(CTrace));
		memset(&tr2,0,sizeof(CTrace));
		memset(&bt,0,sizeof(CBulletTrace));
		memset(&bt2,0,sizeof(CBulletTrace));

		SetupBulletTrace2(&bt, &tr, vieworg, origin, cg->clientNum);
		SetupBulletTrace2(&bt2, &tr2, vieworg, origin, cg->clientNum);

		BYTE result;
		float weaponPower;

		while(1)
		{
			vec3_t trEndBackup;

			if(tr.materialType)
			{
				weaponPower = BG_GetSurfacePenetrationDepth(tr.materialType);
			}
			else
			{
				bt.damageMultiplier = -1.0f;
				break;
			}

			VectorCopy(tr.end, trEndBackup);
			BG_AdvanceTrace(&bt, &tr, 0.135000005364418);

			result = BulletTrace(&tr, &bt, GetEntityByIndex(cg->clientNum), tr.materialType);

			Com_MemCpy((void*)&bt2, (void*)&bt, 64);
			bt2.dir[0] = -bt.dir[0];
			bt2.dir[1] = -bt.dir[1];
			bt2.dir[2] = -bt.dir[2];

			VectorCopy(bt.start, bt2.start);
			VectorMA(trEndBackup, 0.009999999776482582, bt2.dir, bt2.end);

			Com_MemCpy((void*)&tr2, (void*)&tr, 68);

			tr.surfaceDirection[0] = -tr.surfaceDirection[0];
			tr.surfaceDirection[1] = -tr.surfaceDirection[1];
			tr.surfaceDirection[2] = -tr.surfaceDirection[2];

			if(result)
			{
				BG_AdvanceTrace(&bt2, &tr2, 0.009999999776482582);
			}

			BYTE result2 = BulletTrace(&tr2, &bt2, GetEntityByIndex(cg->clientNum), tr2.materialType);
			BYTE result3;

			if(result2 && tr2.allsolid || tr.startsolid && tr2.startsolid)
			{
				result3 = 1;
			}
			else
			{
				result3 = 0;
			}

			if(!result2 && !result3)
			{
				bt.damageMultiplier = -1.0f;
				break;
			}

			float tempPower1, tempPower2;
			float line;
			vec3_t tempVec;

			if(result3)
			{
				VectorSubtract( bt2.start, bt2.end, tempVec );
				line = ( sqrt( tempVec[0] * tempVec[0] + tempVec[1] * tempVec[1] + tempVec[2] * tempVec[2] ) );
			}
			else
			{
				VectorSubtract( tr2.end, trEndBackup, tempVec );
				line = ( sqrt( tempVec[0] * tempVec[0] + tempVec[1] * tempVec[1] + tempVec[2] * tempVec[2] ) );
			}

			if(line < 1.0f)
			{
				line = 1.0f;
			}
		
			if(result2)
			{
				tempPower1 = BG_GetSurfacePenetrationDepth(tr2.materialType);

				tempPower2 = BG_ComparePower(weaponPower, tempPower1);

				/*
				fPower = CG_ComparePenetrationPower(fPower, penetrationPower2),
				fPower <= 0.0
				v23 = bulletTrace->damageMultiplier - wallDepth / fPower,
				bulletTrace->damageMultiplier = v23
				v23 <= 0.0
				*/

				if(tempPower2 <= 0.0f)
				{
					bt.damageMultiplier = -1.0f;
					break;
				}
				else
				{
					bt.damageMultiplier -= line / tempPower2;
					weaponPower = tempPower2;
				}
			}
			else
			{
				bt.damageMultiplier = -1.0f;
				break;
			}
		}
		outPower = bt.damageMultiplier;
		if(outPower > 0.0f)
		{
			return true;
		}
		return false;
#endif
}


void SetupBulletTrace2(CBulletTrace* btr, CTrace* tr, Vector3<_prec>& start, Vector3<_prec>& end, int skipNum)
{
	/*
   btr->weaponEntIndex   = 1022;
   btr->ignoreEntIndex   = skipNum;
   btr->damageMultiplier = 1.0f;
   btr->methodOfDeath    = wep->impactType;
   btr->origStart        = btr->start = start;
   btr->end = end;
   btr->dir = (end - start).Unitize();
	btr->weaponEntIndex = 1022;
	btr->ignoreEntIndex = skipNum;
	btr->damageMultiplier = 1.0f;
	btr->methodOfDeath = pLocalWeapon->impactType;
	//btr->vieworg = Refdef->vOrigin;
	btr->origStart[0] = refdef->vOrigin.X;
	btr->origStart[1] = refdef->vOrigin.Y;
	btr->origStart[2] = refdef->vOrigin.Z;
	btr->start[0] = start.X;
	btr->start[1] = start.Y;
	btr->start[2] = start.Z;
	btr->end[0] = end.X;
	btr->end[1] = end.Y;
	btr->end[2] = end.Z;

	Vector3<_prec> angles = (end - start).Angles();
	angles.AngleVectors(&btr->dir,0,0);

	//CG_Trace(&start,(CTrace*)tr,&end,&_Zero,skipNum,VISIBLE_TRACE);
	BulletTrace(tr,btr,pLocalEnt,0);
	*/
}


bool BG_AdvanceTrace(CBulletTrace* bulletTrace, CTrace* trace, float factor)
{
	DWORD dwBG_AdvanceTrace = 0x00415EE0;
	
	__asm
	{
		push ecx
		push edx

		mov   ecx, bulletTrace
		mov   edx, trace
		lea   eax, factor
		push  eax
		call  dwBG_AdvanceTrace
		add   esp, 4

		pop edx
		pop ecx
	}
#if 0
	int result = 0;
	short hitEntNum = 0;
	int traceContents = trace->contents;

	if (traceContents == 2 || traceContents == 3)
	{
		hitEntNum = 1022;
	}
	else
	{
		if (traceContents == 1)
		{
			hitEntNum = trace->entityNum;
		}
		else
		{
			hitEntNum = 1023;
		}
	}

	bulletTrace->skipNum = hitEntNum;

	if (hitEntNum != 1022 || factor <= 0.0f)
	{
		VectorCopy( trace->end, bulletTrace->start );

		return true;
	}
	else
	{
		float dotProduct = -(trace->surfaceDirection.Dot(trace->surfaceDirection, bulletTrace->dir));

		if (dotProduct >= 0.125f)
		{
			VectorMA(trace->end, factor / dotProduct, bulletTrace->viewDirection, bulletTrace->start);

			return true;
		}
		else
		{
			VectorMA(trace->end, factor * 8.0f, bulletTrace->viewDirection, bulletTrace->start);

			return false;
		}
	}
#endif
}


float BG_GetSurfacePenetrationDepth(int materialType)
{
	int penetrationType = GetWeaponByIndex(pLocalEnt->WeaponId)->penetrateType;

	float result = 0.0f;
	__asm
	{
		mov    edx, penetrationType
		imul   edx, edx, 0x1D
		add    edx, materialType
		fld    dword ptr [edx * 4 + 0x7373B8]
		fstp   result
	}

	if (CheckPerkDeepImpact())
	{
		result *= *(float*)(*(DWORD*)0x736B90 + 12);
	}

	return result;
}


unsigned char BulletTrace(CTrace* trace, CBulletTrace* bulletTrace, CEntity* cent, int materialType)
{
	DWORD dwTraceBullet = 0x004558E0;

	__asm
	{
		mov   eax, trace
		push  materialType
		push  cent
		push  bulletTrace
		call  dwTraceBullet
		add   esp, 0xC
	}
}

bool CheckPerkDeepImpact()
{
	__asm
	{
		mov   edx, pLocalClient 
		mov   eax, dword ptr [edx + 0x158]
		imul  eax, eax, 0x4CC
		mov   ecx, dword ptr [eax + 0x83929C]
		shr   ecx, 5
		xor   eax, eax
		test  cl, 1
		je    NoDeepImpact
		inc   eax
NoDeepImpact:
	}
}
