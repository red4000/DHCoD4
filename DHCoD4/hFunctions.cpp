#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"

char           pBuffer[1024];
Vector3<_prec> _local;
Vector3<_prec> _vright;
Vector3<_prec> _vup;
Vector3<_prec> _vfwd;
Vector3<_prec> _Zero(0.0f, 0.0f, 0.0f);
CTrace         _Trace;
