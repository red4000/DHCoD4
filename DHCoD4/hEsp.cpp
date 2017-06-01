// TODO: customizable esp format( 1 = always on side, 2 = on top, bottom, then side of box
// when box isn't drawn, esp should be centered..

// TODO: fix esp shadow crash
// TODO: fix weapon esp bug and maybe other esps, weapon esp just draws distance esp again
// TODO: fix aimbot...

#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hRadar.h"
#include "hConfig.h"
#include "hBot.h"
#include "hAim.h"
#include "h47.h"

#include "VMProtectSDK.h"

float ESPOffset = 0.0f, RadarX, RadarY = 60.0f;
DWORD sWhite, sTriangle, sGrenade, sIsocolesTriangle, sExplosion, sHelicopter, sAirstrike;

CColor<_color_prec>
FriendlyVis(0.0f, 1.0f, 0.0f, 1.0f),
EnemyVis(1.0f, 0.0f, 0.0f, 1.0f),
EnemyHid(0.8f, 0.0f, 0.0f, 0.7f),
FriendlyHid(0.0f, 1.0f, 0.0f, 0.7f),
Shadow(0.0f, 0.0f, 0.0f, 0.8f),

v4Red(1.0f, 0.0f, 0.54f, 1.0f),
v4Gold(1.0f, 1.0f, 0.54f, 1.0f),
v4Gray(0.6f, 0.6f, 0.6f, 1.0f),
v4GrayHid(0.6f, 0.6f, 0.6f, 1.0f);

Vector3<_prec>
BoxSizeProne(33.0f, 30.0f, 21.0f),
OffsetProne(-55.0f, -20.0f, 0.0f),

BoxSizeCrouch(21.0, 24.0f, 45.0f),
OffsetCrouch(-18.0f, -13.0f, 0.0f),

BoxSizeStand(16.0f, 16.0f, 64.0f),
OffsetStand(-16.0f, -16.0f, 0.0f);

WORD Bone_Middle[5], Bone_Right_Arm[5], Bone_Left_Arm[5], Bone_Right_Leg[5], Bone_Left_Leg[5];

Vector3<_prec> nullvec(0.0f, 0.0f, 0.0f);
DWORD          numVisPlayers = 0, EspFont = 0;
float          fThreat[MAX_PLAYERS];
char           *wNames[MAX_PLAYERS];

CInline bool UpdateBox(CEntity *pEnt, CBox *pBox)
{
   Vector3<_prec> vTop = pEnt->vOrigin;
   Vector3<_prec> TopLeft;
   Vector3<_prec> BottomRight;

   if (pEnt->State & FLAGS_PRONE)
   {
      vTop.Z += 24.0f;
   }

   else if (pEnt->State & FLAGS_CROUCH)
   {
      vTop.Z += 40.0f;
   }

   else
   {
      vTop.Z += 64.0f;
   }

   (pEnt->vOrigin - refdef->vOrigin).Angles().AngleVectors(NULL, &TopLeft, NULL);

   WorldToScreen(pEnt->vOrigin + (TopLeft * (-1.0f * BOX_RADIUS)), &BottomRight);
   WorldToScreen(vTop + (TopLeft * BOX_RADIUS), &TopLeft);

#if 0
   if ((BottomRight.X > refdef->iWidth) || (TopLeft.Y > refdef->iHeight))
   {
      return(false);
   }
#endif

   if (!(TopLeft.Z < 0.01f) && !(BottomRight.Z < 0.01f))
   {
      float save;

      if (TopLeft.X > BottomRight.X)
      {
         save          = TopLeft.X;
         TopLeft.X     = BottomRight.X;
         BottomRight.X = save;
      }

      if (TopLeft.Y > BottomRight.Y)
      {
         save          = TopLeft.Y;
         TopLeft.Y     = BottomRight.Y;
         BottomRight.Y = save;
      }

      pBox->Left   = TopLeft.X;
      pBox->Right  = BottomRight.X;
      pBox->Top    = TopLeft.Y;
      pBox->Bottom = BottomRight.Y;

      return(true);
   }
   return(false);
}


