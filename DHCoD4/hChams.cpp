// TODO: Cham Team Filter
// TODO: ditch the BeginScene hook.  move initialization code to DrawIndexedPrimitive hook.

// http://www.unknowncheats.me/forum/direct3d/70599-silhouette-chams-d3d9-update.html

#pragma warning ( disable : 4530 )
#include <windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <intrin.h>
#include <time.h>
#pragma hdrstop

#include "hDefines.h"
#include "hCrypt.h"

#ifdef CHAMS

#include "hMath.h"
#include "hConfig.h"
#include "hChams.h"
#include "hBot.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hDetour.h"
#include "hAPI.h"
#include "hLog.h"

#include "VMProtectSDK.h"

extern IDirect3DDevice9 **ppD3d9Dev;

DWORD dwDIP;
DWORD dwES;
BYTE  DIPOFF[7];
BYTE  DIPDET[7];

typedef VOID (WINAPI * tEndScene)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT (WINAPI * tDrawIndexedPrimitive)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT primCount);

tDrawIndexedPrimitive oDrawIndexedPrimitive = NULL;
tEndScene             oEndScene             = NULL;

IDirect3DDevice9 *pGameDevice;

UINT m_Stride = 0;

LPDIRECT3DTEXTURE9 texBlue       = NULL;
LPDIRECT3DTEXTURE9 texGreen      = NULL;
LPDIRECT3DTEXTURE9 texRed        = NULL;
LPDIRECT3DTEXTURE9 texYellow     = NULL;
LPDIRECT3DTEXTURE9 texPink       = NULL;
LPDIRECT3DTEXTURE9 texCyan       = NULL;
LPDIRECT3DTEXTURE9 texWhite      = NULL;
LPDIRECT3DTEXTURE9 texSilhouette = NULL;

LPDIRECT3DPIXELSHADER9 pixBlue       = NULL;
LPDIRECT3DPIXELSHADER9 pixGreen      = NULL;
LPDIRECT3DPIXELSHADER9 pixRed        = NULL;
LPDIRECT3DPIXELSHADER9 pixYellow     = NULL;
LPDIRECT3DPIXELSHADER9 pixPink       = NULL;
LPDIRECT3DPIXELSHADER9 pixCyan       = NULL;
LPDIRECT3DPIXELSHADER9 pixWhite      = NULL;
LPDIRECT3DPIXELSHADER9 pixSilhouette = NULL;

IDirect3DDevice9 *hookedDev;
LPDIRECT3DDEVICE9 pOldDev = NULL;
#define CHAMCOLOR(c)     D3DCOLOR_ARGB(255, (BYTE)c ## R->iValue, (BYTE)c ## G->iValue, (BYTE)c ## B->iValue)
#define CHAMCOLOR2(c)    ((float)c ## R->iValue / 255.0f), ((float)c ## G->iValue / 255.0f), ((float)c ## B->iValue / 255.0f), ((float)c ## A->iValue / 255.0f)

DrawnModel Models[] =
{
   "arab",     DRAWN_TEAM1,     4,
   "usmc",     DRAWN_TEAM2,     4,
   "marine",   DRAWN_TEAM2,     6,
   "op_force", DRAWN_TEAM1,     8,
   "opforce",  DRAWN_TEAM1,     7,
   "sas",      DRAWN_TEAM2,     3,
   "reflex",   DRAWN_NOTHING,   6, // to prevent the weapon reflex sight from being chammed
   "acog",     DRAWN_NOTHING,   4, // to prevent the weapon acog sight from being chammed
   "weapon",   DRAWN_WEAPON,    6,
   //"security",DRAWN_LAST_TEAM,8,
   //"accessories",DRAWN_LAST_TEAM,11,
   //"head",DRAWN_LAST_TEAM,4,
   "ghillie",  DRAWN_LAST_TEAM, 7,
   0,          DRAWN_NOTHING, 0
};

