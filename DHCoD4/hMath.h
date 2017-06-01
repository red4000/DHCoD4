#ifndef HMATH_H
#define HMATH_H

#include <math.h>
#include "hDefines.h"
#include "Tzar.h"

typedef float   vec_t;
typedef vec_t   vec2_t[2];
typedef vec_t   vec3_t[3];
typedef vec_t   vec4_t[4];
typedef vec_t   vec5_t[5];

#define M_PI     3.141592653589793238462643383279502884197169399375105820974944
#define M_PIF    3.141592653589793238462643383279502884197169399375105820974944f

#define DEG2RAD(a)         ((a) * (M_PI / 180.0))
#define RAD2DEG(a)         ((a) * (180.0 / M_PI))
#define DEG2RADF(a)        ((a) * (M_PIF / 180.0f))
#define RAD2DEGF(a)        ((a) * (180.0f / M_PIF))
#define ANGLE2SHORT(x)     ((int)((x) * (65536.0 / 360.0)) & 0xFFFF)
#define SHORT2ANGLE(x)     ((x) * (360.0 / 65536.0))
#define ANGLE2SHORTF(x)    ((int)((x) * (65536 / 360)) & 0xFFFF)
#define SHORT2ANGLEF(x)    ((x) * (360 / 65536))
#define PITCH    0                // up / down
#define YAW      1                // left / right
#define ROLL     2                // fall over

#define DotProduct(x, y)           ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])
#define VectorSubtract(a, b, c)    ((c)[0] = (a)[0] - (b)[0], (c)[1] = (a)[1] - (b)[1], (c)[2] = (a)[2] - (b)[2])
#define VectorAdd(a, b, c)         ((c)[0] = (a)[0] + (b)[0], (c)[1] = (a)[1] + (b)[1], (c)[2] = (a)[2] + (b)[2])
#define VectorCopy(a, b)           ((b)[0] = (a)[0], (b)[1] = (a)[1], (b)[2] = (a)[2])
#define VectorScale(v, s, o)       ((o)[0] = (v)[0] * (s), (o)[1] = (v)[1] * (s), (o)[2] = (v)[2] * (s))
#define VectorMA(v, s, b, o)       ((o)[0] = (v)[0] + (b)[0] * (s), (o)[1] = (v)[1] + (b)[1] * (s), (o)[2] = (v)[2] + (b)[2] * (s))
#define VectorClear(a)             ((a)[0] = (a)[1] = (a)[2] = 0)
#define VectorNegate(a, b)         ((b)[0] = -(a)[0], (b)[1] = -(a)[1], (b)[2] = -(a)[2])
#define VectorSet(v, x, y, z)      ((v)[0] = (x), (v)[1] = (y), (v)[2] = (z))
#define Vector4Copy(a, b)          ((b)[0] = (a)[0], (b)[1] = (a)[1], (b)[2] = (a)[2], (b)[3] = (a)[3])
#define VectorInverse(a)           ((a)[0] = -(a)[0], (a)[1] = -(a)[1], (a)[2] = -(a)[2]))
#define _color_prec    float
#define _prec          float
#define _high_prec     float

template<class T = _color_prec>
class CColor
{
public:
   CColor()
   {
      r = 0;
      g = 0;
      b = 0;
      a = 1.0;
   }

   CColor(T r, T g, T b, T a)
   {
      this->r = r;
      this->g = g;
      this->b = b;
      this->a = a;
   }

   CColor(int r, int g, int b, int a)
   {
      this->r = ((T)r / 255.f);
      this->g = ((T)g / 255.f);
      this->b = ((T)b / 255.f);
      this->a = ((T)a / 255.f);
   }

   CColor<T> Lerp(CColor<T> to, float frac)
   {
      CColor<T> cret;
      cret.r = this->r + frac * (to.r - this->r);
      cret.g = this->g + frac * (to.g - this->g);
      cret.b = this->b + frac * (to.b - this->b);
      cret.a = this->a + frac * (to.a - this->a);
      return(cret);
   }

   const bool operator==(CColor<T> in)
   {
      return(r == in.r && g == in.g && b == in.b && a == in.a);
   }

   T r, g, b, a;
};

template<class T = _prec>
class Vector3
{
public:
   T X;
   T Y;
   T Z;