CInline void DrawESPBox(CBox *pBox, CColor<_color_prec> Color)
{
   DrawOutline(pBox->Left, pBox->Top, pBox->Right - pBox->Left, pBox->Bottom - pBox->Top, cvar_BoxEspThickness->Value, Color);
   DrawOutline(pBox->Left - 1, pBox->Top - 1, (pBox->Right - pBox->Left) + 2, (pBox->Bottom - pBox->Top) + 2, cvar_BoxEspThickness->Value, Shadow);
   DrawOutline(pBox->Left + 1, pBox->Top + 1, (pBox->Right - pBox->Left) - 2, (pBox->Bottom - pBox->Top) - 2, cvar_BoxEspThickness->Value, Shadow);
}


void Draw3DPlayerBox(CEntity *pEnt, CColor<_color_prec> Color)
{
   Vector3<_prec> _min, _max;

   if (pEnt->State & FLAGS_PRONE)
   {
      _min = OffsetProne;
      _max = BoxSizeProne;
   }

   else if (pEnt->State & FLAGS_CROUCH)
   {
      _min = OffsetCrouch;
      _max = BoxSizeCrouch;
   }

   else
   {
      _min = OffsetStand;
      _max = BoxSizeStand;
   }

   _min += pEnt->vOrigin;
   _max += pEnt->vOrigin;

   DrawBox3D(_min, _max, Color);
}


CInline Vector3<_prec> GetBoneOrigin(CEntity *Ent, WORD BoneName)
{
   return(GetPlayerTag(BoneName, Ent));
}


CInline void DrawBone(CEntity *Ent, WORD BoneName1, WORD BoneName2, CColor<_color_prec> Color)
{
   Vector3<_prec> vBonePos1, vBonePos2;
   WorldToScreen(GetBoneOrigin(Ent, BoneName1), &vBonePos1);

   if (!(vBonePos1.Z < 0.01f))
   {
      WorldToScreen(GetBoneOrigin(Ent, BoneName2), &vBonePos2);

      if (!(vBonePos2.Z < 0.01f))
      {
         Draw2DLine(vBonePos1.X, vBonePos1.Y, vBonePos2.X, vBonePos2.Y, 1.0f, Color);
      }
   }
}


CInline void DrawBone3(Vector3<H47_PREC> *from, Vector3<H47_PREC> *to, CColor<_color_prec>& Col)
{
   Vector3<_prec> vBonePos1, vBonePos2;
   Vector3<float> ffrom((float)from->X, (float)from->Y, (float)from->Z);
   Vector3<float> fto((float)to->X, (float)to->Y, (float)to->Z);

   WorldToScreen(ffrom, &vBonePos1);

   if (!(vBonePos1.Z < 0.01f))
   {
      WorldToScreen(fto, &vBonePos2);

      if (!(vBonePos2.Z < 0.01f))
      {
         Draw2DLine(vBonePos1.X, vBonePos1.Y, vBonePos2.X, vBonePos2.Y, 2.0f, Col);
      }
   }
}


CInline void DrawBoneEsp(CEntity *Ent, CColor<_color_prec> Col) // TODO: rename to DrawBoneESP
{
   for (int i = 0; i < 4; i++)
   {
      DrawBone(Ent, Bone_Middle[i], Bone_Middle[i + 1], Col);
      DrawBone(Ent, Bone_Right_Arm[i], Bone_Right_Arm[i + 1], Col);
      DrawBone(Ent, Bone_Left_Arm[i], Bone_Left_Arm[i + 1], Col);
      DrawBone(Ent, Bone_Right_Leg[i], Bone_Right_Leg[i + 1], Col);
      DrawBone(Ent, Bone_Left_Leg[i], Bone_Left_Leg[i + 1], Col);
   }
}


