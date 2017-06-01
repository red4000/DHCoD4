#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hConfig.h"
#include "hBot.h"

#define RCH_SIZE    15

void DrawRadar()
{
   RadarX = refdef->iWidth - cvar_RadarSize->Value - 4.0f;

   // TODO: restructure code, maybe add more options
   float fTenth = cvar_RadarSize->Value * 0.1f;

   DrawWindow("2D Radar", RadarX, RadarY, cvar_RadarSize->Value, cvar_RadarSize->Value);

#if 0
   switch(cvar_RadarBackground->iValue)
   {
   case 1:
      // background
      DrawBox(RadarX, RadarY, cvar_RadarSize->Value, cvar_RadarSize->Value, foreground);

      // grid
      for (float i = 1.f; i < 10.f; i += 1.f)
      {
         DrawBox(RadarX, RadarY + (fTenth * i), cvar_RadarSize->Value, 1.0f, foreground3);
         DrawBox(RadarX + (fTenth * i), RadarY, 1, cvar_RadarSize->Value, foreground3);
      }
      break;

   case 2:
      // outline
      DrawOutline(RadarX, RadarY, cvar_RadarSize->Value, cvar_RadarSize->Value, cvar_GUIOutlineThickness->Value, background);
      break;

   case 3:
      // outlined background
      DrawOutline(RadarX, RadarY, cvar_RadarSize->Value, cvar_RadarSize->Value, cvar_GUIOutlineThickness->Value, background);
      DrawBox(RadarX, RadarY, cvar_RadarSize->Value, cvar_RadarSize->Value, foreground);

      // grid
      for (float i = 1.f; i < 10.f; i += 1.f)
      {
         DrawBox(RadarX, RadarY + (fTenth * i), cvar_RadarSize->Value, 1.0f, foreground3);
         DrawBox(RadarX + (fTenth * i), RadarY, 1, cvar_RadarSize->Value, foreground3);
      }
      break;
   }
#endif

   // lines spanning the whole radar
   DrawBox(RadarX + cvar_RadarSize->Value * 0.5f, RadarY, 1.0f, cvar_RadarSize->Value, background);
   DrawBox(RadarX, RadarY + cvar_RadarSize->Value / 2, cvar_RadarSize->Value, 1.0f, background);

   // small crosshair in the middle of the radar
   DrawBox(RadarX + (cvar_RadarSize->Value * 0.5f) - (RCH_SIZE * 0.5f), RadarY + (cvar_RadarSize->Value * 0.5f), RCH_SIZE, 1.0f, foreground2);
   DrawBox(RadarX + (cvar_RadarSize->Value * 0.5f), RadarY + (cvar_RadarSize->Value * 0.5f) - (RCH_SIZE * 0.5f), 1, RCH_SIZE / 2, foreground2);

   RadarX += 2.0f;
}


void CalcRadarPoint(Vector3<_prec> *point, float *outX, float *outY)
{
   Vector3<_prec> Dist, Forward, Right;

   Dist      = (*point - refdef->vOrigin) * cvar_RadarScale->Value;
   Forward.X = 0;
   Forward.Y = refdef->ViewAngles.Y - 90;
   Forward.Z = 0;
   Forward.Normalize180();
   Forward.AngleVectors(&Forward, &Right, 0);
   Dist.Z = 0;
   float SizeX  = cvar_RadarSize->Value;
   float SizeY  = SizeX;
   float PointX = (RadarX + SizeX / 2) + Forward.Dot(Dist, Forward) / SizeX;
   float PointY = (RadarY + SizeY / 2) + Right.Dot(Dist, Right) / SizeY;

   if (PointY < RadarY)
   {
      PointY = RadarY;
   }

   if (PointY > RadarY + SizeY)
   {
      PointY = RadarY + SizeY;
   }

   if (PointX < RadarX)
   {
      PointX = RadarX;
   }

   if (PointX > RadarX + SizeX)
   {
      PointX = RadarX + SizeX;
   }

   *outX = PointX;
   *outY = PointY;
}


void RadarPlayer(CEntity *pEnt, CColor<_color_prec> Color)
{
   float PointX, PointY;

   CalcRadarPoint(&pEnt->vOrigin, &PointX, &PointY);

   DrawRotatedPic(PointX - 10, PointY - 10, 20, 20, (360 - pEnt->vAngles.Y) + refdef->ViewAngles.Y, (float *)&Color, sTriangle);
}


void RadarExplosive(CEntity *pEnt, CColor<_color_prec> Color)
{
   float PointX, PointY;

   CalcRadarPoint(&pEnt->vOrigin, &PointX, &PointY);

   DrawRotatedPic(PointX - 10, PointY - 10, 20, 20, (360 - pEnt->vAngles.Y) + refdef->ViewAngles.Y, (float *)&Color, sExplosion);
}


void RadarWeapon(CEntity *pEnt, CColor<_color_prec> Color)
{
   float PointX, PointY;

   CalcRadarPoint(&pEnt->vOrigin, &PointX, &PointY);

   DrawRotatedPic(PointX - 1, PointY - 5, 1, 10, (360 - pEnt->vAngles.Y) + refdef->ViewAngles.Y, (float *)&Color, sWhite);
}


void RadarHelicopter(CEntity *pEnt, CColor<_color_prec> Color)
{
   float PointX, PointY;

   CalcRadarPoint(&pEnt->vOrigin, &PointX, &PointY);

   DrawRotatedPic(PointX - 25, PointY - 35, 50, 70, (360 - pEnt->vAngles.Y) + refdef->ViewAngles.Y, (float *)&Color, sHelicopter);
}


void RadarAirstrike(CEntity *pEnt, CColor<_color_prec> Color)
{
   float PointX, PointY;

   CalcRadarPoint(&pEnt->vOrigin, &PointX, &PointY);

   DrawRotatedPic(PointX - 15, PointY - 15, 30, 30, (360 - pEnt->vAngles.Y) + refdef->ViewAngles.Y, (float *)&Color, sAirstrike);
}


void AddToCircle(CEntity *pEnt, CColor<_color_prec> Color)
{
   Vector3<_prec> Forward, Right;
   Vector3<_prec> Dis = (pEnt->vOrigin - refdef->vOrigin).Unitize();
   Dis.Z     = 0.0f;
   Forward.X = 0.0f;
   Forward.Y = refdef->ViewAngles.Y - 90.0f;
   Forward.Z = 0.0f;
   Forward.Normalize360();
   Forward.AngleVectors(&Forward, &Right, NULL);
   float X = Forward.Dot(Forward, Dis);
   float Y = Forward.Dot(Right, Dis);

   float fX = X * cvar_CircleSize->Value;
   float fY = Y * cvar_CircleSize->Value;

   if (abs(fX) > cvar_CircleSize->Value)
   {
      fX = Forward.Dot(Forward, cvar_CircleSize->Value);
   }

   if (abs(fY) > cvar_CircleSize->Value)
   {
      fY = Forward.Dot(Right, cvar_CircleSize->Value);
   }

   X = fX + (refdef->iWidth / 2);
   Y = fY + (refdef->iHeight / 2);
   DrawRotatedPic(X - 9, Y - 11, 18, 22, (360 - pEnt->vAngles.Y) + refdef->ViewAngles.Y, (float *)&Color, sIsocolesTriangle);
}