   CFInline Vector3()
   {
      /*
       * this->X = 0;
       * this->Y = 0;
       * this->Z = 0;
       */
   }

   CFInline Vector3(const T f)
   {
      this->X = f;
      this->Y = f;
      this->Z = f;
   }

   CFInline Vector3(const Vector3<T>& v)
   {
      this->X = v.X;
      this->Y = v.Y;
      this->Z = v.Z;
   }

   CFInline Vector3(const Vector3<T> *v)
   {
      this->X = v->X;
      this->Y = v->Y;
      this->Z = v->Z;
   }

   CFInline Vector3(const T x, const T y, const T z)
   {
      this->X = x;
      this->Y = y;
      this->Z = z;
   }

   CInline Vector3<T> operator+(const Vector3<T>& in) const
   {
      return(Vector3<T>(this->X + in.X, this->Y + in.Y, this->Z + in.Z));
   }

   CInline Vector3<T> operator+=(const Vector3<T>& in)
   {
      this->X += in.X;
      this->Y += in.Y;
      this->Z += in.Z;
      return(*this);
   }

   CInline Vector3<T> operator+(const T in) const
   {
      return(Vector3<T>(this->X + in, this->Y + in, this->Z + in));
   }

   CInline Vector3<T> operator+=(const T in)
   {
      this->X += in;
      this->Y += in;
      this->Z += in;
      return(*this);
   }

   CInline Vector3<T> operator-(const Vector3<T>& in) const
   {
      return(Vector3<T>(this->X - in.X, this->Y - in.Y, this->Z - in.Z));
   }

   CInline Vector3<T> operator-=(const Vector3<T>& in)
   {
      this->X -= in.X;
      this->Y -= in.Y;
      this->Z -= in.Z;
      return(*this);
   }

   CInline Vector3<T> operator-(const T in) const
   {
      return(Vector3<T>(this->X - in, this->Y - in, this->Z - in));
   }

   CInline Vector3<T> operator-=(const T in)
   {
      this->X -= in;
      this->Y -= in;
      this->Z -= in;
      return(*this);
   }

   CInline Vector3<T> operator*(const Vector3<T>& in) const
   {
      return(Vector3<T>(this->X * in.X, this->Y * in.Y, this->Z * in.Z));
   }

   CInline Vector3<T> operator*=(const Vector3<T>& in)
   {
      this->X *= in.X;
      this->Y *= in.Y;
      this->Z *= in.Z;
      return(*this);
   }

   CInline Vector3<T> operator*(const T in) const
   {
      return(Vector3<T>(this->X * in, this->Y * in, this->Z * in));
   }

   CInline Vector3<T> operator*=(const T in)
   {
      this->X *= in;
      this->Y *= in;
      this->Z *= in;
      return(*this);
   }

   CInline Vector3<T> operator/(const Vector3<T>& in) const
   {
      //return Vector3<T>(in*Vector3<T>(1./in.X,1./in.Y,1./in.Z));
      return(Vector3<T>(this->X / in.X, this->Y / in.Y, this->Z / in.Z));
   }

   CInline Vector3<T> operator/=(const Vector3<T>& in)
   {
      /*
       * Vector3<T> vscalar(1./in.X,1./in.Y,1./in.Z);
       * this *= vscalar;
       */
      this->X /= in.X;
      this->Y /= in.Y;
      this->Z /= in.Z;
      return(*this);
   }

   CInline Vector3<T> operator/(const T in) const
   {
      if (in == 0)
      {
         return(*this);
      }
      float scalar = 1. / in;
      return(Vector3<T>(this->X * scalar, this->Y * scalar, this->Z * scalar));
   }

   CInline Vector3<T> operator/=(const T in) const
   {
      return(Vector3<T>(this->X / in, this->Y / in, this->Z / in));
   }

   CInline bool operator==(const Vector3<T>& in) const
   {
      return(this->X == in.X && this->Y == in.Y && this->Z == in.Z);
   }

   CInline bool operator==(const T in) const
   {
      return(this->X == in && this->Y == in && this->Z == in);
   }

   CInline bool operator>(const Vector3<T>& in) const
   {
      return(this->X > in.X && this->Y > in.Y && this->Z > in.Z);
   }

   CInline bool operator>(const T in) const
   {
      return(this->X > in && this->Y > in && this->Z > in);
   }