CInline void DrawBone2(CEntity *Ent, WORD BoneName1, WORD BoneName2, CColor<_color_prec> Color)
{
   Vector3<_prec> vBonePos1, vBonePos2;
   vBonePos1 = WorldToScreen(GetBoneOrigin(Ent, BoneName1));
   vBonePos2 = WorldToScreen(GetBoneOrigin(Ent, BoneName2));
   DrawBox(vBonePos1.X, vBonePos1.Y, 3, 3, Color);
   DrawBox(vBonePos2.X, vBonePos2.Y, 3, 3, Color);
}


void DrawBoneEsp2(CEntity *Ent, CColor<_color_prec> Col) // TODO: rename to DrawBoneESP2
{
#ifdef H47
   for (unsigned int i = 0; i < 10; ++i)
   {
      DrawBone3(&vic[Ent->ClientNumber].bone[i].nw.pos, &(vic[Ent->ClientNumber].bone[i].pred.pos + (vic[Ent->ClientNumber].bone[i].pred.vel * 5.0f)), Col);
   }
#else
   for (int i = 0; i < 4; i++)
   {
      DrawBone2(Ent, Bone_Middle[i], Bone_Middle[i + 1], Col);
      DrawBone2(Ent, Bone_Right_Arm[i], Bone_Right_Arm[i + 1], Col);
      DrawBone2(Ent, Bone_Left_Arm[i], Bone_Left_Arm[i + 1], Col);
      DrawBone2(Ent, Bone_Right_Leg[i], Bone_Right_Leg[i + 1], Col);
      DrawBone2(Ent, Bone_Left_Leg[i], Bone_Left_Leg[i + 1], Col);
   }
#endif // H47
}


void DrawBonerEsp(CEntity *Ent, CColor<_color_prec> Col) // TODO: rename to DrawBonerESP
{
   Vector3<_prec> fwd, Pelvis, Penis;
   CClientInfo    *Cli = GetClientByIndex(Ent->ClientNumber);

   Ent->vAngles.AngleVectors(&fwd, 0, 0);

   if (strstr(Cli->Name, DS("Houstin")))
   {
      fwd    = fwd * 20.0f;
      fwd.Z += 20.0f;
   }

   else
   {
      fwd    = fwd * 5.0f;
      fwd.Z += 5;
   }

   Pelvis = GetPlayerTag(Bone_Middle[4], Ent);
   WorldToScreen(Pelvis + fwd, &Penis);
   WorldToScreen(Pelvis, &Pelvis);

   if (!(Pelvis.Z < 0.01f) && !(Penis.Z < 0.01f))
   {
      Draw2DLine(Pelvis.X, Pelvis.Y, Penis.X, Penis.Y, 3.0f, Col);
   }
}


void DrawBarrelESP(CEntity *Ent, CColor<_color_prec> col)
{
   Vector3<_prec> fwd, ttAngles, bStart;
   CTrace         tc;

   ttAngles = Ent->vNewAngles;
   ttAngles.Normalize360();
   ttAngles.AngleVectors(&fwd, 0, 0);
   fwd      *= 1500.0f;
   bStart    = vHead[Ent->ClientNumber];
   fwd      += bStart;
   bStart.Z += 10.0f;
   CG_Trace(&bStart, &tc, &fwd, &_Zero, Ent->ClientNumber, VISIBLE_TRACE);
   WorldToScreen(bStart + ((fwd - bStart) * tc.Fraction), &fwd);
   WorldToScreen(bStart, &bStart);

   if (!(bStart.Z < 0.01f) && !(fwd.Z < 0.01f))
   {
      Draw2DLine(bStart.X, bStart.Y, fwd.X, fwd.Y, 1.0f, col);
   }
}


void DrawLineESP(CEntity *Ent, CColor<_color_prec> col)
{
   Vector3<_prec> fwd;
   WorldToScreen(vHead[Ent->ClientNumber], &fwd);

   if (!(fwd.Z < 0.01f))
   {
      Draw2DLine(((float)refdef->iWidth) * 0.5f, (float)(refdef->iHeight - 1), fwd.X, fwd.Y, 1.0f, col);
   }
}