bool                   chamsSet = false;
extern DWORD           dwSafetyTime;
HRESULT                hret;
int                    iModelType;
float                  chamsVal;
bool                   bSolid = false;
UINT                   uStride;
bool                   bTrans     = false;
bool                   bTex0      = false;
bool                   bTex1      = false;
bool                   bPixShader = false;
IDirect3DBaseTexture9  *oTex0;
IDirect3DBaseTexture9  *oTex1;
DWORD                  oBLENDENABLE;
DWORD                  oDESTCOLOR;
DWORD                  oSRCCOLOR;
DWORD                  oStage0;
DWORD                  oStage1;
LPDIRECT3DPIXELSHADER9 oPixShader;
IDirect3DSurface9      *oRenderTarget;
IDirect3DSurface9      *pBackBuffer;
IDirect3DSurface9      *pRenderSurface;
IDirect3DVertexBuffer9 *pOutlineBuffer;
ID3DXBuffer            *pShaderBuf;
WORD                   *pDst16;
D3DLOCKED_RECT         d3dlr;
WORD                   colour16;
char                   *Model = NULL;
D3DCAPS9               devCaps;
DWORD                  frames = 0;
D3DSURFACE_DESC        desc;
LPDIRECT3DDEVICE9      oFrameDev = 0;

DWORD oECX;
int   iLastModelType = DRAWN_NOTHING;

typedef HRESULT (WINAPI * tPresent)(LPDIRECT3DDEVICE9 pDevice, RECT *pSourceRect, RECT *pDestRect, HWND hDestWindowOverride, RGNDATA *pDirtyRegion);
typedef HRESULT (WINAPI * tSetStreamSource)(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride);
tPresent         oPresent;
bool             bonetime = false;
tSetStreamSource oSetStreamSource;

char szShader[1024];

CInline HRESULT GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
{
   *ppD3Dtex = NULL;

   for (int i = 0; i < 5; ++i)
   {
      pD3Ddev->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL);

      if (*ppD3Dtex)
      {
         break;
      }
   }

   if (!*ppD3Dtex)
   {
      return(E_FAIL);
   }

   colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
              | (WORD)(((colour32 >> 20) & 0xF) << 8)
              | (WORD)(((colour32 >> 12) & 0xF) << 4)
              | (WORD)(((colour32 >> 4) & 0xF) << 0);

   (*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
   pDst16 = (WORD *)d3dlr.pBits;

   for (int xy = 0; xy < 8 * 8; ++xy)
   {
      *pDst16++ = colour16;
   }

   (*ppD3Dtex)->UnlockRect(0);

   return(S_OK);
}