   CInline bool operator>=(const Vector3<T>& in) const
   {
      return(this->X >= in.X && this->Y >= in.Y && this->Z >= in.Z);
   }

   CInline bool operator>=(const T in) const
   {
      return(this->X >= in && this->Y >= in && this->Z >= in);
   }

   CInline bool operator<=(const Vector3<T>& in) const
   {
      return(this->X <= in.X && this->Y <= in.Y && this->Z <= in.Z);
   }

   CInline bool operator<=(const T in) const
   {
      return(this->X <= in && this->Y <= in && this->Z <= in);
   }

   CInline bool operator<(const Vector3<T>& in) const
   {
      return(this->X < in.X && this->Y < in.Y && this->Z < in.Z);
   }

   CInline bool operator<(const T in) const
   {
      return(this->X < in && this->Y < in && this->Z < in);
   }

   CInline bool operator!=(const Vector3<T>& in) const
   {
      return(this->X != in.X && this->Y != in.Y && this->Z != in.Z);
   }

   CInline bool operator!=(const T in) const
   {
      return(this->X != in && this->Y != in && this->Z != in);
   }

   CInline T& operator[](const unsigned int i)
   {
      if (i == 0)
      {
         return(this->X);
      }
      if (i == 1)
      {
         return(this->Y);
      }
      return(this->Z);
   }

   CInline T Dot(const Vector3<T>& V1, const Vector3<T>& V2) const
   {
      return(V1.X * V2.X + V1.Y * V2.Y + V1.Z * V2.Z);
   }

   CInline T Dot(const Vector3<T>& V1, const T V2) const
   {
      return(V1.X * V2 + V1.Y * V2 + V1.Z * V2);
   }

   CInline Vector3<T> Lerp(const Vector3<T>& to, const T frac) const
   {
      return(*this + ((to - *this) * frac));
   }

   CInline Vector3<T> Angles()
   {
      Vector3<T> Angles;
      T          yaw, pitch;

      if ((this->Y == 0) && (this->X == 0))
      {
         yaw = 0;

         if (this->Z > 0)
         {
            pitch = 90;
         }
         else
         {
            pitch = 270;
         }
      }
      else
      {
         if (this->X)
         {
            yaw = RAD2DEGF(atan2(Y, X));
         }
         else if (this->Y > 0)
         {
            yaw = 90;
         }
         else
         {
            yaw = 270;
         }

         if (yaw < 0.0)
         {
            yaw += 360;
         }

         T forward = sqrt((this->X * this->X) + (this->Y * this->Y));

         pitch = RAD2DEGF(atan2(this->Z, forward));

         if (pitch < 0.0)
         {
            pitch += 360;
         }
      }

      Angles.X = -pitch;
      Angles.Y = yaw;
      Angles.Z = 0;
      Angles.Normalize360();
      return(Angles);
   }

   CInline Vector3<T> DAngles()
   {
      Vector3<T> Angles;
      TVec3      TAngles;

      FVec3ToTVec3(&this->X, &TAngles);
      TVec3Angles(&TAngles.x, &TAngles.y, &TAngles.z);
      TVec3ToFVec3(&TAngles, &Angles.X);

      /*
      Vector3<T> Angles;
      T          yaw, pitch;

      if ((this->Y == 0) && (this->X == 0))
      {
         yaw = 0;

         if (this->Z > 0)
         {
            pitch = 90;
         }
         else
         {
            pitch = 270;
         }
      }
      else
      {
         if (this->X)
         {
            yaw = RAD2DEGF(atan2(Y, X));
         }
         else if (this->Y > 0)
         {
            yaw = 90;
         }
         else
         {
            yaw = 270;
         }

         if (yaw < 0.0)
         {
            yaw += 360;
         }

         T forward = sqrt((this->X * this->X) + (this->Y * this->Y));

         pitch = RAD2DEGF(atan2(this->Z, forward));

         if (pitch < 0.0)
         {
            pitch += 360;
         }
      }

      Angles.X = -pitch;
      Angles.Y = yaw;
      Angles.Z = 0;
      Angles.Normalize360();
      return(Angles);
       */
      Angles.Normalize360();
      return(Angles);
   }