void DrawPyramidESP(CBox *pBox, CColor<_color_prec> col)
{
   float topX = pBox->Left + ((pBox->Right - pBox->Left) / 2.0f);

   Draw2DLine(topX, pBox->Top, pBox->Left, pBox->Bottom, 1.0f, col);
   Draw2DLine(topX, pBox->Top, pBox->Right, pBox->Bottom, 1.0f, col);
   Draw2DLine(pBox->Left, pBox->Bottom, pBox->Right, pBox->Bottom, 1.0f, col);
}


void EESP(CEntity *pEnt)
{
   if (pEnt->IsValidEnt && pEnt->Alive)
   {
      Vector3<_prec> vScreenPos;
      char           szLocalWeapon[256]; // TODO: rename
      bool           bVisible = VisTrace(refdef->vOrigin, pEnt->vOrigin, pEnt->ClientNumber);

      if (pEnt->WeaponId)
      {
         CWeapon *pWeapon = GetWeaponByIndex(pEnt->WeaponId);

         if (pWeapon)
         {
			strcpy_s(szLocalWeapon, pWeapon->displayName);
         }

         else
         {
            goto label_1;
         }
      }

      else
      {
label_1:
         strcpy_s(szLocalWeapon, DS("None"));
      }

      WorldToScreen(pEnt->vOrigin, &vScreenPos);

      CColor<_color_prec>& col = (bVisible) ? v4Red : v4Gold;
      bool                 bInvalidExplosive = false;

      switch (pEnt->EntityType)
      {
      case TYPE_EXPLOSIVE:
         if (!strcmp(szLocalWeapon, DS("WEAPON_M2FRAGGRENADE")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 14, vScreenPos.Y - 14, 35, 35, col, R_RegisterShader(DS("hud_us_grenade")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[Frag]"));
            }
         }

         else if (!strcmp(szLocalWeapon, DS("WEAPON_CLAYMORE")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 14, vScreenPos.Y - 14, 35, 35, col, R_RegisterShader(DS("hud_icon_claymore")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[Claymore]"));
            }
         }

         else if (!strcmp(szLocalWeapon, DS("WEAPON_FLASH_GRENADE")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 14, vScreenPos.Y - 14, 35, 35, col, R_RegisterShader(DS("hud_us_flashgrenade")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[Flash]"));
            }
         }

         else if (!strcmp(szLocalWeapon, DS("WEAPON_CONCUSSION_GRENADE")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 14, vScreenPos.Y - 14, 35, 35, col, R_RegisterShader(DS("hud_us_stungrenade")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[Stun]"));
            }
         }

         else if (!strcmp(szLocalWeapon, DS("WEAPON_C4")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 14, vScreenPos.Y - 14, 35, 35, col, R_RegisterShader(DS("hud_icon_c4")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[C4]"));
            }
         }

         else if (!strcmp(szLocalWeapon, DS("WEAPON_SMOKE_GRENADE")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 14, vScreenPos.Y - 14, 35, 35, col, R_RegisterShader(DS("hud_us_smokegrenade")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[Smoke]"));
            }
         }

         else if (!strcmp(szLocalWeapon, DS("WEAPON_M203")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 20, vScreenPos.Y - 14, 40, 35, col, R_RegisterShader(DS("hud_icon_40mm_grenade")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[40mm]"));
            }
         }

         else if (!strcmp(szLocalWeapon, DS("WEAPON_RPG")))
         {
            if (cvar_Explosives->iValue == 1)
            {
               DrawBox(vScreenPos.X - 22, vScreenPos.Y - 14, 42, 35, col, R_RegisterShader(DS("hud_icon_rpg")));                             // TODO: cache shader
            }

            else if (cvar_Explosives->iValue == 2)
            {
               DrawEspText(vScreenPos.X, vScreenPos.Y, EspFont, col, cvar_EspFontScale->Value, DS("[RPG]"));
            }
         }
         else
         {
            bInvalidExplosive = true;
         }

         if ((bInvalidExplosive == false) && (cvar_Radar->iValue == 1) && (cvar_RadarExplosives->iValue == 1))
         {
            RadarExplosive(pEnt, col);
         }
         break;

      case TYPE_WEAPON:
         if (strlen(szLocalWeapon))
         {
            if ((cvar_Radar->iValue == 1) && (cvar_RadarWeapons->iValue == 1))
            {
               RadarWeapon(pEnt, col);
            }
            if (cvar_WeaponEsp->iValue == 1)
            {
#if 0
               DrawEspText(vScreenPos.X, vScreenPos.Y + 7, SmallDevFont, col, 1.0f, szLocalWeapon);
#endif
               DrawEspText(vScreenPos.X, vScreenPos.Y + 7, SmallDevFont, col, 1.0f, "Weapon");
            }
         }
         break;

      case TYPE_HELICOPTER:
         if ((cvar_Radar->iValue == 1) && (cvar_RadarHelicopters->iValue == 1))
         {
            RadarHelicopter(pEnt, col);
         }
         break;

      case TYPE_AIRSTRIKE:
         if ((cvar_Radar->iValue == 1) && (cvar_RadarAirstrikes->iValue == 1))
         {
            RadarAirstrike(pEnt, col);
         }
         break;
      }
   }
}


float WarningGetThreat(CEntity *ce, Vector3<_prec> *tHead)
{
   Vector3<_prec> wVec, wVec2;

   wVec2 = ce->vNewAngles;
   wVec2.Normalize360();

   wVec = (GetLocalViewpoint() - *tHead).Angles() - wVec2;
   wVec.Normalize180();

   float fTmpthreat = ((180.0f - wVec.Length()) / 180.0f) * 100.0f;
   //fTmpthreat += 5.0f;

   if (fTmpthreat < 0.0f)
   {
      fTmpthreat = -fTmpthreat;
   }

   if (fTmpthreat > 100.0f)
   {
      fTmpthreat = 100.0f;
   }

   return(fTmpthreat);
}


void _Dodge(float threat)
{
   if ((pLocalEnt->State & FLAGS_PRONE))
   {
      return;
   }

   if (threat > 95.0f)
   {
      switch (cvar_Dodge->iValue)
      {
      case 1:
         CG_ConsoleCommand("gocrouch");
         break;

      case 2:
         CG_ConsoleCommand("goprone");
         break;
      }
   }
}


void ESP(CEntity *pEnt, CClientInfo *pClient)
{
   CBox Box;

   Vector3<_prec>      wVec;
   bool                bVis  = VisTrace(refdef->vOrigin, pEnt->vOrigin, pEnt->ClientNumber);
   CColor<_color_prec> Color = (pClient->Team == pLocalClient->Team ? (bVis ? FriendlyVis : FriendlyHid) : (bVis ? EnemyVis : EnemyHid));

   if (Color == EnemyVis)
   {
      if (cvar_SnapLines->iValue == 1)
      {
         WorldToScreen(vHead[pEnt->ClientNumber], &wVec);

         if ((wVec.Z < 0.01f))               // they're behind us
         {
            if (wVec.X < refdef->iWidth / 2)
            {
               wVec.X -= refdef->iWidth;
            }

            else
            {
               wVec.X += refdef->iWidth;
            }
         }

         Draw2DLine(((float)refdef->iWidth) * 0.5f, ((float)refdef->iHeight) * 0.5f, wVec.X, wVec.Y, 1, EnemyVis);
      }

      wNames[numVisPlayers]  = pClient->Name;
      fThreat[numVisPlayers] = WarningGetThreat(pEnt, &vHead[pEnt->ClientNumber]);
      _Dodge(fThreat[numVisPlayers++]);
   }

   if ((cvar_Radar->iValue == 1) && (cvar_RadarPlayers->iValue == 1))
   {
      RadarPlayer(pEnt, Color);
   }

   if (cvar_Circle->iValue == 1)
   {
      AddToCircle(pEnt, Color);
   }

   if (!UpdateBox(pEnt, &Box))
   {
      return;
   }

   if (cvar_BarrelESP->iValue == 1)
   {
      DrawBarrelESP(pEnt, Color);
   }

   if (cvar_LineESP->iValue == 1)
   {
      DrawLineESP(pEnt, Color);
   }

   float centerY = (abs(Box.Bottom - Box.Top) / 2) + Box.Top + ESPOffset;

   switch (cvar_2DBoxes->iValue)
   {
   case 1:      // 2d boxes
      DrawESPBox(&Box, Color);
      break;

   case 2:      // 3d boxes
      Draw3DPlayerBox(pEnt, Color);
      break;
   }

   if (cvar_PyramidEsp->iValue)
   {
      DrawPyramidESP(&Box, Color);
   }

   if (cvar_BoneESP->iValue == 1)
   {
      DrawBoneEsp(pEnt, Color);
   }

   else if (cvar_BoneESP->iValue == 2)
   {
      DrawBoneEsp2(pEnt, Color);
   }

   if (cvar_BonerESP->iValue == 1)
   {
      DrawBonerEsp(pEnt, Color);
   }

   switch(cvar_PlayerName->iValue)
   {
   case 1:
      DrawEspText(Box.Right + 2.0f, centerY, EspFont, Color, cvar_EspFontScale->Value, pClient->Name);
      centerY += TEXT_HEIGHT;
	   break;

   case 2:
      DrawNameTag(0, pEnt, Color.a);
	   break;
   }

   if (cvar_PlayerDistance->iValue == 1)
   {
      DrawEspTextf(Box.Right + 2.0f, centerY, EspFont, Color, cvar_EspFontScale->Value, DS("%i"), (int)fDistance[pEnt->ClientNumber]);
      centerY += TEXT_HEIGHT;
   }

   if (pClient->WeaponNumber)
   {
      CWeapon *wep = GetWeaponByIndex(pClient->WeaponNumber);
      char    wpnName[256];

      if (wep)
      {
         switch (cvar_PlayerWeapon->iValue)
         {
         case 1:
            if (wep->clipName)
            {
               if (strstr(wep->clipName, "silencer"))
               {
                  DrawEspTextf(Box.Right + 2.0f, centerY, EspFont, Color, cvar_EspFontScale->Value, DS("Silent %s"), GetWeaponNameByIndex(wpnName, pClient->WeaponNumber));
               }

               else
               {
                  DrawEspText(Box.Right + 2.0f, centerY, EspFont, Color, cvar_EspFontScale->Value, GetWeaponNameByIndex(wpnName, pClient->WeaponNumber));
               }
            }

            else
            {
               DrawEspText(Box.Right + 2.0f, centerY, EspFont, Color, cvar_EspFontScale->Value, GetWeaponNameByIndex(wpnName, pClient->WeaponNumber));
            }

            centerY += TEXT_HEIGHT;
            break;

         case 2:
            DrawBox(Box.Right + 2.0f, centerY, 70, 45, Color, wep->hudIcon);
            break;
         }
      }
   }
}


void DrawWarnings()
{
#if 0
   float wBY = 7.0f;

   if (cvar_InfoBar->iValue == 1)
   {
      wBY = 40.0f;
   }

   switch (cvar_Clock->iValue)
   {
   case 1:
      wBY += cvar_ClockSize->Value + 2.0f;
      break;

   case 2:
      wBY += 15.0f;
      break;
   }

   for (DWORD i = 0; i < numVisPlayers; ++i)
   {
      float dScale = 1.0f + (fThreat[i] / 100.0f);
      wBY += 13.0f * dScale;
	  DrawCenteredString(((float)refdef->iWidth) * 0.5f, wBY, SmallDevFont, CColor<_color_prec>(255, 255, 255, 255), dScale, DS("%s is aiming at you %s%i%%"), wNames[i], fThreat[i] > 70.0f ? "^1":"\0", (int)fThreat[i]); // crashing
   }
#endif
}