extern float HighestThreat;
CInline HRESULT GeneratePixelShader(IDirect3DDevice9 *pD3Ddev, IDirect3DPixelShader9 **pShader, float r, float g, float b, float a)
{
   pShaderBuf = NULL;

   *pShader = NULL;

   for (int i = 0; i < 150; ++i)
   {
      sprintf_s(szShader, "ps_%d_%d\ndef c0, %f, %f, %f, %f\nmov oC0,c0", D3DSHADER_VERSION_MAJOR(devCaps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(devCaps.PixelShaderVersion), r, g, b, a); // TODO: the %f's in this sprintf is what is causing the Y version to crash
      D3DXAssembleShader(szShader, sizeof(szShader), NULL, NULL, 0, &pShaderBuf, NULL);

      if (!pShaderBuf)
      {
         sprintf_s(szShader, "ps.1.1\ndef c0, %f, %f, %f, %f\nmov r0,c0", r, g, b, a);
         D3DXAssembleShader(szShader, sizeof(szShader), NULL, NULL, 0, &pShaderBuf, NULL);
      }

      if (pShaderBuf)
      {
         break;
      }

      Sleep(1);
   }

   if (!pShaderBuf)
   {
      return(E_FAIL);
   }

   if (FAILED(pD3Ddev->CreatePixelShader((const DWORD *)pShaderBuf->GetBufferPointer(), pShader)))
   {
      pShaderBuf->Release();
      return(E_FAIL);
   }

   pShaderBuf->Release();

   return(S_OK);
}


void InitializeStuff()
{
   if ((pOldDev != pGameDevice && pGameDevice != hookedDev) && cvar_FriendlyHidB)
   {
      pGameDevice->GetDeviceCaps(&devCaps);
      pGameDevice->GetRenderTarget(0, &pBackBuffer);

      if (pBackBuffer)
      {
         pBackBuffer->GetDesc(&desc);
         pGameDevice->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texSilhouette, NULL);
         texSilhouette->GetSurfaceLevel(0, &pRenderSurface);
      }

      pOutlineBuffer;
      GenerateTexture(pGameDevice, &texYellow, CHAMCOLOR(cvar_EnemyHid));        // Enemy Hidden
      GenerateTexture(pGameDevice, &texGreen, CHAMCOLOR(cvar_FriendlyVis));      // Enemy Visible
      GenerateTexture(pGameDevice, &texRed, CHAMCOLOR(cvar_EnemyVis));           // Friendly Visible
      GenerateTexture(pGameDevice, &texBlue, CHAMCOLOR(cvar_FriendlyHid));       // Friendly Hidden
      GenerateTexture(pGameDevice, &texPink, CHAMCOLOR(cvar_WeaponVis));         // Weapon Visible
      GenerateTexture(pGameDevice, &texCyan, CHAMCOLOR(cvar_WeaponHid));         // Weapon Hidden
      GenerateTexture(pGameDevice, &texWhite, CHAMCOLOR(cvar_Wall));             // Wall

      GeneratePixelShader(pGameDevice, &pixYellow, CHAMCOLOR2(cvar_EnemyHid));   // Enemy Hidden
      GeneratePixelShader(pGameDevice, &pixGreen, CHAMCOLOR2(cvar_FriendlyVis)); // Enemy Visible
      GeneratePixelShader(pGameDevice, &pixRed, CHAMCOLOR2(cvar_EnemyVis));      // Friendly Visible
      GeneratePixelShader(pGameDevice, &pixBlue, CHAMCOLOR2(cvar_FriendlyHid));  // Friendly Hidden
      GeneratePixelShader(pGameDevice, &pixPink, CHAMCOLOR2(cvar_WeaponVis));    // Weapon Visible
      GeneratePixelShader(pGameDevice, &pixCyan, CHAMCOLOR2(cvar_WeaponHid));    // Weapon Hidden
      GeneratePixelShader(pGameDevice, &pixWhite, CHAMCOLOR2(cvar_Wall));        // Wall

      pOldDev = pGameDevice;
   }
}


void __declspec(naked) hScene()
{
   PRESERVE

      pGameDevice = *ppD3d9Dev;

   if (oFrameDev != pGameDevice)
   {
      frames    = 0;
      oFrameDev = pGameDevice;
   }

   if (frames < 20)
   {
      frames++;
   }

   else
   {
      InitializeStuff();
   }

   RESTORE

      __asm
   {
      JMP [oEndScene]
   }
}

CInline void PreCham(IDirect3DDevice9 *pDevice, int mode, LPDIRECT3DTEXTURE9 tex, LPDIRECT3DPIXELSHADER9 pix)
{
   switch (mode)
   {
   case 1:      // specular map
      if (bTex1 == false)
      {
         pDevice->GetTexture(1, &oTex1);
         bTex1 = true;
      }

      pDevice->SetTexture(1, tex);
      break;

   case 2:      // spec & diffuse map
      if (bTex1 == false)
      {
         pDevice->GetTexture(1, &oTex1);
         bTex1 = true;
      }

      if (bTex0 == false)
      {
         pDevice->GetTexture(0, &oTex0);
         bTex0 = true;
      }

      pDevice->SetTexture(1, tex);
      pDevice->SetTexture(0, tex);
      break;

   case 3:      // diffuse map
      if (bTex0 == false)
      {
         pDevice->GetTexture(0, &oTex0);
         bTex0 = true;
      }

      pDevice->SetTexture(0, tex);
      break;

   case 4:      // pix shader
      if (bPixShader == false)
      {
         pDevice->GetPixelShader(&oPixShader);
         bPixShader = true;
      }

      if (pix)
      {
         pDevice->SetPixelShader(pix);
      }

      break;
   }
}


