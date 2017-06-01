%ifndef _TZARMATH_INC
%define _TZARMATH_INC

%define TVec_t float80_t
%define TVec tword

NASMX_STRUC TVec3_t
    NASMX_RESERVE x, TVec_t, 1
    NASMX_RESERVE y, TVec_t, 1
    NASMX_RESERVE z, TVec_t, 1
NASMX_ENDSTRUC
%define TVec3_t_size (sizeof(TVec_t)*3)

;---------------------------------------------------------------------------
;precision conversion routines
;---------------------------------------------------------------------------
import  FVec3ToTVec3, ptrdiff_t pfVec3, ptrdiff_t ptVec3
import  TVec3ToFVec3, ptrdiff_t ptVec3, ptrdiff_t pfVec3
import  TVec3Copy, ptrdiff_t pfVec3, ptrdiff_t ptVec3
;---------------------------------------------------------------------------
;arithmetic routines
;---------------------------------------------------------------------------
import  TVec3Multiply, ptrdiff_t pfVec3, ptrdiff_t ptVec3
import  TVec3MultiplyT, ptrdiff_t pfVec3, ptrdiff_t tS
import  TVec3Subtract, ptrdiff_t pfVec3, ptrdiff_t ptVec3
import  TVec3Add, ptrdiff_t pfVec3, ptrdiff_t ptVec3
import TVec3Lerp, ptrdiff_t pfVec3, ptrdiff_t ptVec3, ptrdiff_t ptA
import TVec3Length, ptrdiff_t pVec3
import TVec3Distance, ptrdiff_t pfVec3, ptrdiff_t ptVec3
import TVec3Normalize, ptrdiff_t pVec3
import TVec3Null, ptrdiff_t pVec3
extern TRadToDeg ; converts st0 from radians to degrees
import  TVec3Angles, ptrdiff_t pX, ptrdiff_t pY, ptrdiff_t pZ
extern TDegToRadEx ; converts st0 from degrees(0-360) to radians
import  TVec3AngleVectors, ptrdiff_t tV, ptrdiff_t forward, ptrdiff_t right, ptrdiff_t up ; FIXME: it is possible that i didn't write this function correctly, because i didn't follow the left to right rule when solving expressions

%endif
