#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hConfig.h"
#include "hBot.h"
#include "hAim.h"

typedef struct
{
   Vector3<_prec> pos;
   DWORD          numThreats;
} tactical_node_t;

#define NODE_STEP    12.0f
#define MAX_NODES    200 // only 150 nodes should be used.

DWORD nodeIndex = 0;

tactical_node_t nodes[MAX_NODES];

DWORD CalculateHostilesVisibleFromPoint(Vector3<_prec> hpt)
{
   UNREFERENCED_PARAMETER(hpt);
   return(0);
}


void ExpandKnowledgeFromPoint(Vector3<_prec> pt)
{
   UNREFERENCED_PARAMETER(pt);
   for (float fa = 0; fa < 360; fa += NODE_STEP)
   {
   }
}


void CalculateTacticalKnowledge()
{
}


void DrawTacticalAdvisories()
{
   nodeIndex = 0;
   CalculateTacticalKnowledge();

   if (cvar_DefensiveTacticalAdvisories->Value)
   {
   }

   if (cvar_OffensiveTacticalAdvisories->Value)
   {
   }
}
