#include <windows.h>

#include "d3d9.h"
#include "hLog.h"

tDirect3DCreate9 oDirect3DCreate9;

IDirect3D9 *APIENTRY hkDirect3DCreate9(UINT SDKVersion)
{
	IDirect3D9 *d3dint = oDirect3DCreate9(SDKVersion);

	if( d3dint != NULL )
	{
		hkIDirect3D9 *ret = new hkIDirect3D9(&d3dint);
		Logf("Direct3DCreate9(%i) succeeded...", SDKVersion);
		Logf("Hooked Direct3D9 interface: 0x%x -> 0x%x", ret->m_pD3Dint, ret);
	}
	else
	{
		Logf("Direct3DCreate9(%i) failed...", SDKVersion);
	}

	return d3dint;
}
