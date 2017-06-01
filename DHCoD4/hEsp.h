#ifndef HESP_H
#define HESP_H

#include "hBot.h"

extern void ESP(CEntity *pEnt, CClientInfo *pClient);
extern void EESP(CEntity *pEnt);

extern float ESPOffset;
extern DWORD EspFont;
CInline void SetESPOffset()
{
   int NumLines = 0;

   if (cvar_PlayerName->iValue == 1)
   {
      NumLines++;
   }

   if (cvar_PlayerDistance->iValue == 1)
   {
      NumLines++;
   }

   if (cvar_PlayerWeapon->iValue == 1)
   {
      NumLines++;
   }

   if (NumLines == 1)
   {
      ESPOffset = 6.0f;
   }

   if (NumLines == 2)
   {
      ESPOffset = 0.0f;
   }

   if (NumLines == 3)
   {
      ESPOffset = -7.0f;
   }

   int iFont = cvar_EspFont->iValue;
   if (iFont < 0)
   {
      iFont = 0;
   }
   else if (iFont > 3)
   {
      iFont = 3;
   }

   EspFont = EFonts[iFont];
}


void DrawWarnings();

#define SmallDevFont    EFonts[0]
#define ConsoleFont     EFonts[1]
#define NormalFont      EFonts[2]
extern DWORD               sWhite;
extern DWORD               sTriangle;
extern DWORD               sExplosion;
extern DWORD               sHelicopter;
extern DWORD               sAirstrike;
extern DWORD               sGrenade;
extern DWORD               sIsocolesTriangle;
extern float               RadarX;
extern float               RadarY;
extern float               RadarSize;
extern CColor<_color_prec> FriendlyVis;
extern CColor<_color_prec> EnemyVis;
extern CColor<_color_prec> EnemyHid;
extern CColor<_color_prec> FriendlyHid;
extern CColor<_color_prec> Shadow;
extern WORD                Bone_Middle[5];
extern WORD                Bone_Right_Arm[5];
extern WORD                Bone_Left_Arm[5];
extern WORD                Bone_Right_Leg[5];
extern WORD                Bone_Left_Leg[5];

#endif
