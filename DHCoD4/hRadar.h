#ifndef HRADAR_H
#define HRADAR_H

void DrawRadar();
void CalcRadarPoint(Vector3<_prec> *point, float *outX, float *outY);
void RadarPlayer(CEntity *pEnt, CColor<_color_prec> Color);
void RadarExplosive(CEntity *pEnt, CColor<_color_prec> Color);
void RadarWeapon(CEntity *pEnt, CColor<_color_prec> Color);
void RadarHelicopter(CEntity *pEnt, CColor<_color_prec> Color);
void RadarAirstrike(CEntity *pEnt, CColor<_color_prec> Color);
void AddToCircle(CEntity *pEnt, CColor<_color_prec> Color);

#endif
