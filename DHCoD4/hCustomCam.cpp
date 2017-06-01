#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hConfig.h"
#include "hBot.h"
#include "hCustomCam.h"

void CCustomCam::SetViewOrigin(Vector3<_prec> *origin)
{
	this->m_viewOrigin = *origin;
}

void CCustomCam::SetViewAngles(Vector3<_prec> *angle)
{
	this->m_viewAngle = *angle;
	this->m_viewAngle.AngleVectors(&this->m_viewAxis[0], &this->m_viewAxis[1], &this->m_viewAxis[2]);
}


void CCustomCam::WorldToScreen(Vector3<_prec> *origin, Vector3<_prec> *out)
{
   _vright = this->m_viewAxis[1];
   _vfwd   = this->m_viewAxis[0];
   _vup    = this->m_viewAxis[2];
   _local  = (*origin) - this->m_viewOrigin;
   out->X  = _vright.Dot(_local, _vright);
   out->Y  = _vright.Dot(_local, _vup);
   out->Z  = _vright.Dot(_local, _vfwd);

   out->X = (this->m_w / 2.f) * (1.f - (out->X / refdef->FovX / out->Z));
   out->Y = (this->m_h / 2.f) * (1.f - (out->Y / refdef->FovY / out->Z));
}

void CCustomCam::Draw2DLine(float x1, float y1, float x2, float y2, float thickness, CColor<_color_prec>& colour)
{
}


void CCustomCam::DrawBackground()
{
}