   CInline void AngleVectors(Vector3<T> *forward, Vector3<T> *right, Vector3<T> *up) const
   {
      T angle;
      T sr, sp, sy, cr, cp, cy;

      angle = this->Z * (M_PIF * 2.0f / 360.0f);
      sr    = sin(angle);
      cr    = cos(angle);

      angle = this->X * (M_PIF * 2.0f / 360.0f);
      sp    = sin(angle);
      cp    = cos(angle);

      angle = this->Y * (M_PIF * 2.0f / 360.0f);
      sy    = sin(angle);
      cy    = cos(angle);

      if (right)
      {
         right->X = (float)(-1.0 * sr * sp * cy + -1.0 * cr * -sy);
         right->Y = (float)(-1.0 * sr * sp * sy + -1.0 * cr * cy);
         right->Z = (float)(-1.0 * sr * cp);
      }

      if (forward)
      {
         forward->X = cp * cy;
         forward->Y = cp * sy;
         forward->Z = -sp;
      }

      if (up)
      {
         up->X = (cr * sp * cy + -sr * -sy);
         up->Y = (cr * sp * sy + -sr * cy);
         up->Z = cr * cp;
      }
   }

   CInline void DAngleVectors(Vector3<T> *forward, Vector3<T> *right, Vector3<T> *up) const
   {
      double angle;
      double sr, sp, sy, cr, cp, cy;

      angle = (double)this->Z * (M_PI * 2.0 / 360.0);
      sr    = sin(angle);
      cr    = cos(angle);

      angle = (double)this->X * (M_PI * 2.0 / 360.0);
      sp    = sin(angle);
      cp    = cos(angle);

      angle = (double)this->Y * (M_PI * 2.0 / 360.0);
      sy    = sin(angle);
      cy    = cos(angle);

      if (right)
      {
         right->X = (T)(-1 * sr * sp * cy + -1 * cr * -sy);
         right->Y = (T)(-1 * sr * sp * sy + -1 * cr * cy);
         right->Z = (T)(-1 * sr * cp);
      }

      if (forward)
      {
         forward->X = (T)(cp * cy);
         forward->Y = (T)(cp * sy);
         forward->Z = (T)(-sp);
      }

      if (up)
      {
         up->X = (T)(cr * sp * cy + -sr * -sy);
         up->Y = (T)(cr * sp * sy + -sr * cy);
         up->Z = (T)(cr * cp);
      }
   }

   void AnglesToAxis(const Vector3<T> *angles, Vector3<T> *axis) const
   {
      Vector3<T> right;

      // angle vectors returns "right" instead of "y axis"
      AngleVectors(angles, &axis[0], &right, &axis[2]);
      VectorSubtract(nullvec, right, &axis[1]);
      *axis[1] = -right;

      /*
       * float           length1;
       * float           yaw, pitch, roll = 0.0f;
       *
       * if(axis[0][1] == 0 && axis[0][0] == 0)
       * {
       * yaw = 0;
       * if(axis[0][2] > 0)
       * {
       *      pitch = 90;
       * }
       * else
       * {
       *      pitch = 270;
       * }
       * }
       * else
       * {
       * if(axis[0][0])
       * {
       *      yaw = (atan2(axis[0][1], axis[0][0]) * 180 / M_PI);
       * }
       * else if(axis[0][1] > 0)
       * {
       *      yaw = 90;
       * }
       * else
       * {
       *      yaw = 270;
       * }
       * if(yaw < 0)
       * {
       *      yaw += 360;
       * }
       *
       * length1 = sqrt(axis[0][0] * axis[0][0] + axis[0][1] * axis[0][1]);
       * pitch = (atan2(axis[0][2], length1) * 180 / M_PI);
       * if(pitch < 0)
       * {
       *      pitch += 360;
       * }
       *
       * roll = (atan2(axis[1][2], axis[2][2]) * 180 / M_PI);
       * if(roll < 0)
       * {
       *      roll += 360;
       * }
       * }
       *
       * angles[PITCH] = -pitch;
       * angles[YAW] = yaw;
       * angles[ROLL] = roll;
       */
   }

   CInline void Negate()
   {
      this->X = -this->X;
      this->Y = -this->Y;
      this->Z = -this->Z;
   }

   CInline Vector3<T> Abs()
   {
      Vector3<T> Temp = *this;

      if (Temp.X < 0)
      {
         Temp.X = -Temp.X;
      }

      if (Temp.Y < 0)
      {
         Temp.Y = -Temp.Y;
      }

      if (Temp.Z < 0)
      {
         Temp.Z = -Temp.Z;
      }

      return(Temp);
   }

