#ifndef h47_H
#define h47_H

#define H_MAX_BONES    19
#define H47_PREC       double

struct HTrajectory
{
   float             time;  /// time in ms
   Vector3<H47_PREC> accel; /// acceleration
   Vector3<H47_PREC> vel;   /// velocity
   Vector3<H47_PREC> pos;   /// position
};

struct HBone
{
   WORD        id;
   HTrajectory nw;
   HTrajectory old;
   HTrajectory pred;
};

struct HVictim
{
   DWORD       i;
   HBone       bone[H_MAX_BONES];

   //entity origin trajectories
   HTrajectory nw;      // new
   HTrajectory old;
   HTrajectory pred;
};

void Update_47();

#ifdef H47
extern HVictim vic[64];
#endif // H47

#endif