CInline void PostCham(IDirect3DDevice9 *pDevice, int mode)
{
   switch (mode)
   {
   case 1:      // specular map
      if (bTex1)
      {
         pDevice->SetTexture(1, oTex1);
         bTex1 = false;
      }

      break;

   case 2:      // spec & diffuse map
      if (bTex0)
      {
         pDevice->SetTexture(0, oTex0);
         bTex0 = false;
      }

      if (bTex1)
      {
         pDevice->SetTexture(1, oTex1);
         bTex1 = false;
      }

      break;

   case 3:      // diffuse
      if (bTex0)
      {
         pDevice->SetTexture(0, oTex0);
         bTex0 = false;
      }

      break;

   case 4:      // pix shader
      if (bPixShader)
      {
         pDevice->SetPixelShader(oPixShader);
         bPixShader = false;
      }

      break;
   }
}


CInline void GenericPostCham(IDirect3DDevice9 *pDevice)
{
   if (bTex0)
   {
      pDevice->SetTexture(0, oTex0);
      bTex0 = false;
   }

   if (bTex1)
   {
      pDevice->SetTexture(1, oTex1);
      bTex1 = false;
   }

   if (bPixShader)
   {
      pDevice->SetPixelShader(oPixShader);
      bPixShader = false;
   }
}