   CInline void Normalize180()
   {
      Normalize360();

      if (this->X > 180.0)
      {
         this->X -= 360.0;
      }

      if (this->Y > 180.0)
      {
         this->Y -= 360.0;
      }

      if (this->Z > 180.0)
      {
         this->Y -= 360.0;
      }
   }

   CInline void Normalize360()
   {
      this->X = fmod(((fmod(this->X, (T)360)) + (T)360), (T)360);
      this->Y = fmod(((fmod(this->Y, (T)360)) + (T)360), (T)360);
      this->Z = fmod(((fmod(this->Z, (T)360)) + (T)360), (T)360);
   }

   /*
    * CInline Vector3<T> Unitize()
    * {
    *      return *this / this->Length();
    * }
    */

   CInline Vector3<T> Unitize()
   {
      if (this->IsNull())
      {
         return(*this);
      }
#if 0
      const T denom = ((T)1) / this->Length();
      this->Z *= denom;
      this->Y *= denom;
      this->X *= denom;
#endif
	  T denom = this->Length(); // using this instead in hopes of higher accuracy
	  this->Z /= denom;
	  this->Y /= denom;
	  this->X /= denom;
      return(*this);
   }

   CInline Vector3<T> Cross(const Vector3<T> *rhs) const
   {
      return(Vector3<T>((this->Y * rhs->Z) - (this->Z * rhs->Y), (this->Z * rhs->X) - (this->X * rhs->Z), (this->X * rhs->Y) - (this->Y * rhs->X)));
   }

   CInline T Dot(const Vector3<T> *V1) const
   {
      return(V1->X * this->X + V1->Y * this->Y + V1->Z * this->Z);
   }

   CInline void Reflect(const Vector3<T> *normal)
   {
      const T dotProductTimesTwo = Dot(normal) * 2.0f;

      this->X -= dotProductTimesTwo * normal->X;
      this->Y -= dotProductTimesTwo * normal->Y;
      this->Z -= dotProductTimesTwo * normal->Z;
   }

   CInline Vector3<T> Reflect(const Vector3<T> *vector, const Vector3<T> *normal) const
   {
      const T dotProductTimesTwo = vector->Dot(normal) * 2.0f;

      return(Vector3<T>(vector->X - (dotProductTimesTwo * normal->X), vector->Y - (dotProductTimesTwo * normal->Y), vector->Z - (dotProductTimesTwo * normal->Z)));
   }

   CInline T Distance(const Vector3<T>& V1) const
   {
      return((*this - V1).Length());
   }

   CInline bool IsNull() const
   {
      return(*this == 0.0);
   }

   CInline T Length() const
   {
      return(sqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z));
   }

   CInline T LengthFast() const
   {
      return(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
   }

   CInline T LengthFast2D() const
   {
      return(this->X * this->X + this->Y * this->Y);
   }
};

#include "hClasses.h"

class CBox
{
public:
   float Top;
   float Bottom;
   float Left;
   float Right;
};

#define square(x)    (x * x)

extern morelazies_t p;

