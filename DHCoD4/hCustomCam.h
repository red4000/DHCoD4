#ifndef HCUSTOMCAM_H
#define HCUSTOMCAM_H

class CCustomCam
{
public:
	void SetViewOrigin(Vector3<_prec> *origin);
	void SetViewAngles(Vector3<_prec> *angle);

	void WorldToScreen(Vector3<_prec> *origin, Vector3<_prec> *out);
	void Draw2DLine(float x1, float y1, float x2, float y2, float thickness, CColor<_color_prec>& colour);

	void DrawBackground();

protected:
	float m_x, m_y, m_w, m_h;
	Vector3<_prec> m_viewOrigin, m_viewAngle, m_viewAxis[3];
};

#endif