HRESULT WINAPI hDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT primCount)
{
#ifdef DISABLED3D9
	return D3D_OK;
#endif
   if (!pixWhite || !pLocalClient)
   {
      return(oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount));
   }

   PRESERVE

   if (GetTickCount() <= dwSafetyTime)
   {
      goto wrong;
   }

   if ((cvar_NoSky->iValue == 1) && Sky)
   {
      return(D3D_OK);
   }

   if ((cvar_WireframeEffects->iValue == 1) && (Rain || Smoke))
   {
      pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

      hret = oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);

      pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

      RESTORE
      return(hret);
   }

   if (Walls)
   {
      if (cvar_ColoredWalls->iValue == 5)
      {
         RESTORE
         return(D3D_OK);
      }

      if (cvar_WallWireframe->iValue >= 1)
      {
         pDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)cvar_WallWireframe->iValue);
         bSolid = false;
      }

      if (cvar_WallWallhack->iValue == 1)
      {
         pDevice->SetRenderState(D3DRS_ZENABLE, false);

         oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);

         pDevice->SetRenderState(D3DRS_ZENABLE, true);
      }

      if (cvar_WallAlphaBlend->iValue == 1)
      {
         pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oBLENDENABLE);
         pDevice->GetRenderState(D3DRS_SRCBLEND, &oSRCCOLOR);
         pDevice->GetRenderState(D3DRS_DESTBLEND, &oDESTCOLOR);

         pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

         pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)cvar_ChamAlphaSrc->iValue);
         pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)cvar_ChamAlphaDst->iValue);
         bTrans = true;
      }

      if (cvar_ColoredWalls->iValue >= 1)
      {
         PreCham(pDevice, cvar_ColoredWalls->iValue, texWhite, pixWhite);
      }

      hret = oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);

      if (!bSolid)
      {
         pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
         bSolid = true;
      }

      if (bTrans)
      {
         pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oBLENDENABLE);
         pDevice->SetRenderState(D3DRS_SRCBLEND, oSRCCOLOR);
         pDevice->SetRenderState(D3DRS_DESTBLEND, oDESTCOLOR);
         bTrans = false;
      }

      GenericPostCham(pDevice);

      RESTORE
      return(hret);
   }

   if (((_ReturnAddress() == (void *)0x64695C) || (_ReturnAddress() == (void *)0x646A53) || (_ReturnAddress() == (void *)0x6569BD))) // TODO: put these return address in their own vars
   {
      _asm
      {
         MOV ECX, [EBP + 0x48]
         TEST ECX, ECX
         JE wrong
         MOV ECX, [ECX + 0xB8]
         TEST ECX, ECX
         JE wrong
         MOV ECX, [ECX]
         TEST ECX, ECX
         JE wrong
         MOV Model, ECX
      }
      iModelType = DRAWN_NOTHING;

      if ((Model[0] == 'm') && (Model[1] == 'c') && (Model[3] == 'm') && (Model[7] != 'e'))
      {
         for (int i = 0; Models[i].Model != NULL; ++i)
         {
            if (houstin_strstr(Model, Models[i].Model, Models[i].len))
            {
               iModelType = Models[i].Drawn;
               break;
            }
         }
      }
      else
      {
         goto wrong;
      }

      if (iModelType != DRAWN_NOTHING)
      {
         if (iModelType == DRAWN_WEAPON)
         {
            if (cvar_WeaponChams->iValue == 0)
            {
               goto wrong;
            }

            if (cvar_WeaponChams->iValue == 5)
            {
               RESTORE
               return(D3D_OK);
            }
         }

         else if (iModelType == DRAWN_LAST_TEAM)
         {
            iModelType = iLastModelType;
         }

         else if ((pLocalClient->Team == TEAM_FFA) || (iModelType == pLocalClient->Team))
         {
            iModelType     = DRAWN_TEAM2;
            iLastModelType = DRAWN_TEAM2;

            if (cvar_PlayerChams->iValue == 5)
            {
               RESTORE
               return(D3D_OK);
            }
         }

         else
         {
            iModelType     = DRAWN_TEAM1;
            iLastModelType = DRAWN_TEAM1;

            if (cvar_PlayerChams->iValue == 5)
            {
               RESTORE
               return(D3D_OK);
            }
         }
      }

      else
      {
         goto wrong;
      }

      if (cvar_PlayerWireframe->iValue >= 1.0f)
      {
         pDevice->SetRenderState(D3DRS_FILLMODE, (DWORD)cvar_PlayerWireframe->iValue);
         bSolid = false;
      }

      if (cvar_ChamAlpha->iValue == 1)
      {
         pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oBLENDENABLE);
         pDevice->GetRenderState(D3DRS_SRCBLEND, &oSRCCOLOR);
         pDevice->GetRenderState(D3DRS_DESTBLEND, &oDESTCOLOR);

         pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

         pDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)cvar_ChamAlphaSrc->iValue);
         pDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)cvar_ChamAlphaDst->iValue);
         bTrans = true;
      }

      if (cvar_Wallhack->iValue == 1)
      {
         pDevice->SetRenderState(D3DRS_ZENABLE, false);

         switch (iModelType)
         {
         case DRAWN_TEAM1:
            PreCham(pDevice, cvar_PlayerChams->iValue, texYellow, pixYellow);
            break;

         case DRAWN_TEAM2:
            PreCham(pDevice, cvar_PlayerChams->iValue, texBlue, pixBlue);
            break;

         case DRAWN_WEAPON:
            PreCham(pDevice, cvar_WeaponChams->iValue, texCyan, pixCyan);
            break;
         }

         oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);

         pDevice->SetRenderState(D3DRS_ZENABLE, true);
      }

      switch (iModelType)
      {
      case DRAWN_TEAM1:
         PreCham(pDevice, cvar_PlayerChams->iValue, texRed, pixRed);
         break;

      case DRAWN_TEAM2:
         PreCham(pDevice, cvar_PlayerChams->iValue, texGreen, pixGreen);
         break;

      case DRAWN_WEAPON:
         PreCham(pDevice, cvar_WeaponChams->iValue, texPink, pixPink);
         break;
      }
   }

wrong:
   ;

   hret = oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);

   if (!bSolid)
   {
      pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
      bSolid = true;
   }

   if (bTrans)
   {
      pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oBLENDENABLE);
      pDevice->SetRenderState(D3DRS_SRCBLEND, oSRCCOLOR);
      pDevice->SetRenderState(D3DRS_DESTBLEND, oDESTCOLOR);
      bTrans = false;
   }

   GenericPostCham(pDevice);

   RESTORE

   return(hret);
}


HRESULT APIENTRY hSetStreamSource(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride)
{
   m_Stride = Stride;
   return(oSetStreamSource(pDevice, StreamNumber, pStreamData, OffsetInBytes, Stride));
}