CInline void sphere_line_intersection(
   _prec x1, _prec y1, _prec z1,
   _prec x2, _prec y2, _prec z2,
   _prec x3, _prec y3, _prec z3, _prec r)
{
   // x1,y1,z1  P1 coordinates (point of line)
   // x2,y2,z2  P2 coordinates (point of line)
   // x3,y3,z3, r  P3 coordinates and radius (sphere)
   // x,y,z   intersection coordinates
   //
   // This function returns a pointer array which first index indicates
   // the number of intersection point, followed by coordinate pairs.

   _prec a, b, c, mu, i;

   a = square(x2 - x1) + square(y2 - y1) + square(z2 - z1);
   b = 2 * ((x2 - x1) * (x1 - x3)
            + (y2 - y1) * (y1 - y3)
            + (z2 - z1) * (z1 - z3));
   c = square(x3) + square(y3) +
       square(z3) + square(x1) +
       square(y1) + square(z1) -
       2 * (x3 * x1 + y3 * y1 + z3 * z1) - square(r);
   i = b * b - 4 * a * c;

   if (i < 0.0)
   {
      // no intersection
      p.numOfIntersections = 0.0;
   }

   if (i == 0.0)
   {
      // one intersection
      p.numOfIntersections = 1.0;

      mu = -b / (2 * a);
      p.firstIntersection.X = x1 + mu * (x2 - x1);
      p.firstIntersection.Y = y1 + mu * (y2 - y1);
      p.firstIntersection.Z = z1 + mu * (z2 - z1);
   }

   if (i > 0.0)
   {
      // two intersections
      p.numOfIntersections = 2.0;

      // first intersection
      mu = (-b + sqrt(square(b) - 4 * a * c)) / (2 * a);
      p.firstIntersection.X = x1 + mu * (x2 - x1);
      p.firstIntersection.Y = y1 + mu * (y2 - y1);
      p.firstIntersection.Z = z1 + mu * (z2 - z1);
      // second intersection
      mu = (-b - sqrt(square(b) - 4 * a * c)) / (2 * a);
      p.secondIntersection.X = x1 + mu * (x2 - x1);
      p.secondIntersection.Y = y1 + mu * (y2 - y1);
      p.secondIntersection.Z = z1 + mu * (z2 - z1);
   }
}


CInline void Dsphere_line_intersection(
   _high_prec x1, _high_prec y1, _high_prec z1,
   _high_prec x2, _high_prec y2, _high_prec z2,
   _high_prec x3, _high_prec y3, _high_prec z3, _high_prec r)
{
   // x1,y1,z1  P1 coordinates (point of line)
   // x2,y2,z2  P2 coordinates (point of line)
   // x3,y3,z3, r  P3 coordinates and radius (sphere)
   // x,y,z   intersection coordinates
   //
   // This function returns a pointer array which first index indicates
   // the number of intersection point, followed by coordinate pairs.

   _high_prec a, b, c, mu, i;

   a = square(x2 - x1) + square(y2 - y1) + square(z2 - z1);
   b = 2 * ((x2 - x1) * (x1 - x3)
            + (y2 - y1) * (y1 - y3)
            + (z2 - z1) * (z1 - z3));
   c = square(x3) + square(y3) +
       square(z3) + square(x1) +
       square(y1) + square(z1) -
       2 * (x3 * x1 + y3 * y1 + z3 * z1) - square(r);
   i = b * b - 4 * a * c;

   if (i < 0.0)
   {
      // no intersection
      p.numOfIntersections = 0.0;
   }

   if (i == 0.0)
   {
      // one intersection
      p.numOfIntersections = 1.0;

      mu = -b / (2 * a);
      p.firstIntersection.X = (_prec)(x1 + mu * (x2 - x1));
      p.firstIntersection.Y = (_prec)(y1 + mu * (y2 - y1));
      p.firstIntersection.Z = (_prec)(z1 + mu * (z2 - z1));
   }

   if (i > 0.0)
   {
      // two intersections
      p.numOfIntersections = 2.0;

      // first intersection
      mu = (-b + sqrt(square(b) - 4 * a * c)) / (2 * a);
      p.firstIntersection.X = (_prec)(x1 + mu * (x2 - x1));
      p.firstIntersection.Y = (_prec)(y1 + mu * (y2 - y1));
      p.firstIntersection.Z = (_prec)(z1 + mu * (z2 - z1));
      // second intersection
      mu = (-b - sqrt(square(b) - 4 * a * c)) / (2 * a);
      p.secondIntersection.X = (_prec)(x1 + mu * (x2 - x1));
      p.secondIntersection.Y = (_prec)(y1 + mu * (y2 - y1));
      p.secondIntersection.Z = (_prec)(z1 + mu * (z2 - z1));
   }
}


CInline void sphere_line_intersection(Vector3<_prec>& rayStart, Vector3<_prec>& rayEnd, Vector3<_prec>& sphereCenter, _prec r)
{
   Dsphere_line_intersection((_high_prec)rayStart.X, (_high_prec)rayStart.Y, (_high_prec)rayStart.Z,
                             (_high_prec)rayEnd.X, (_high_prec)rayEnd.Y, (_high_prec)rayEnd.Z,
                             (_high_prec)sphereCenter.X, (_high_prec)sphereCenter.Y, (_high_prec)sphereCenter.Z,
                             (_high_prec)r);
}


#endif
