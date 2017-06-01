#ifndef HCROSSHAIR_H
#define HCROSSHAIR_H

CInline void DrawCross(float x, float y, float w, float h, float s, CColor<_color_prec>& color) //xpos,ypos,width,height,spread
{
   DrawBox(x - s, y - (h / 2.0f), w, h, color);                                                 //left
   DrawBox(x - (h / 2.0f), y - s, h, w, color);                                                 //top
   DrawBox(x + s - w - 1, y - (h / 2.0f), w, h, color);                                         //right
   DrawBox(x - (h / 2.0f), y + s - w - 1, h, w, color);                                         //bottom
}


CInline void DrawCrossWithoutTop(float x, float y, float w, float h, float s, CColor<_color_prec>& color) //xpos,ypos,width,height,spread
{
   DrawBox(x - s, y - (h / 2.0f), w, h, color);                                                           //left
   DrawBox(x + s - w - 1, y - (h / 2.0f), w, h, color);                                                   //right
   DrawBox(x - (h / 2.0f), y + s - w - 1, h, w, color);                                                   //bottom
}


CInline void DrawXCross(float x, float y, float w, float h, float s, CColor<_color_prec>& color) //xpos,ypos,width,height,spread
{
   Draw2DLine(x - s, y - s, x - s + w, y - s + h, 1, color);                                     //top-left
   Draw2DLine(x + s /*-1*/, y - s, x + s - w /*-1*/, y - s + h, 1, color);                       //top-right
   Draw2DLine(x - s - 1, y + s + 1, x - s + w, y + s - h, 1, color);                             //bottom-left
   Draw2DLine(x + s, y + s + 1, x + s - w, y + s - h, 1, color);                                 //bottom-right
}


CInline void DrawC2Cross(float x, float y, float w, float h, float s, CColor<_color_prec>& color) //xpos,ypos,width,height,spread
{
   DrawBox(x - 1, y - s, 1, h, color);                                                            //top
   Draw2DLine(x - s, y + s, x - s + w, y + s - h, 1, color);                                      //bottom-left
   Draw2DLine(x + s, y + s, x + s - w - 1, y + s - h, 1, color);                                  //bottom-right
}


CInline void DrawCrosshair()
{
   CColor<_color_prec> CrosshairCol       = CColor<_color_prec>(cvar_CrosshairR->iValue, cvar_CrosshairB->iValue, cvar_CrosshairG->iValue, cvar_CrosshairA->iValue); // TODO: make macroes to do this(initialize CColor with color cvars)
   CColor<_color_prec> CrosshairShadowCol = CColor<_color_prec>(cvar_CrosshairShadowR->iValue, cvar_CrosshairShadowB->iValue, cvar_CrosshairShadowG->iValue, cvar_CrosshairShadowA->iValue);

   switch (cvar_Crosshair->iValue)
   {
   case 1:      // cross
      switch (cvar_CrosshairShadow->iValue)
      {
      case 1:           // simple shadow
         DrawCross((refdef->iWidth / 2) + 1, (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;

      case 2:           // outline
         DrawCross((refdef->iWidth / 2) - 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawCross((refdef->iWidth / 2), (refdef->iHeight / 2) - 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawCross((refdef->iWidth / 2) + 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawCross((refdef->iWidth / 2), (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;
      }

      DrawCross(refdef->iWidth / 2, refdef->iHeight / 2, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairCol);

      break;

   case 2:      // x-cross
      switch (cvar_CrosshairShadow->iValue)
      {
      case 1:           // simple shadow
         DrawXCross((refdef->iWidth / 2) + 1, (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;

      case 2:           // outline
         DrawXCross((refdef->iWidth / 2) - 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawXCross((refdef->iWidth / 2), (refdef->iHeight / 2) - 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawXCross((refdef->iWidth / 2) + 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawXCross((refdef->iWidth / 2), (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;
      }

      DrawXCross(refdef->iWidth / 2, refdef->iHeight / 2, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairCol);
      break;

   case 3:      // c2-cross
      switch (cvar_CrosshairShadow->iValue)
      {
      case 1:           // simple shadow
         DrawC2Cross((refdef->iWidth / 2) + 1, (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;

      case 2:           // outline
         DrawC2Cross((refdef->iWidth / 2) - 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawC2Cross((refdef->iWidth / 2), (refdef->iHeight / 2) - 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawC2Cross((refdef->iWidth / 2) + 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawC2Cross((refdef->iWidth / 2), (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;
      }

      DrawC2Cross(refdef->iWidth / 2, refdef->iHeight / 2, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairCol);
      break;

   case 4:      // cross without top
      switch (cvar_CrosshairShadow->iValue)
      {
      case 1:           // simple shadow
         DrawCrossWithoutTop((refdef->iWidth / 2) + 1, (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;

      case 2:           // outline
         DrawCrossWithoutTop((refdef->iWidth / 2) - 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawCrossWithoutTop((refdef->iWidth / 2), (refdef->iHeight / 2) - 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawCrossWithoutTop((refdef->iWidth / 2) + 1, (refdef->iHeight / 2), cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         DrawCrossWithoutTop((refdef->iWidth / 2), (refdef->iHeight / 2) + 1, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairShadowCol);
         break;
      }

      DrawCrossWithoutTop(refdef->iWidth / 2, refdef->iHeight / 2, cvar_CrosshairWidth->Value, cvar_CrosshairHeight->Value, cvar_CrosshairSpread->Value, CrosshairCol);
      break;
   }
}


#endif
