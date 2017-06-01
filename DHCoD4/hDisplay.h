#ifndef HDISPLAY_H
#define HDISPLAY_H

CInline float min2ang(float min)
{
   return(LerpFloat(0.0f, 360.0f, min / 60.0f));
}


CInline float sec2ang(float sec)
{
   return(LerpFloat(0.0f, 360.0f, sec / 60.0f));
}


CInline float hour2ang(float hour)
{
   if (hour > 12.0f)
   {
      hour -= 12.0f;
   }

   return(LerpFloat(0.0f, 360.0f, hour / 12.0f));
}


CInline void DrawClockTick(float clockCenterX, float clockCenterY, float distance, float angle, float length, float thickness, CColor<_color_prec>& col)
{
   Vector3<_prec> _end;
   Vector3<_prec> _start(0.0f, angle, 0.0f);

   _start.AngleVectors(&_start, 0, 0);
   _end    = _start;
   _start *= distance;
   _end   *= distance + length;

   _start.X += clockCenterX;
   _start.Y += clockCenterY;

   _end.X += clockCenterX;
   _end.Y += clockCenterY;

   Draw2DLine(_start.X, _start.Y, _end.X, _end.Y, thickness, col);
}


CInline void DrawClock()
{
   SYSTEMTIME st;
   float      clockX, clockY, clockSize, clockSize8, clockSize2, clockCenterX, clockCenterY;

   clockSize = cvar_ClockSize->Value;
   clockX    = (refdef->iWidth / 2) - clockSize / 2;
   if (cvar_InfoBar->iValue == 1)
   {
      clockY = 40.0f;
   }
   else
   {
      clockY = 3.0f;
   }
   clockCenterX = clockX + (clockSize / 2.0f);
   clockCenterY = clockY + (clockSize / 2.0f);
   DrawOutline(clockX, clockY, clockSize, clockSize, cvar_GUIOutlineThickness->Value, background);
   DrawBox(clockX, clockY, clockSize, clockSize, foreground);
   //DrawRUFlag(clockX, clockY, clockSize, clockSize);

   clockSize /= 2.0f;
   GetLocalTime(&st);

   clockSize8 = (clockSize * .8f);
   clockSize2 = (clockSize * .2f) - 5.f;

   for (float i = 0.0f; i < 360.0f; i += 6.0f)
   {
      if (((int)i % 30) == 0)
      {
         DrawClockTick(clockCenterX, clockCenterY, (clockSize8), i, (clockSize2), 1.0f, CColor<_color_prec>(150, 150, 150, 255));
      }

      else
      {
         DrawClockTick(clockCenterX, clockCenterY, (clockSize8), i, (clockSize2), 1.0f, CColor<_color_prec>(150, 150, 150, 100));
      }
   }

   DrawClockTick(clockCenterX, clockCenterY, 0.0f, sec2ang((float)st.wSecond + ((float)(st.wMilliseconds % 1000) / 1000.0f)) - 90, clockSize8, 1.0f, CColor<_color_prec>(0, 0, 255, 255));
   DrawClockTick(clockCenterX, clockCenterY, 0.0f, min2ang((float)st.wMinute) + sec2ang((float)st.wSecond / 60.0f) - 90, clockSize * 0.6f, 1.0f, CColor<_color_prec>(0, 255, 0, 255));
   DrawClockTick(clockCenterX, clockCenterY, 0.0f, hour2ang((float)st.wHour) + min2ang((float)st.wMinute / 60.0f) - 90, clockSize * 0.4f, 2.0f, CColor<_color_prec>(255, 255, 255, 255));
}


CInline void DrawDigitalClock()
{
   SYSTEMTIME st;
   float      clockX, clockY, clockSize;

   clockSize = 50.0f;
   clockX    = (refdef->iWidth / 2) - clockSize / 2.0f;
   if (cvar_InfoBar->iValue == 1)
   {
      clockY = 40.0f;
   }
   else
   {
      clockY = 3.0f;
   }
   DrawOutline(clockX, clockY, clockSize, 13.0f, cvar_GUIOutlineThickness->Value, background);
   DrawBox(clockX, clockY, clockSize, 13.0f, foreground);

   GetLocalTime(&st);

   DrawGlowTextf(NormalFont, clockX, clockY + 14.0f, 1.0f, 1.0f, 0.0f, CColor<_color_prec>(255, 255, 255, 255), GLOW_MORE_SHADOWED, foreground, "%02i:%02i:%02i", st.wHour, st.wMinute, st.wSecond);
}


CInline void DrawHealthbar()
{
   float hbX, hbY, hbW, hbH, fMaxHealth=100;

   if (cg_player->health == 30)
   {
      fMaxHealth = 30;
   }

   float healthPercent = (float)cg_player->health / fMaxHealth;
   hbW = (float)refdef->iWidth * (1.f / 3.f);
   hbH = 15;
   hbX = (refdef->iWidth / 2) - (hbW / 2);
   hbY = refdef->iHeight - hbH * 3;

   DrawOutline(hbX, hbY, hbW, hbH, cvar_GUIOutlineThickness->Value, background);
   DrawBox(hbX, hbY, hbW, hbH, CColor<_color_prec>(healthPercent, 0.0f, 0.0f, 0.65f));
   DrawStringf(hbX + (hbW / 2) - 10, hbY + 15, SmallDevFont, CColor<_color_prec>(255, 255, 255, 255), 1.0f, DS("%i%%"), (int)(healthPercent * 100.0f));
}


#endif
