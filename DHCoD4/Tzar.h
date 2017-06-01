/*            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.  */

/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
 
/* THE AUTHOR OF THIS PROGRAM SHALL NOT BE HELD RESPONSIBLE OR
 * LIABLE FOR THE USE OR MISUSE OF THIS CODE, OR DAMAGES
 * ARISING FROM THE USE OR MISUSE OF THIS CODE. */


#ifndef TZAR_H
#define TZAR_H

#include "controlfp.h"

// all math functions should be called within Initialize/DeinitializeTzarMath blocks.
// Initialize/Deinitialize are not fast functions and you shouldn't call them a lot
#define InitializeTzarMath set64bitpc
#define DeinitializeTzarMath set24bitpc

// also, you shouldn't use the following functions a lot either, because they're slower than regular functions, but extremely more accurate

typedef char TFloat[10];

#pragma pack(push, 1)
typedef struct
{
   TFloat x, y, z;
} TVec3;
#pragma pack(pop)

#define proc    void __stdcall
extern "C"
{
proc FVec3ToTVec3(float *pfVec3, TVec3 *ptVec3);
proc TVec3ToFVec3(TVec3 *pfVec3, float *ptVec3);
proc TVec3Multiply(TVec3 *pfVec3, TVec3 *ptVec3); // pfVec3 *= ptVec3
proc TVec3MultiplyT(TVec3 *pfVec3, TFloat *ptVec); // pfVec3 *= ptVec3
proc TVec3Subtract(TVec3 *pfVec3, TVec3 *ptVec3); // pfVec3 -= ptVec3
proc TVec3Add(TVec3 *pfVec3, TVec3 *ptVec3); // pfVec3 += ptVec3
proc TVec3Lerp(TVec3 *pfVec3, TVec3 *ptVec3, TFloat *ptA); // *pf = (*pf * (1.0 - *ptA)) + (*pt * *ptA)
proc TVec3Length(TVec3 *ptVec3, TFloat *poutt);
proc TVec3Distance(TVec3 *pfVec3, TVec3 *ptVec3, TFloat *poutt);
proc TVec3Normalize180(TVec3 *pVec3);
proc TVec3Normalize(TVec3 *pVec3);
proc TVec3Angles(TFloat *ptX, TFloat *ptY, TFloat *ptZ);
}

#endif