DWORD GetPageSize(void *page)
{
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(page, &mbi, sizeof(mbi));
	return mbi.RegionSize;
}
void NewD3D9Hook()
{
#if 0
	DWORD devSize = GetPageSize((void*)pGameDevice);
	hookedDev = (IDirect3DDevice9*)malloc(devSize);
	memcpy((void*)hookedDev, (void*)pGameDevice, devSize);
	(*ppD3d9Dev) = hookedDev;
	DWORD *newVtable = (DWORD*)malloc(400 * 4);
	memcpy((void*)newVtable, *(void**)hookedDev, 400 * 4);
	*(DWORD**)hookedDev = newVtable;
#endif
	hookedDev = pGameDevice;
#define VTABLE_HOOK(idx, to, tramp) (*(DWORD*)&tramp) = (*(DWORD**)hookedDev)[idx]; (*(DWORD**)hookedDev)[idx] = (DWORD)to
	VTABLE_HOOK(41, hScene, oEndScene);
	VTABLE_HOOK(82, hDrawIndexedPrimitive, oDrawIndexedPrimitive);
	VTABLE_HOOK(100, hSetStreamSource, oSetStreamSource);
}

#endif // CHAMS

#ifdef DISABLED3D9
typedef IDirect3D9* ( WINAPI* tDirect3DCreate9 )( UINT );
tDirect3DCreate9 oDirect3DCreate9 = 0;
typedef HRESULT ( WINAPI* tCreateDevice )( IDirect3D9* , UINT , D3DDEVTYPE , HWND , DWORD , D3DPRESENT_PARAMETERS* , IDirect3DDevice9** );
tCreateDevice oCreateDevice = 0;// trampoline
HRESULT APIENTRY hCreateDevice( IDirect3D9* pDirect3D, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
	HRESULT ret = oCreateDevice( pDirect3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface );

	if( *ppReturnedDeviceInterface )
	{
   DWORD *pVtable = (DWORD *)pGameDevice;
   pVtable = (DWORD *)pVtable[0];
   DetourFunction((PBYTE)pVtable[82], (PBYTE)&hDrawIndexedPrimitive, (void **)&oDrawIndexedPrimitive);
	}

	return ret;
}
IDirect3D9* APIENTRY hDirect3DCreate9( UINT uiSDKVersion )
{
	IDirect3D9* ret = oDirect3DCreate9( uiSDKVersion );

	if( ret )
	{
   DWORD *pVtable = (DWORD *)pGameDevice;
   pVtable = (DWORD *)pVtable[0];

   DetourFunction((PBYTE)pVtable[16], (PBYTE)&hCreateDevice, (void **)&oCreateDevice);
	}

	return ret;
}
#endif

MFBEG(DWORD WINAPI, ChamThread(LPVOID));
DWORD WINAPI ChamThread(LPVOID)
{
#ifdef CHAMS
   for ( ; pGameDevice == NULL; Sleep(25))
   {
      pGameDevice = *ppD3d9Dev;
   }

#if 0
   NewD3D9Hook();
#endif

#ifndef DISABLED3D9
   DWORD *pVtable = (DWORD *)pGameDevice;
   pVtable = (DWORD *)pVtable[0];

   DetourFunction((PBYTE)pVtable[41], (PBYTE)&hScene, (void **)&oEndScene);     // 41 = beginscene, 42 = endscene
   DetourFunction((PBYTE)pVtable[100], (PBYTE)&hSetStreamSource, (void **)&oSetStreamSource);
   DetourFunction((PBYTE)pVtable[82], (PBYTE)&hDrawIndexedPrimitive, (void **)&oDrawIndexedPrimitive);
#else
   DetourFunction((PBYTE)Direct3DCreate9, (PBYTE)&hDirect3DCreate9, (void **)&oDirect3DCreate9);
#endif

   MFEX(ChamThread, 1500);
#endif // CHAMS
   return(0);
}


MFEND(DWORD WINAPI, ChamThread(LPVOID));
