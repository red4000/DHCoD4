// TODO: create hook manager class that takes an array of hooks, and then just executes them all and suspends all other threads, etc.
// TODO: create mem patch manager class
// TODO: signateur manager class that kinda vectorizes scans for the same range

// TODO: MAJOR MAJOR TODO: organize this whole file into new files: hConfig, hMisc, hEsp, hGUI, hGameHooks?
//
//
// TODO: move menu/gui related code to hMenu
// TODO: seperate visual-only and functional features into different files.

#pragma warning ( disable : 4530 )
#include <windows.h>
#include <d3d9.h>
#include <vector>
#include <algorithm>
#include <time.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hDisplay.h"
#include "hConfig.h"
#include "hAim.h"
#include "hEsp.h"
#include "hObituary.h"
#include "hBot.h"
#include "hCrosshair.h"
#include "hRadar.h"
#include "hDefines.h"
#include "hAntiPB.h"
#include "hChams.h"
#include "h47.h"
#include "hAPI.h"
#include "hTactics.h"
#include "hPrediction.h"

#include "VMProtectSDK.h"

// TODO: move these pointer definitions to hFunctions.cpp


//--------------------------------------------------------------------------------------
//---------various data pointers--------------------------------------------------------
//--------------------------------------------------------------------------------------

CRefdef *GPTR        refdef               = (CRefdef * GPTR)               0x00797600;
CGame *GPTR          cg                   = (CGame * GPTR)                 0x0074E338;
twice_cg_t *GPTR     twice_cg             = (twice_cg_t * GPTR)            cg;
CCamera *GPTR        camera               = (CCamera * GPTR)               0x00C84F00;
CViewMatrix *GPTR    viewmatrix           = (CViewMatrix * GPTR)           0x0079E700;
CCompassEntity *GPTR compassEntities      = (CCompassEntity * GPTR)        0x00742A40;
CWeaponInfo *GPTR    cg_weapons           = (CWeaponInfo * GPTR)           0x007486D4;
CPlayerState *GPTR   predictedPlayerState = (CPlayerState * GPTR)          0x00794474;
CHUD *GPTR           hud                  = (CHUD * GPTR)                  0x00E343D8;
int *GPTR            pVoteActive          = (int *GPTR)                    0x00C6291E;
DWORD *GPTR          pWhite               = (DWORD * GPTR)                 0x0084CB30;
DWORD *GPTR          pSmalldev            = (DWORD * GPTR)                 0x0084CD8C;
int *GPTR            Zoom                 = (int *GPTR)                    0x00C5F930;
int *GPTR            pDvarunlock          = (int *GPTR)                    0x0CBA7404;
DWORD *GPTR          pNumPlayersInGame    = (DWORD * GPTR)                 0x013EB8C4;
float *GPTR          CMouseSensivity      = (float *GPTR)                  0x00C84F8C;
DWORD *GPTR          dwpMenuState         = (DWORD * GPTR)                 0x00C5F8F8;


//--------------------------------------------------------------------------------------
//---------addresses of game functions--------------------------------------------------
//--------------------------------------------------------------------------------------

DWORD GPTR dwConsoleCommand             = 0x004F9AB0;
DWORD GPTR dwGetRandomFloats            = 0x004A66E0;
DWORD GPTR dwRegisterTag                = 0x00518290;
DWORD GPTR dwGetPlayerTagAccurate       = 0x00403470;
DWORD GPTR dwGetPlayerTag               = 0x004024B0;
DWORD GPTR dwCG_Trace                   = 0x00459EF0;
DWORD GPTR dwDrawBulletTrajectory       = 0x00455A80;
DWORD GPTR dwNewPstate                  = 0x00794474;
DWORD GPTR dwCalculateWeaponSpread      = 0x00416C70;
DWORD GPTR dwCG_GetPlayerViewPoint      = 0x00456580;
DWORD GPTR dwBG_EvaluateTrajectory      = 0x0040BD70;
DWORD GPTR dwBG_EvaluateTrajectoryDelta = 0x0040C050;


//---------following is used in X,Y-----------------------------------------------------
DWORD GPTR dwCG_EchoToConsolef     = 0x004FCBC0;
DWORD GPTR dwRegisterShader        = 0x005F2A80;
DWORD GPTR dwRegisterFont          = 0x005F1EC0;
DWORD GPTR dwDrawGlowTextLng       = 0x005F6D30;
DWORD GPTR dwDrawNameTag           = 0x0042D770;
DWORD GPTR dwDrawStretchPic        = 0x005F65F0;
DWORD GPTR dwDrawStringEngine      = 0x005F6B00;
DWORD GPTR dwDrawRotatedPic2       = 0x00431490;
DWORD GPTR dwPscreenMatrix         = 0x00E343D8;
DWORD GPTR dwDeactivateFullBright  = 0x0063326C;
DWORD GPTR dwActivateFullBright    = 0x0063325B;


//---------following is only used in PBSS cleaner---------------------------------------
DWORD GPTR dwCom_Frame = 0x004FFF30;


//---------following is only used in anti DVar/CVar checks------------------------------
DWORD GPTR dwCG_DvarCreate = 0x0056C130;


//--------------------------------------------------------------------------------------
//---------used only in Y version-------------------------------------------------------
//--------------------------------------------------------------------------------------

CPlayer *GPTR    cg_player                       = (CPlayer * GPTR)                       0x0074E36C;
IDirect3DDevice9 **ppD3d9Dev                     = (IDirect3DDevice9 **)0x0CC9A408;
BYTE *GPTR       pbServerId                      = (BYTE * GPTR)                          0x00C84FE4;
DWORD            dwRenderScene                   = 0x005FAF00;
DWORD            dwObituary                      = 0x00435620;
DWORD            dwPredictPlayerState            = 0x00447260;
DWORD            dwSendCommandToConsole          = 0x004F9AB0;
DWORD            dwWritePacket                   = 0x00463F50;
void             *dwpECSRenderSceneReturnAddress = (void *GPTR)0x005FA369;


//--------------------------------------------------------------------------------------
//---------CVar pointers----------------------------------------------------------------
//--------------------------------------------------------------------------------------

CCVar *real_g_compassShowEnemies,
*real_cg_laserForceOn,
*real_cg_drawShellshock,
*real_r_fullbright,
*real_fx_draw,
*real_fx_enable,
*real_r_fog,
*real_cg_tracerSpeed,
*real_cg_tracerwidth,
*real_r_lodBiasRigid,
*real_r_lodBiasSkinned,
*real_sv_disableClientConsole,
*real_nightVisionDisableEffects,
*real_cg_debug_overlay_viewport,
*real_bg_viewKickScale,
*real_bg_viewKickMin,
*real_bg_bobMax,
*real_player_sprintCameraBob,
*real_bg_shock_lookControl,
*real_dynEnt_active,
*real_sv_clientSideBullets,
*real_r_filmTweakEnable,
*real_r_filmUseTweaks,
*real_r_glowTweakEnable,
*real_r_glowUseTweaks,
*real_cl_maxpackets,
*real_cl_packetdup,
*real_snaps,
*real_r_filmTweakContrast,
*real_r_filmTweakDesaturation,
*real_r_filmTweakBrightness,
*real_r_filmTweakInvert,
*real_cg_firstPersonTracerChance,
*real_cg_tracerchance,
*real_cg_tracerScrewRadius,
*real_cg_laserLight;



//--------------------------------------------------------------------------------------
//---------no more pointers after here--------------------------------------------------
//--------------------------------------------------------------------------------------


CColor<_color_prec>
	background(0.0f, 0.f, 0.f, 1.0f),           // generic background color
	foreground(0.04f, 0.04f, 0.04f, 1.0f),      // generic foreground color
	foreground2(0.6f, 0.6f, 0.6f, 0.8f),        // color for crosshair in the radar
	foreground3(0.15f, 0.1f, 0.1f, 0.7f),       // color for grid in the radar, and tree lines in menu
	glow(0.0f, 0.0f, 0.0f, 0.0f),               // glow color
	text(0.7f, 0.7f, 0.7f, 1.0f),               // text color
	selectedText(0.9f, 0.9f, 0.9f, 1.0f);       // highlighted text color


CColor<float> RUWhite(1.f, 1.f, 1.f, RUFlagAlpha),
	RUBlue(0.f, 0.f, 1.f, RUFlagAlpha),
	RURed(1.f, 0.f, 0.f, RUFlagAlpha);

CEntity     *pLocalEnt    = NULL;
CClientInfo *pLocalClient = NULL;
CWeapon     *pLocalWeapon = NULL;

float LocX = 15.0f, LocY = 170.0f;

#define MENU_TEXT_W_SCALE    1.0f
#define MENU_TEXT_H_SCALE    1.0f
#define MENU_TEXT_H          12 * MENU_TEXT_H_SCALE
#define MENU_FONT            NormalFont
#define MENU_WIDTH           180

bool         bHardcoreMode = false, bSpin = false, Register = false, bMenu = false, FullBright_Switch = false;
unsigned int Selected      = 0;

DWORD oWhite = 0, oSmalld = 0, EFonts[4], lastNamestealTime = 0, dwTargetinfoTime;
WORD  aimTags[20];

float                 orig_BloodSpatterScaleX = 0.0f, orig_BloodSpatterScaleY = 0.0f, fDistance[MAX_GENTITIES], tiPower;
extern Vector3<_prec> SpreadHit;

// -----------------------------------------------------
unsigned long LastVote   = 0;
int           LastClient = -1;

int iMinHealth = 85, iMaxHealth = 100;

BOOL bKnifing = FALSE;

Vector3<_prec> sway, espPos, _spinVec, vHead[MAX_PLAYERS];
char           szTargetinfo[128];

extern bool bFwd, bBwd, bLft, bRgt;

extern BOOL menuOpen;
extern int  classMenuID;
extern int  teamMenuID;

extern DWORD dwSafetyTime;

extern Vector3<_prec> BestAngles, SpreadDisplacement;
extern bool           bShouldRotate;

extern Vector3<_prec> FlyCamPos;
extern Vector3<_prec> FlyCamAngles;
extern Vector3<_prec> FlyCamViewAxis[3];
extern Vector3<_prec> FlyCamDelta;
extern float          PingCorrection;

extern Vector3<_prec> LocalPredicted, vTargFinal, vTargNeck;
extern Vector3<_prec> vTargPredict, tmpAngles, HeadBackup, SwayDisplacement;
extern DWORD          dwTargetTag;
extern int            dwTargetID;
extern float          BestPower;
extern DWORD          numVisPlayers;

char *aimBones[20];

char mpNsafe[512];

CFInline char *GetMapName()
{
   char *mpN = mpNsafe;

   strcpy_s(mpNsafe, camera->MapName);

   if (strlen(mpNsafe) > 11)
   {
	  strcpy(mpNsafe, &camera->MapName[11]);
   }

   mpN = strstr(mpNsafe, ".d3dbsp");

   if (mpN)
   {
      *mpN = '\0';
   }

   return(mpNsafe);
}


CInline void RegisterTags()
{
   for (unsigned int i = 0; i < sizeof(aimBones) / sizeof(char *); ++i)
   {
      aimTags[i] = R_RegisterTag(aimBones[i]);
   }
}


void InitializeGame()
{
#ifdef USE_EXTRA_VMP
   VMProtectBeginMutation(NULL);
#endif
   if ((oWhite != *pWhite) || (oSmalld != *pSmalldev))
   {
      if (
#ifdef VERIFY_POINTERS
         hud &&
#endif
         hud->BloodSplatterScaleX > 0.001f)
      {
         orig_BloodSpatterScaleX = hud->BloodSplatterScaleX;
         orig_BloodSpatterScaleY = hud->BloodSplatterScaleY;
      }

      DECRYPTSTREX(j_head, 7);
      DECRYPTSTREX(j_spineupper, 13);
      DECRYPTSTREX(j_spinelower, 13);
      DECRYPTSTREX(j_hip_le, 9);
      DECRYPTSTREX(j_hip_ri, 9);
      DECRYPTSTREX(j_shoulder_le, 14);
      DECRYPTSTREX(j_shoulder_ri, 14);
      DECRYPTSTREX(j_knee_le, 10);
      DECRYPTSTREX(j_knee_ri, 10);
      DECRYPTSTREX(j_ankle_le, 11);
      DECRYPTSTREX(j_ankle_ri, 11);
      DECRYPTSTREX(j_wrist_le, 11);
      DECRYPTSTREX(j_wrist_ri, 11);
      DECRYPTSTREX(j_clavicle_le, 14);
      DECRYPTSTREX(j_clavicle_ri, 14);
      DECRYPTSTREX(j_elbow_le, 11);
      DECRYPTSTREX(j_elbow_ri, 11);
      DECRYPTSTREX(j_neck, 7);
      DECRYPTSTREX(j_helmet, 9);

      aimBones[0]  = j_head;
      aimBones[1]  = j_spineupper;
      aimBones[2]  = j_spinelower;
      aimBones[3]  = j_hip_le;
      aimBones[4]  = j_hip_ri;
      aimBones[5]  = j_shoulder_le;
      aimBones[6]  = j_shoulder_ri;
      aimBones[7]  = j_knee_le;
      aimBones[8]  = j_knee_ri;
      aimBones[9]  = j_ankle_le;
      aimBones[10] = j_ankle_ri;
      aimBones[11] = j_wrist_le;
      aimBones[12] = j_wrist_ri;
      aimBones[13] = j_clavicle_le;
      aimBones[14] = j_clavicle_ri;
      aimBones[15] = j_elbow_le;
      aimBones[16] = j_elbow_ri;
      aimBones[17] = j_neck;
      aimBones[18] = j_helmet;
	  aimBones[19] = "tag_eye";

      // Tags
      RegisterTags();

      real_fx_draw = CG_GetDVarByName("fx_draw");
      
#ifdef VISUALS
      DECRYPTEX(&neck, _SEK, 5);
      DECRYPTEX(&pelvis, _SEK, 7);
      DECRYPTEX(&white, _SEK, 6);
      DECRYPTEX(&compassping_player, _SEK, 19);
      DECRYPTEX(&compassping_grenade, _SEK, 20);
      DECRYPTEX(&hud_grenadepointer, _SEK, 19);
      DECRYPTEX(&compassping_explosion, _SEK, 22);
      DECRYPTEX(&compass_objpoint_helicopter_friendly, _SEK, 37);
      DECRYPTEX(&compass_objpoint_airstrike_friendly, _SEK, 36);
      DECRYPTEX(&fonts_smallDevFont, _SEK, 19);
      DECRYPTEX(&fonts_consoleFont, _SEK, 18);
      DECRYPTEX(&fonts_normalfont, _SEK, 17);
      DECRYPTEX(&fonts_boldFont, _SEK, 15);

      Bone_Middle[0] = R_RegisterTag(j_head);
      Bone_Middle[1] = R_RegisterTag(neck);
      Bone_Middle[2] = R_RegisterTag(j_spineupper);
      Bone_Middle[3] = R_RegisterTag(j_spinelower);
      Bone_Middle[4] = R_RegisterTag(pelvis);

      Bone_Right_Arm[0] = R_RegisterTag(neck);
      Bone_Right_Arm[1] = R_RegisterTag(j_clavicle_ri);
      Bone_Right_Arm[2] = R_RegisterTag(j_shoulder_ri);
      Bone_Right_Arm[3] = R_RegisterTag(j_elbow_ri);
      Bone_Right_Arm[4] = R_RegisterTag(j_wrist_ri);

      Bone_Left_Arm[0] = R_RegisterTag(neck);
      Bone_Left_Arm[1] = R_RegisterTag(j_clavicle_le);
      Bone_Left_Arm[2] = R_RegisterTag(j_shoulder_le);
      Bone_Left_Arm[3] = R_RegisterTag(j_elbow_le);
      Bone_Left_Arm[4] = R_RegisterTag(j_wrist_le);

      Bone_Right_Leg[0] = R_RegisterTag(pelvis);
      Bone_Right_Leg[1] = R_RegisterTag(j_knee_ri);
      Bone_Right_Leg[2] = R_RegisterTag(j_ankle_ri);
      Bone_Right_Leg[3] = R_RegisterTag(j_ankle_ri);
      Bone_Right_Leg[4] = R_RegisterTag(j_ankle_ri);

      Bone_Left_Leg[0] = R_RegisterTag(pelvis);
      Bone_Left_Leg[1] = R_RegisterTag(j_knee_le);
      Bone_Left_Leg[2] = R_RegisterTag(j_ankle_le);
      Bone_Left_Leg[3] = R_RegisterTag(j_ankle_le);
      Bone_Left_Leg[4] = R_RegisterTag(j_ankle_le);

      // Shaders
      sWhite            = R_RegisterShader(white);
      sTriangle         = R_RegisterShader(compassping_player);
      sGrenade          = R_RegisterShader(compassping_grenade);
      sIsocolesTriangle = R_RegisterShader(hud_grenadepointer);
      sExplosion        = R_RegisterShader(compassping_explosion);
      sHelicopter       = R_RegisterShader(compass_objpoint_helicopter_friendly);
      sAirstrike        = R_RegisterShader(compass_objpoint_airstrike_friendly);

      // Fonts
      EFonts[0] = R_RegisterFont(fonts_smallDevFont);
      EFonts[1] = R_RegisterFont(fonts_consoleFont);
      EFonts[2] = R_RegisterFont(fonts_normalfont);
      EFonts[3] = R_RegisterFont(fonts_boldFont);

      ENCRYPTEX(&neck, _SEK, 5);
      ENCRYPTEX(&pelvis, _SEK, 7);
      ENCRYPTEX(&white, _SEK, 6);
      ENCRYPTEX(&compassping_player, _SEK, 19);
      ENCRYPTEX(&compassping_grenade, _SEK, 20);
      ENCRYPTEX(&hud_grenadepointer, _SEK, 19);
      ENCRYPTEX(&compassping_explosion, _SEK, 22);
      ENCRYPTEX(&compass_objpoint_helicopter_friendly, _SEK, 37);
      ENCRYPTEX(&compass_objpoint_airstrike_friendly, _SEK, 36);
      ENCRYPTEX(&fonts_smallDevFont, _SEK, 19);
      ENCRYPTEX(&fonts_consoleFont, _SEK, 18);
      ENCRYPTEX(&fonts_normalfont, _SEK, 17);
      ENCRYPTEX(&fonts_boldFont, _SEK, 15);

      // TODO: encrypt cvar strings
      real_g_compassShowEnemies = CG_GetDVarByName("g_compassShowEnemies");
      real_r_fog = CG_GetDVarByName("r_fog");
      real_cg_laserForceOn = CG_GetDVarByName("cg_laserForceOn");
      real_cg_drawShellshock = CG_GetDVarByName("cg_drawShellshock");
      real_cg_tracerSpeed = CG_GetDVarByName("cg_tracerSpeed");
      real_cg_tracerwidth = CG_GetDVarByName("cg_tracerwidth");
      real_r_lodBiasRigid = CG_GetDVarByName("r_lodBiasRigid");
      real_r_lodBiasSkinned = CG_GetDVarByName("r_lodBiasSkinned");
      real_sv_disableClientConsole = CG_GetDVarByName("sv_disableClientConsole");
      real_nightVisionDisableEffects = CG_GetDVarByName("nightVisionDisableEffects");
      real_cg_debug_overlay_viewport = CG_GetDVarByName("cg_debug_overlay_viewport");
      real_bg_viewKickScale = CG_GetDVarByName("bg_viewKickScale");
      real_bg_viewKickMin = CG_GetDVarByName("bg_viewKickMin");
      real_bg_bobMax = CG_GetDVarByName("bg_bobMax");
      real_player_sprintCameraBob = CG_GetDVarByName("player_sprintCameraBob");
      real_bg_shock_lookControl = CG_GetDVarByName("bg_shock_lookControl");
      real_r_filmTweakEnable = CG_GetDVarByName("r_filmTweakEnable");
      real_r_filmUseTweaks = CG_GetDVarByName("r_filmUseTweaks");
      real_r_glowTweakEnable = CG_GetDVarByName("r_glowTweakEnable");
      real_r_glowUseTweaks = CG_GetDVarByName("r_glowUseTweaks");
      real_cl_maxpackets = CG_GetDVarByName("cl_maxpackets");
      real_cl_packetdup = CG_GetDVarByName("cl_packetdup");
      real_snaps = CG_GetDVarByName("snaps");
      real_r_filmTweakContrast = CG_GetDVarByName("r_filmTweakContrast");
      real_r_filmTweakDesaturation = CG_GetDVarByName("r_filmTweakDesaturation");
      real_r_filmTweakBrightness = CG_GetDVarByName("r_filmTweakBrightness");
      real_r_filmTweakInvert = CG_GetDVarByName("r_filmTweakInvert");
      real_cg_firstPersonTracerChance = CG_GetDVarByName("cg_firstPersonTracerChance");
      real_cg_tracerchance = CG_GetDVarByName("cg_tracerchance");
      real_cg_tracerScrewRadius = CG_GetDVarByName("cg_tracerScrewRadius");
      real_cg_laserLight = CG_GetDVarByName("cg_laserLight");


      ANTI_DVAR_CHECK_DWORD(g_compassShowEnemies, 0, 0, 1);
      ANTI_DVAR_CHECK_DWORD(cg_laserForceOn, 0, 0, 1);
      ANTI_DVAR_CHECK_DWORD(cg_drawShellshock, 0, 0, 1);
      ANTI_DVAR_CHECK_DWORDEX(r_fog, 0, 0, 1, 0x0D569848);
      ANTI_DVAR_CHECK_DWORD(fx_draw, 0, 0, 1);
      ANTI_DVAR_CHECK_DWORD(nightVisionDisableEffects, 0, 0, 1);
      ANTI_DVAR_CHECK_DWORD(cg_debug_overlay_viewport, 0, 0, 1);
      ANTI_DVAR_CHECK_FLOAT(bg_bobMax, 8.0f, 0.0f, 36.0f);
      ANTI_DVAR_CHECK_FLOAT(player_sprintCameraBob, 0.5f, 0.0f, 2.0f);
      ANTI_DVAR_CHECK_FLOAT(bg_viewKickScale, 0.2f, 0.0f, 10.0f);
      ANTI_DVAR_CHECK_FLOAT(bg_viewKickMin, 5.0f, 0.0f, 90.0f);
#endif // VISUALS

      ENCRYPTSTREX(j_head, 7);
      ENCRYPTSTREX(j_spineupper, 13);
      ENCRYPTSTREX(j_spinelower, 13);
      ENCRYPTSTREX(j_hip_le, 9);
      ENCRYPTSTREX(j_hip_ri, 9);
      ENCRYPTSTREX(j_shoulder_le, 14);
      ENCRYPTSTREX(j_shoulder_ri, 14);
      ENCRYPTSTREX(j_knee_le, 10);
      ENCRYPTSTREX(j_knee_ri, 10);
      ENCRYPTSTREX(j_ankle_le, 11);
      ENCRYPTSTREX(j_ankle_ri, 11);
      ENCRYPTSTREX(j_wrist_le, 11);
      ENCRYPTSTREX(j_wrist_ri, 11);
      ENCRYPTSTREX(j_clavicle_le, 14);
      ENCRYPTSTREX(j_clavicle_ri, 14);
      ENCRYPTSTREX(j_elbow_le, 11);
      ENCRYPTSTREX(j_elbow_ri, 11);
      ENCRYPTSTREX(j_neck, 7);
      ENCRYPTSTREX(j_helmet, 9);

      real_fx_enable = CG_GetDVarByName("fx_enable");
      real_dynEnt_active = CG_GetDVarByName("dynEnt_active");
      real_sv_clientSideBullets = CG_GetDVarByName("sv_clientSideBullets");


      //PatchDeveloper();
      oWhite  = *pWhite;
      oSmalld = *pSmalldev;
   }
#ifdef USE_EXTRA_VMP
   VMProtectEnd();
#endif // USE_EXTRA_VMP
}


void DrawMenu()
{
#ifdef X_VERSION
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		bMenu = !bMenu;
	}

	if (!bMenu)
	{
		return;
	}
	
   float oLocY = LocY;
   int  MC             = Mngr.GetMenuCount();
   bool vkLeftPressed  = (GetAsyncKeyState(VK_LEFT) & 1);
   bool vkRightPressed = (GetAsyncKeyState(VK_RIGHT) & 1);
   char datestr[32];
   time_t curTime;

   CColor<_color_prec> TextColor(255, 255, 255, 255);

   DrawWindow("", LocX, LocY, MENU_WIDTH, MC * MENU_TEXT_H);

   DrawString(LocX + 3, LocY - 3, MENU_FONT, selectedText, 1.0f, HACK_NAME);

   time(&curTime);
   //strftime(datestr, sizeof(datestr), "%a %b %H:%M:%S %Y", localtime(&curTime)); // too long
   strftime(datestr, sizeof(datestr), "%I:%M %p", localtime(&curTime));
   DrawString((LocX + MENU_WIDTH) - R_TextWidth(datestr, strlen(datestr), MENU_FONT), LocY - 3, MENU_FONT, selectedText, 1.0f, datestr);

   LocY += 15.0f;

   if (GetAsyncKeyState(VK_DOWN) & 1)
   {
      // if selected is a header and it is not open
      if ((Mngr.Vars[Selected]->Type == Header) && (Mngr.Vars[Selected]->iValue == 0))
      {
         Selected = Mngr.IsLastHeader(Mngr.Vars[Selected]);
      }
      else if (Selected < Mngr.Count - 1)
      {
         if ((Mngr.Vars[Selected]->Type == Invisible) || (Mngr.Vars[Selected]->Type == Completely_Invisible) || (Mngr.Vars[Selected]->Type == Incognito))
         {
            Selected = 0;
         }

         Selected++;

         if ((Mngr.Vars[Selected]->Type == Invisible) || (Mngr.Vars[Selected]->Type == Completely_Invisible) || (Mngr.Vars[Selected]->Type == Incognito))
         {
            Selected = 0;
         }
      }
      else if (Selected == Mngr.Count - 1)
      {
         Selected = 0;
      }
   }
   else if (GetAsyncKeyState(VK_UP) & 1)
   {
      if (Selected == 0)
      {
         Selected = Mngr.FindBottomHeader();
      }
      else if (Mngr.IsSecondHeader(Mngr.Vars[Selected]))          // special case for second header
      {
         if (Mngr.Vars[0]->iValue)
         {
            Selected--;
         }
         else
         {
            Selected = 0;
         }
      }
      else
      {
         if (Mngr.Vars[Selected]->Type == Header)
         {
            if (Mngr.Vars[Mngr.GetUpHeader(Mngr.Vars[Selected])]->iValue == 0)
            {
               Selected = Mngr.GetUpHeader(Mngr.Vars[Selected]);
            }
            else
            {
               Selected--;
            }
         }
         else if (Selected > 0)
         {
            Selected--;
         }
      }
   }

   // preliminary loop, to determine maximum length of value/status strings...

   int maxValueWidth = 0;
#if 0
   bool bLMB = (GetAsyncKeyState(VK_LBUTTON) & 1),
		bRMB = (GetAsyncKeyState(VK_RBUTTON) & 1),
		bAMB = bLMB | bRMB;
   float mX = *RelMouseX, mY = *RelMouseY;
#endif

   for (unsigned int i = 0; i < Mngr.Count; i++)
   {
	   int tmpWidth;
#define TEST_STRING(x, y) tmpWidth = R_TextWidth(x, y, MENU_FONT); if (tmpWidth > maxValueWidth) { maxValueWidth = tmpWidth; }

      if ((Mngr.Vars[i]->Type != Invisible) && (Mngr.Vars[i]->Type != Completely_Invisible) && (Mngr.Vars[i]->Type != Incognito) && (Mngr.Vars[i]->parent != NULL) && (Mngr.Vars[i]->parent->iValue == 0))
      {
         continue;
      }
#if 0
	  if (bAMB)
	  {
		  if (mX > LocX && mX < (LocX + MENU_WIDTH) &&
			  mY > (LocY - MENU_TEXT_H) && mY < LocY)
		  {
			  Selected = i;

			  if (bLMB)
			  {
				  vkRightPressed = true;
				  vkLeftPressed  = false;
			  }
			  else
			  {
				  vkRightPressed = false;
				  vkLeftPressed  = true;
			  }
		  }
	  }
#endif

      if (Mngr.Vars[i]->Type != Header)
      {
		  if (Mngr.Vars[i]->Type == Toggle)
		  {
			 if (((Mngr.Vars[i]->bInt == TRUE) && (Mngr.Vars[i]->iValue == 0)) || ((Mngr.Vars[i]->bInt == FALSE) && (Mngr.Vars[i]->Value == 0.0f)))
			 {
				 TEST_STRING("Off", 3);
			 }
			 else
			 {
				 TEST_STRING("On", 2);
			 }
		  }
		  else if (Mngr.Vars[i]->Type == Plus)
		  {
			 if (Mngr.Vars[i]->Options[0] == NULL)
			 {
				if ((Mngr.Vars[i]->bInt == TRUE) || ((Mngr.Vars[i]->bInt == FALSE) && ((float)((int)Mngr.Vars[i]->Value) == Mngr.Vars[i]->Value)))
				{
					TEST_STRING(pBuffer, sprintf(pBuffer, "%i", Mngr.Vars[i]->GetIValue()));
				}
				else
				{
	//#ifdef X_VERSION
					TEST_STRING(pBuffer, sprintf(pBuffer, "%.2f", Mngr.Vars[i]->GetFValue()));
	/*#else
				   char szfix[32];
				   char *fix = gcvt((double)Mngr.Vars[i]->GetFValue(), 5, szfix);
					TEST_STRING(fix, strlen(szfix));
	#endif*/
				}
			 }
			 else
			 {
				if (Mngr.Vars[i]->Options[Mngr.Vars[i]->GetIValue()] != NULL)
				{
					TEST_STRING(Mngr.Vars[i]->Options[Mngr.Vars[i]->GetIValue()], strlen(Mngr.Vars[i]->Options[Mngr.Vars[i]->GetIValue()]));
				}
				else
				{
				 TEST_STRING("Off", 3);
				}
			 }
		  }
	  }

      LocY += MENU_TEXT_H;
   }

   LocY = oLocY + 15.0f;

   float valueX = (LocX + MENU_WIDTH) - (maxValueWidth * 0.5f);

   for (unsigned int i = 0; i < Mngr.Count; i++)
   {
      BOOL iS;

      if ((Mngr.Vars[i]->Type != Invisible) && (Mngr.Vars[i]->Type != Completely_Invisible) && (Mngr.Vars[i]->Type != Incognito) && (Mngr.Vars[i]->parent != NULL) && (Mngr.Vars[i]->parent->iValue == 0))
      {
         continue;
      }

      if (i == Selected)
      {
		  /*
		  if (0 == strcmp(Mngr.Vars[i]->Name, "Aim Key"))
		  {
			  DrawWindow("Key IDs", LocX + 200.0f, 40.0f, 150.0f, 600.0f);
			  DrawString(LocX + 201.0f, 55, NormalFont, selectedText, 0.8f, "1               Left mouse button\n 2               Right mouse button\n 3               Control-break processing\n 4               Middle mouse button\n 8               BACKSPACE\n 9               TAB\n12               CLEAR\n13               ENTER\n16               SHIFT\n17               CTRL\n18               ALT\n19               PAUSE\n20               CAPS LOCK\n27               ESC\n32               SPACEBAR\n33               PAGE UP\n34               PAGE DOWN\n35               END\n36               HOME\n37               LEFT ARROW\n38               UP ARROW\n39               RIGHT ARROW\n40               DOWN ARROW\n41               SELECT\n42               PRINT\n43               EXECUTE\n44               PRINT SCREEN\n45               INS\n46               DEL\n47               HELP\n48               0\n49               1\n50               2\n51               3\n52               4\n53               5\n54               6\n55               7\n56               8\n57               9\n58               A\n59               B\n60               C\n61               D\n62               E\n63               F\n64               G\n65               H\n66               I\n67               J\n68               K\n69               L\n70               M\n71               N\n72               O\n73               P\n74               Q\n75               R\n76               S\n77               T\n78               U\n79               V\n80               W\n81               X\n82               Y\n83               Z\n96               Numpad 0\n97               Numpad 1\n98               Numpad 2\n99               Numpad 3\n100              Numpad 4\n101              Numpad 5\n102              Numpad 6\n103              Numpad 7\n104              Numpad 8\n105              Numpad 9\n108              Separator\n109              Subtract\n110              Decimal\n111              Divide\n112              F1\n113              F2\n114              F3\n115              F4\n116              F5\n117              F6\n118              F7\n119              F8\n120              F9\n121              F10\n122              F11\n123              F12\n124              F13\n125              F14\n126              F15\n127              F16\n128              F17\n129              F18\n130              F19\n131              F20\n132              F21\n133              F22\n134              F23\n135              F24\n144              NUM LOCK\n145              SCROLL LOCK\n160              Left SHIFT\n161              Right SHIFT\n162              Left CONTROL\n163              Right CONTROL\n164              Left MENU\n165              Right MENU\n250              Play\n251              Zoom");
		  }
		  */
         TextColor = selectedText;

         if (vkLeftPressed)
         {
            Mngr.Vars[i]->Decrement();
         }
         else if (vkRightPressed)
         {
            Mngr.Vars[i]->Increment();
         }

         iS = TRUE;

         DrawBox(LocX, LocY - MENU_TEXT_H, MENU_WIDTH, MENU_TEXT_H, glow);
         DrawGlowText(MENU_FONT, LocX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "->");
      }
      else
      {
         TextColor = text;

		 iS = FALSE;
      }

      if (Mngr.Vars[i]->Type == Header)
      {
         if (iS)
         {
            if (Mngr.Vars[i]->iValue == 0)
            {
               DrawGlowTextf(MENU_FONT, LocX + 15, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "   %s", Mngr.Vars[i]->Name);
            }
            else
            {
               DrawGlowTextf(MENU_FONT, LocX + 15, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "   %s", Mngr.Vars[i]->Name);
            }
         }
         else
         {
            if (Mngr.Vars[i]->iValue == 0)
            {
               DrawGlowTextf(MENU_FONT, LocX + 15, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "   %s", Mngr.Vars[i]->Name);
            }
            else
            {
               DrawGlowTextf(MENU_FONT, LocX + 15, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "   %s", Mngr.Vars[i]->Name);
            }
         }
      }
      else if ((Mngr.Vars[i]->Type != Invisible) && (Mngr.Vars[i]->Type != Completely_Invisible) && (Mngr.Vars[i]->Type != Incognito))
      {
		DrawBox(LocX + 20, LocY - MENU_TEXT_H, 1, MENU_TEXT_H, foreground3);
		DrawBox(LocX + 20, LocY - (MENU_TEXT_H * 0.5f), 5, 1, foreground3);
         if (iS)
         {
			DrawGlowTextf(MENU_FONT, LocX + 20, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "   %s", Mngr.Vars[i]->Name);
		 }
		 else
		 {
			DrawGlowTextf(MENU_FONT, LocX + 20, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "   %s", Mngr.Vars[i]->Name);
		 }
      }

#if 0
      if (((Mngr.Vars[i]->bInt == TRUE) && (Mngr.Vars[i]->iValue == 0)) || ((Mngr.Vars[i]->bInt == FALSE) && (Mngr.Vars[i]->Value == 0.0f)))
      {
         if (!iS)
         {
            TextColor = CColor<_color_prec>(250, 90, 90, 255);
         }
         else
         {
            TextColor = CColor<_color_prec>(250, 150, 150, 255);
         }
      }
      else
      {
         if (!iS)
         {
            TextColor = CColor<_color_prec>(90, 250, 90, 255);
         }
         else
         {
            TextColor = CColor<_color_prec>(150, 250, 150, 255);
         }
      }
#endif

      if (Mngr.Vars[i]->Type == Toggle)
      {
         if (((Mngr.Vars[i]->bInt == TRUE) && (Mngr.Vars[i]->iValue == 0)) || ((Mngr.Vars[i]->bInt == FALSE) && (Mngr.Vars[i]->Value == 0.0f)))
         {
            DrawCenteredGlowText(MENU_FONT, valueX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "Off");
         }
         else
         {
            DrawCenteredGlowText(MENU_FONT, valueX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "On");
         }
      }
      else if (Mngr.Vars[i]->Type == Plus)
      {
         if (Mngr.Vars[i]->Options[0] == NULL)
         {
            if ((Mngr.Vars[i]->bInt == TRUE) || ((Mngr.Vars[i]->bInt == FALSE) && ((float)((int)Mngr.Vars[i]->Value) == Mngr.Vars[i]->Value)))
            {
               DrawCenteredGlowTextf(MENU_FONT, valueX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "%i", Mngr.Vars[i]->GetIValue());
            }
            else
            {
#ifdef X_VERSION
               DrawCenteredGlowTextf(MENU_FONT, valueX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "%.2f", Mngr.Vars[i]->GetFValue());
#else
               char szfix[32];
               char *fix = gcvt((double)Mngr.Vars[i]->GetFValue(), 5, szfix);
               DrawCenteredGlowText(MENU_FONT, valueX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, fix);
#endif
            }
         }
         else
         {
            if (Mngr.Vars[i]->Options[Mngr.Vars[i]->GetIValue()] != NULL)
            {
               DrawCenteredGlowTextf(MENU_FONT, valueX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "%s", Mngr.Vars[i]->Options[Mngr.Vars[i]->GetIValue()]);
            }
            else
            {
               DrawCenteredGlowText(MENU_FONT, valueX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "Off");
            }
         }
      }

      LocY += MENU_TEXT_H;
   }

   LocY = oLocY;

#if 0
   DrawStretchPic(mX - 16, mY - 16, 32, 32, 0.0f, 0.0f, 1.0f, 1.0f, NULL, sCursor);
#endif
#else

   float oLocY = LocY;
   //float mX = *RelMouseX, mY = *RelMouseY;
   //bool Clicked = GetAsyncKeyState(VK_LBUTTON) & 1;
   int  MC             = Mngr.GetMenuCount();
   bool vkLeftPressed  = (GetAsyncKeyState(VK_LEFT) & 1);
   bool vkRightPressed = (GetAsyncKeyState(VK_RIGHT) & 1);

   CColor<_color_prec> TextColor(255, 255, 255, 255);

   // TODO: make width customizable by a #define
   DrawOutline(LocX - 2, LocY - 22, 241, (MC * MENU_TEXT_H) + 24.0f, cvar_GUIOutlineThickness->Value, background);
   DrawBox(LocX - 2, LocY - 4, 241, 2, background);
   DrawBox(LocX - 2, LocY - 22, 241, 18, foreground);
   DrawGlowText(HACK_NAME " Menu", 20, NormalFont, LocX + 43, LocY - 5, 1.0f, 1.0f, 0.0f, CColor<_color_prec>(0.8f, 0.8f, 0.8f, 0.7f), GLOW_MORE_SHADOWED, glow);
   DrawBox(LocX - 2, LocY - 2, 241, (MC * MENU_TEXT_H) + 4.0f, foreground);
   LocY += 12.0f;

   if (GetAsyncKeyState(VK_DOWN) & 1)
   {
      // if selected is a header and it is not open
      if ((Mngr.Vars[Selected]->Type == Header) && (Mngr.Vars[Selected]->iValue == 0))
      {
         Selected = Mngr.IsLastHeader(Mngr.Vars[Selected]);
      }

      else if (Selected < Mngr.Count - 1)
      {
         if ((Mngr.Vars[Selected]->Type == Invisible) || (Mngr.Vars[Selected]->Type == Completely_Invisible) || (Mngr.Vars[Selected]->Type == Incognito))
         {
            Selected = 0;
         }

         Selected++;

         if ((Mngr.Vars[Selected]->Type == Invisible) || (Mngr.Vars[Selected]->Type == Completely_Invisible) || (Mngr.Vars[Selected]->Type == Incognito))
         {
            Selected = 0;
         }
      }

      else if (Selected == Mngr.Count - 1)
      {
         Selected = 0;
      }
   }

   else if (GetAsyncKeyState(VK_UP) & 1)
   {
      if (Selected == 0)
      {
         Selected = Mngr.FindBottomHeader();
      }

      else if (Mngr.IsSecondHeader(Mngr.Vars[Selected]))          // special case for second header
      {
         if (Mngr.Vars[0]->iValue)
         {
            Selected--;
         }

         else
         {
            Selected = 0;
         }
      }

      else
      {
         if (Mngr.Vars[Selected]->Type == Header)
         {
            if (Mngr.Vars[Mngr.GetUpHeader(Mngr.Vars[Selected])]->iValue == 0)
            {
               Selected = Mngr.GetUpHeader(Mngr.Vars[Selected]);
            }

            else
            {
               Selected--;
            }
         }

         else if (Selected > 0)
         {
            Selected--;
         }
      }
   }

   for (unsigned int i = 0; i < Mngr.Count; i++)
   {
      BOOL iS = FALSE;

      if ((Mngr.Vars[i]->Type != Invisible) && (Mngr.Vars[i]->Type != Completely_Invisible) && (Mngr.Vars[i]->Type != Incognito) && (Mngr.Vars[i]->parent != NULL) && (Mngr.Vars[i]->parent->iValue == 0))
      {
         continue;
      }

      if (i == Selected)
      {
         TextColor = CColor<_color_prec>(1.0f, 1.0f, 1.0f, 1.0f);

         if (vkLeftPressed)
         {
            Mngr.Vars[i]->Decrement();
         }

         else if (vkRightPressed)
         {
            Mngr.Vars[i]->Increment();
         }

         iS = TRUE;
      }

      else
      {
         TextColor = CColor<_color_prec>(120, 120, 120, 255);                // TODO: move to some definition
      }

      if (iS)
      {
         DrawBox(LocX, LocY - 12.0f, 241, 12.0f, glow);
      }

      if (Mngr.Vars[i]->Type == Header)
      {
         if (iS)
         {
            if (Mngr.Vars[i]->iValue == 0)
            {
               DrawGlowTextf(ConsoleFont, LocX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, selectedText, GLOW_SHADOWED, glow, "%s ->", Mngr.Vars[i]->Name);
            }

            else
            {
               DrawGlowTextf(ConsoleFont, LocX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, selectedText, GLOW_SHADOWED, glow, "%s <-", Mngr.Vars[i]->Name);
            }
         }

         else
         {
            if (Mngr.Vars[i]->iValue == 0)
            {
               DrawGlowTextf(ConsoleFont, LocX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, text, GLOW_SHADOWED, glow, "%s ->", Mngr.Vars[i]->Name);
            }

            else
            {
               DrawGlowTextf(ConsoleFont, LocX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, text, GLOW_SHADOWED, glow, "%s <-", Mngr.Vars[i]->Name);
            }
         }
      }

      else if ((Mngr.Vars[i]->Type != Invisible) && (Mngr.Vars[i]->Type != Completely_Invisible) && (Mngr.Vars[i]->Type != Incognito))
      {
         DrawGlowTextf(ConsoleFont, LocX, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "%s", Mngr.Vars[i]->Name);
      }

      if (((Mngr.Vars[i]->bInt == TRUE) && (Mngr.Vars[i]->iValue == 0)) || ((Mngr.Vars[i]->bInt == FALSE) && (Mngr.Vars[i]->Value == 0.0f)))
      {
         if (!iS)
         {
            TextColor = CColor<_color_prec>(250, 90, 90, 255);
         }
         else
         {
            TextColor = CColor<_color_prec>(250, 150, 150, 255);
         }
      }

      else
      {
         if (!iS)
         {
            TextColor = CColor<_color_prec>(90, 250, 90, 255);
         }
         else
         {
            TextColor = CColor<_color_prec>(150, 250, 150, 255);
         }
      }

      if (Mngr.Vars[i]->Type == Toggle)
      {
         if (((Mngr.Vars[i]->bInt == TRUE) && (Mngr.Vars[i]->iValue == 0)) || ((Mngr.Vars[i]->bInt == FALSE) && (Mngr.Vars[i]->Value == 0.0f)))
         {
            DrawGlowText(ConsoleFont, LocX + 190.0f, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "Off");
         }

         else
         {
            DrawGlowText(ConsoleFont, LocX + 190.0f, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "On");
         }
      }

      else if (Mngr.Vars[i]->Type == Plus)
      {
         if (Mngr.Vars[i]->Options[0] == NULL)
         {
            if ((Mngr.Vars[i]->bInt == TRUE) || ((Mngr.Vars[i]->bInt == FALSE) && ((float)((int)Mngr.Vars[i]->Value) == Mngr.Vars[i]->Value)))
            {
               DrawGlowTextf(ConsoleFont, LocX + 190.0f, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "%i", Mngr.Vars[i]->GetIValue());
            }

            else
            {
#ifdef X_VERSION
               DrawGlowTextf(ConsoleFont, LocX + 190.0f, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "%.2f", Mngr.Vars[i]->GetFValue());
#else
               char szfix[32];
               char *fix = gcvt((double)Mngr.Vars[i]->GetFValue(), 5, szfix);
               DrawGlowText(ConsoleFont, LocX + 190.0f, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, fix);
#endif
            }
         }

         else
         {
            if (Mngr.Vars[i]->Options[Mngr.Vars[i]->GetIValue()] != NULL)
            {
               DrawGlowTextf(ConsoleFont, LocX + 190.0f, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "%s", Mngr.Vars[i]->Options[Mngr.Vars[i]->GetIValue()]);
            }

            else
            {
               DrawGlowText(ConsoleFont, LocX + 190.0f, LocY, MENU_TEXT_W_SCALE, MENU_TEXT_H_SCALE, 0.0f, TextColor, GLOW_SHADOWED, glow, "Off");
            }
         }
      }

      LocY += MENU_TEXT_H;
   }

   LocY = oLocY;
#endif
}


CInline void DrawInfoBar()
{
   DrawOutline(0.0f, 0.0f, (float)refdef->iWidth, 35.0f, cvar_GUIOutlineThickness->Value, background);
   DrawBox(0.0f, 0.0f, (float)refdef->iWidth, 35.0f, foreground);
   DrawRUFlag(0.0f, 0.0f, (float)refdef->iWidth, 35.0f);

   DrawGlowTextf(NormalFont, (((float)refdef->iWidth) * 0.2f), 26.0f, 1.1f, 1.1f, 0.0f, CColor<_color_prec>(255, 255, 255, 255), GLOW_NORMAL, glow, DS("^3(" HACK_NAME "^3) (Ping: ^7%i^3)  (Map: ^7%s^3)  (Name: ^7%s^3)"), cg->NextSnap->Ping, GetMapName(), pLocalClient->Name);
   //DrawGlowTextf(NormalFont, (((float)refdef->iWidth) * 0.2f), 26.0f, 1.1f, 1.1f, 0.0f, CColor<_color_prec>(255, 255, 255, 255), GLOW_NORMAL, glow, DS("^3(" HACK_NAME " ^7%s %s^3) (Ping: ^7%i^3)  (Map: ^7%s^3)  (Name: ^7%s^3)"), __TIME__, __DATE__, cg->NextSnap->Ping, GetMapName(), pLocalClient->Name);
}


CInline void AutoVote()
{
   if ((*pVoteActive) == 0)
   {
      if (*pNumPlayersInGame > 1)          // there is more than 1 player in the game
      {
         DWORD CurrentTime = GetTickCount();

         if ((CurrentTime - LastVote) > 250)               // because it takes time for the server to process your vote
         {
            CG_ConsoleCommand(RandomizeColors("callvote map \"^1" HACK_NAME "!\n"));
            LastVote = CurrentTime;
         }
      }
   }
}


CInline void StealName()
{
   CClientInfo *nsCL[MAX_PLAYERS];
   int         nsCN[MAX_PLAYERS];
   char        sNamesteal[128];
   DWORD       nsCI = 0;

   for (int i = 0; i < MAX_PLAYERS; ++i)
   {
      CClientInfo *ci = GetClientByIndex(i);

      if ((i != cg->clientNum) && (LastClient != i))
      {
         if (strlen(ci->Name))
         {
            nsCN[nsCI]   = i;
            nsCL[nsCI++] = ci;
         }
      }
   }

   if (nsCI > 0)
   {
      DWORD nsPI = hrand() % nsCI;
      sprintf_s(sNamesteal, "set name \"%s\"", nsCL[nsPI]->Name);
      CG_ConsoleCommand(sNamesteal);
      LastClient = nsCN[nsPI];
   }
}


void AntiDeathThread(LPVOID)
{
#ifdef GHOST_CVARS
   Sleep(250);

   BYTE bServerID = *pbServerId;
   char szAntiDeath[512];

   sprintf_s(szAntiDeath, "mr %d %d autoassign\n", (DWORD)bServerID, (DWORD)teamMenuID);
   bIgnoreNextCmd = true;
   CG_ConsoleCommand(szAntiDeath);

   Sleep(250);

   sprintf_s(szAntiDeath, "mr %d %d autoassign %s\n", (DWORD)bServerID, (DWORD)teamMenuID, szClass);
   bIgnoreNextCmd = true;
   CG_ConsoleCommand(szAntiDeath);
#endif // GHOST_CVARS
}


CInline void SwitchTeam()
{
   BYTE bServerID = *pbServerId;
   char szAntiDeath[512];

   sprintf_s(szAntiDeath, "mr %d %d autoassign\n", (DWORD)bServerID, (DWORD)teamMenuID);

   bIgnoreNextCmd = true;
   CG_ConsoleCommand(szAntiDeath);
   CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AntiDeathThread, NULL, 0, NULL);
}


void AntiDeath()
{
   if ((cg_player->health > 0) && pLocalEnt->Alive)   // Player is alive <- check redundent?
   {
      // TODO: use fMinHealth from healthbar, be sure that the healthbar drawing is called before antideath and even if healthbar is off, do checks within the function....
      // hardcore mode check
      if ((cg_player->health == 30) && !bHardcoreMode)
      {
         bHardcoreMode = true;
      }

      else if ((cg_player->health == 100) && bHardcoreMode)
      {
         bHardcoreMode = false;
      }

      // Set minimum health based on mode
      if (bHardcoreMode && (iMinHealth != 29))        // Hardcore Mode
      {
         iMinHealth = 29;
      }

      else if (!bHardcoreMode && (iMinHealth != 85))            // Non Hardcore Mode
      {
         iMinHealth = 85;
      }

      if (cg_player->health > iMaxHealth)
      {
         iMaxHealth = cg_player->health;
      }

      // Do the antideath
      if (cg_player->health <= iMinHealth)
      {
         SwitchTeam();
      }
   }
}


void DisableVisuals()
{
#ifdef VISUALS
   if(FullBright_Switch)
   {
   __asm CALL dwActivateFullBright
   FullBright_Switch = false;
   }
   SAFE_CVAR_WRITE_DWORD(real_r_filmTweakEnable,0);
   SAFE_CVAR_WRITE_DWORD(real_r_filmUseTweaks,0);
   SAFE_CVAR_WRITE_DWORD(real_r_glowTweakEnable,0);
   SAFE_CVAR_WRITE_DWORD(real_r_glowUseTweaks,0);
   SAFE_CVAR_WRITE_FLOAT(real_cg_tracerSpeed,7500.0f);
   SAFE_CVAR_WRITE_DWORD(real_cg_laserForceOn, 0);
   SAFE_CVAR_WRITE_DWORD(real_cg_drawShellshock, 1);
   SAFE_CVAR_WRITE_DWORD(real_fx_draw, 1);
   SAFE_CVAR_WRITE_DWORD(real_r_fog, 1);
   SAFE_CVAR_WRITE_DWORD(real_g_compassShowEnemies, 0);
   SAFE_CVAR_WRITE_FLOAT(real_bg_viewKickScale, 0.2f);
   SAFE_CVAR_WRITE_FLOAT(real_bg_viewKickMin, 5.0f);
   SAFE_CVAR_WRITE_FLOAT(real_bg_bobMax, 8.0f);
   SAFE_CVAR_WRITE_FLOAT(real_player_sprintCameraBob, 0.5f);
   SAFE_CVAR_WRITE_DWORD(real_nightVisionDisableEffects, 0);
   SAFE_CVAR_WRITE_DWORD(real_cg_debug_overlay_viewport, 0);
#endif // VISUALS
}


bool sortpredicate(Sortable_t i, Sortable_t j)
{
	return (i._a > j._a);
}

extern float aaFix;

void Frame()
{
   if (!cg->CurrentSnap || !cg->NextSnap)
   {
      return;
   }

   pLocalEnt    = GetEntityByIndex(cg->clientNum);
   pLocalClient = GetClientByIndex(cg->clientNum);

   InitializeGame();

   if (!cg->CurrentSnap || !cg->NextSnap)
   {
      return;
   }

   if (!pLocalEnt->IsValidEnt || (pLocalEnt->EntityType != TYPE_HUMAN))
   {
      return;
   }

   if (*dwpMenuState & MENU_OPEN_FLAG)
   {
      hud->BloodSplatterScaleX = orig_BloodSpatterScaleX;
      hud->BloodSplatterScaleY = orig_BloodSpatterScaleY;
   }

   else
   {
      if (cvar_NoHUDBlood->iValue == 1)
      {
         hud->BloodSplatterScaleX = 0.0f;
         hud->BloodSplatterScaleY = 0.0f;
      }

      else
      {
         hud->BloodSplatterScaleX = orig_BloodSpatterScaleX;
         hud->BloodSplatterScaleY = orig_BloodSpatterScaleY;
      }
   }

   float    fRecoilScalar;
   CWeapon *myWep;

   switch (cvar_NoRecoil->iValue)
   {
   case 0:
      break;

   case 1:
      viewmatrix->Recoil[0] = 0.0f;
      viewmatrix->Recoil[1] = 0.0f;
      viewmatrix->Recoil[2] = 0.0f;
      break;

   case 101:
      myWep = GetWeaponByIndex(pLocalEnt->WeaponId);

      if (myWep)
      {
         myWep->adsGunKickPitchMin = 0.0f;
         myWep->adsGunKickPitchMax = 0.0f;
         myWep->adsGunKickYawMin   = 0.0f;
         myWep->adsGunKickYawMax   = 0.0f;
         myWep->adsGunKickAccel    = 0.0f;
         myWep->adsGunKickSpeedMax = 0.0f;

         myWep->hipGunKickPitchMin = 0.0f;
         myWep->hipGunKickPitchMax = 0.0f;
         myWep->hipGunKickYawMin   = 0.0f;
         myWep->hipGunKickYawMax   = 0.0f;
         myWep->hipGunKickAccel    = 0.0f;
         myWep->hipGunKickSpeedMax = 0.0f;

         myWep->hipViewKickPitchMin = 0.0f;
         myWep->hipViewKickPitchMax = 0.0f;
         myWep->hipViewKickYawMin   = 0.0f;
         myWep->hipViewKickYawMax   = 0.0f;

         // following lines let you zoom in but still look like you're not zoomed in, so you could fake a nospread video or something lol.
         //myWep->horizViewJitter = 0.0f;
         //myWep->vertViewJitter = 0.0f;
      }
      break;

   default:
      fRecoilScalar          = (100.0f - ((float)cvar_NoRecoil->iValue)) / 100.0f;
      viewmatrix->Recoil[0] *= fRecoilScalar;
      viewmatrix->Recoil[1] *= fRecoilScalar;
      viewmatrix->Recoil[2] *= fRecoilScalar;
      break;
   }

   numVisPlayers = 0;

   if (GetTickCount() > dwSafetyTime)
   {
      if (cvar_Radar->iValue == 1)
      {
         DrawRadar();
      }

      for (int i = MAX_PLAYERS; i < MAX_GENTITIES; i++)
      {
         CEntity *pEnt = GetEntityByIndex(i);

         if (pEnt->Alive && pEnt->IsValidEnt)
         {
            fDistance[i] = (GetLocalViewpoint() - pEnt->vOrigin).Length() / DIST_DIVISOR;
            EESP(pEnt);
         }
      }

      bKnifing = FALSE;

      int AutoKnifeTarget = -1;

#if 0
      Update_47();
#endif

      Sortable_t sortedPlayers[MAX_PLAYERS];
	  size_t spIdx = 0;

      for (int i = 0; i < MAX_PLAYERS; ++i)
      {
         CEntity     *pEnt    = GetEntityByIndex(i);
         CClientInfo *pClient = GetClientByIndex(i);

         if (pEnt->Alive && pEnt->IsValidEnt && ((i != cg->clientNum) || (FlyCamEnabled)) && (pEnt->EntityType == TYPE_HUMAN) && ((pClient->Team == TEAM_FFA) || (pClient->Team != GetClientByIndex(cg->clientNum)->Team) || (cvar_TeamFilter->Value == 0.0f)))
         {
            float fDist = (GetLocalViewpoint() - pEnt->vOrigin).Length() / DIST_DIVISOR;
            fDistance[i] = fDist;
            vHead[i]     = GetPlayerTag(aimTags[0], pEnt);
			ApplyPrediction3(true, pEnt, NULL);

            sortedPlayers[spIdx]._a = fDist;
            sortedPlayers[spIdx++]._b = i;

            if ((cvar_AutoKnife->iValue == 1) && (fDist <= cvar_AutoKnifeRange->Value) && ((pClient->Team != pLocalClient->Team) || (pClient->Team == TEAM_FFA)) && bAimbotIsReady() && VisTrace(GetLocalViewpoint(), pEnt->vOrigin, pEnt->ClientNumber) && pLocalEnt->Alive && (i != cg->clientNum) && !cg->demoPlayback)
            {
               AutoKnifeTarget = i;
            }
         }
      }

      if ((AutoKnifeTarget > -1) && (numVisPlayers <= cvar_AutoKnifeMaxHostiles->uiValue) && pLocalEnt->Alive && pLocalEnt->IsValidEnt && (pLocalEnt->EntityType == TYPE_HUMAN))
      {
         BestAngles = (vHead[AutoKnifeTarget] - GetLocalViewpoint()).DAngles() - refdef->ViewAngles;
         AddViewAngleDelta(BestAngles);
         BestAngles.Normalize180();
         bShouldRotate = true;
         CG_ConsoleCommand(DS("+melee"));
         CG_ConsoleCommand(DS("-melee"));
         bKnifing = TRUE;
      }
      else
      {
         bKnifing = FALSE;
      }

      if (spIdx)
      {
		  std::sort(&sortedPlayers[0],&sortedPlayers[spIdx],sortpredicate);

         for (size_t i = 0; i < spIdx; ++i)
         {
            ESP(GetEntityByIndex(sortedPlayers[i]._b), GetClientByIndex(sortedPlayers[i]._b));
         }
      }
      
      if ((cvar_PlayerAim->iValue == 1) && pLocalEnt->Alive && pLocalEnt->IsValidEnt && (pLocalEnt->EntityType == TYPE_HUMAN) && !cg->demoPlayback)
      {
         if ((dwTargetID != -1) && (BestPower > 0.0f))
         {
            CEntity *pTargEnt = GetEntityByIndex(dwTargetID);

            if (cvar_SilentAim->iValue == 0)
            {
               if (pTargEnt->Alive && pTargEnt->IsValidEnt)
               {
                  set64bitpc();

                  if (cvar_PingCorrection->iValue == 1)
                  {
                     PingCorrection = LerpFloat(1.0f, ((cg->CurrentSnap->Ping + cg->NextSnap->Ping) / 2) / cvar_PingDivisor->Value, cvar_PingScalar->Value);
                  }

                  else
                  {
                     PingCorrection = 1.0f;
                  }

                  if (cvar_TimeCorrection->iValue == 1)
                  {
                     TimeCorrection = LerpFloat(1.0f, 1.0f + (((float)(cg->NextSnap->ServerTime - *PhysicsTime)) / ((float)(cg->NextSnap->ServerTime - cg->CurrentSnap->ServerTime))), cvar_TimeCorrectionScalar->Value);
                  }

                  else
                  {
                     TimeCorrection = 1.0f;
                  }

                  Vector3<_prec> vTargFinal, vTargNeck, accel;

                  LocalPrediction = (predictedPlayerState->vel) * (cvar_LocalPrediction->Value * PingCorrection);
                  ViewOrigin      = GetLocalViewpoint();
                  LocalPredicted  = ViewOrigin + LocalPrediction;
                  AimOrigin       = LocalPredicted;
				  
				 Vector3<double> velocity;
                  GetEntityVelocity(pTargEnt, &velocity);

				  DWORD        dwCG_CalcEntityLerpPositions = 0x00434FE0;
				  DWORD oldtime = *(DWORD*)0x794460;
				  *(DWORD*)0x794460 = (*(DWORD*)0xC628D0) + cvar_TimeAdditive->Value;
				  DWORD li = cg->clientNum;
				  __asm
				  {
					  push pTargEnt
					  push li
					  call [dwCG_CalcEntityLerpPositions]
					  add esp, 8
				  }
                  if (dwTargetTag == 0)                              // custom code for head positioning
                  {
                     vTargFinal = GetHeadPos(pTargEnt);
					 ApplyPrediction3(true, pTargEnt, &velocity);
                  }
                  else
                  {
                     vTargFinal = GetPlayerTagAccurateSafe(aimTags[dwTargetTag], pTargEnt);
#ifdef H47
                     Vector3<float> bpred(vic[pTargEnt->ClientNumber].bone[dwTargetTag].pred.vel.X + vic[pTargEnt->ClientNumber].bone[dwTargetTag].pred.accel.X, vic[pTargEnt->ClientNumber].bone[dwTargetTag].pred.vel.Y + vic[pTargEnt->ClientNumber].bone[dwTargetTag].pred.accel.Y, vic[pTargEnt->ClientNumber].bone[dwTargetTag].pred.vel.Z + vic[pTargEnt->ClientNumber].bone[dwTargetTag].pred.accel.Z);
                     bpred.X    *= cvar_AimPredictScalarX->Value;
                     bpred.Y    *= cvar_AimPredictScalarY->Value;
                     bpred.Z    *= cvar_AimPredictScalarZ->Value;
                     vTargFinal += bpred;
#endif               // H47
				  }
				  *(DWORD*)0x794460 = oldtime;

				  __asm
				  {
					  push pTargEnt
					  push li
					  call [dwCG_CalcEntityLerpPositions]
					  add esp, 8
				  }
				  
				  vTargFinal.X += velocity.X;
				  vTargFinal.Y += velocity.Y;
				  vTargFinal.Z += velocity.Z;

                  HeadBackup    = vTargFinal;
                  AbsBestAngles = (vTargFinal - AimOrigin).DAngles();
                  vTargFinal    = AbsBestAngles - refdef->ViewAngles;
                  vTargFinal.Z = 0.0f;
                  vTargFinal.Normalize180();
                  BestAngles = vTargFinal;
                  bShouldRotate = true;

                  Rotate(true);
                  GetAimAngles(&vTargFinal);
                  vTargFinal.Normalize180();
                  AddViewAngleDelta(vTargFinal);
                  set24bitpc();
               }
            }

#define DOPEINFO_H 200
		    //DrawWindow("D.O.P.E. Info", 10, refdef->iHeight - DOPEINFO_H, 400, DOPEINFO_H);

            if (cvar_AimTargetInfo->iValue == 1)
            {
               tiPower = BestPower;

               if(tiPower > 1.0f)
               {
                    tiPower = 1.0f;
               }

               if (dwTargetID < MAX_PLAYERS)
               {
                  CClientInfo *pTargCli = GetClientByIndex(dwTargetID);
#if 0
#ifdef X_VERSION
                  sprintf_s(szTargetinfo, DS("%s[%.1f"), pTargCli->Name, tiPower * 100.0f);
#else
                  sprintf_s(szTargetinfo, DS("%s[%i"), pTargCli->Name, (int)(tiPower * 100.0f));
#endif
                  DrawGlowTextf(SmallDevFont, ((float)((refdef->iWidth / 2) - 50)), (((float)refdef->iHeight) / 3.f), 1.2f, 1.2f, 0.0f, CColor<_color_prec>(tiPower, 0.1f, 0.1f, 1.0f), GLOW_MORE_SHADOWED, glow, "%s%%]", szTargetinfo);
#endif
               }

               dwTargetinfoTime = GetTickCount();
            }

            if (cvar_MovingXhair->iValue == 2)
            {
               Vector3<_prec> vHitPos;
               WorldToScreen(HeadBackup, &vHitPos);

               if (!(vHitPos.Z < 0.01f))
               {
                  DrawBox(vHitPos.X, vHitPos.Y - 10, 1, 20, CColor<_color_prec>(255, 255, 255, 255));
                  DrawBox(vHitPos.X - 10, vHitPos.Y, 20, 1, CColor<_color_prec>(255, 255, 255, 255));
               }
            }
         }
      }

      DWORD tcNow = GetTickCount();

      if ((cvar_AimTargetInfo->iValue == 1) && (tcNow != dwTargetinfoTime))      // it wasn't drawn this frame, so let's fade it
      {
         float tcFade = (float)(tcNow - dwTargetinfoTime);

         if (tcFade <= 500.0f)               // TODO: #define TARGETINFO_FADETIME
         {
            DrawGlowTextf(SmallDevFont, (float)(refdef->iWidth / 2 - 50), ((float)refdef->iHeight) / 3.f, 1.2f, 1.2f, 0.0f, CColor<_color_prec>(tiPower, 0.1f, 0.1f, (500.0f - tcFade) / 500.0f), GLOW_MORE_SHADOWED, glow, "%s%%]", szTargetinfo);
         }
      }

      if (cvar_Spin->iValue == 2)
      {
         _spinVec.X = 0.0f;
         _spinVec.Y = 0.0f;
         _spinVec.Z = cvar_SpinDegrees->Value;
         AddViewAngleDelta(_spinVec);
         bSpin = true;
      }
      else if (bSpin)
      {
         SetViewAngleZ(0.0f);
         bSpin = false;
      }

      for (int i = 0; i < 8; ++i)
      {
         CCompassEntity *compassEntity = &compassEntities[i];

         if (compassEntity->time < cg->CurrentSnap->ServerTime - 1500)
         {
            continue;
         }

         if ((pLocalClient->Team != TEAM_FFA) && ((pLocalClient->Team == compassEntity->team) && cvar_TeamFilter->Value))
         {
            continue;
         }

         bool isFriend = false;

         if (pLocalClient->Team)
         {
            isFriend = pLocalClient->Team == compassEntity->team;
         }

         else
         {
            isFriend = cg->clientNum == compassEntity->clientNum;
         }

         Vector3<_prec>      screenPos;
         CColor<_color_prec> color;
         CEntity             *cent = GetEntityByIndex(compassEntity->entityIndex);
         screenPos = cent->vOrigin;

         if (VisTrace(refdef->vOrigin, screenPos, compassEntity->entityIndex))
         {
            if (isFriend)
            {
               color = FriendlyVis;
            }

            else
            {
               color = EnemyVis;
            }
         }

         else
         {
            if (isFriend)
            {
               color = FriendlyHid;
            }

            else
            {
               color = EnemyHid;
            }
         }

         WorldToScreen(screenPos, &screenPos);

         if (!(screenPos.Z < 0.01f))
         {
            switch (cent->EntityType)
            {
            case TYPE_HELICOPTER:
               if (cvar_HelicopterESP->iValue == 1)
               {
                  DrawString(screenPos.X, screenPos.Y, EspFont, color, 1.0f, "[Helicopter]");
               }
			   // TODO: move helicopter/airstrike Radar code here.

               break;

            case TYPE_AIRSTRIKE:
               if (cvar_AirstrikeESP->iValue == 1)
               {
                  DrawString(screenPos.X, screenPos.Y, EspFont, color, 1.0f, "[Air-Strike]");
               }

               break;
            }
         }
      }

      if (bMenu)
      {
         DrawMenu();
         SetESPOffset();

         if (cvar_Save->iValue == 1)
         {
            cvar_Save->iValue = 0;
            Mngr.Save(ConfigPath);
         }
      }

#ifdef COMPILE_DEBUG
      if (cvar_Debug->iValue == 1)
      {
         CWeapon *pWeapon = GetWeaponByIndex(pLocalEnt->WeaponId);
         if(pWeapon)
         DrawStringf(100, 100, SmallDevFont, CColor<float>(255, 255, 255, 255), 1.0f, "aimbot ready = %d\ntarget = %i\nknifing = %d\ndemo = %i\nbrotate = %i\nbfire = %i\npower = %f\nthreat = %f\nspread x = %f\nspread y = %f\nswayMaxAngle = %f\nswayHorizScale = %f\nswayLerpSpeed = %f\nswayPitchScale = %f\nswayVertScale = %f\nswayYawScale = %f", bAimbotIsReady(), dwTargetID, bKnifing, cg->demoPlayback, bShouldRotate, bShouldFire, BestPower, HighestThreat, SpreadHit.X, SpreadHit.Y, pWeapon->swayMaxAngle, pWeapon->swayHorizScale, pWeapon->swayLerpSpeed, pWeapon->swayPitchScale, pWeapon->swayVertScale, pWeapon->swayYawScale);
      }
#endif // COMPILE_DEBUG

      if (cvar_InfoBar->iValue == 1)
      {
         DrawInfoBar();
      }

      if (cvar_Crosshair->iValue >= 1)
      {
         DrawCrosshair();
      }

      if (cvar_AutoVote->iValue == 1)
      {
         AutoVote();
      }

      if (cvar_NameStealer->iValue == 1)
      {
         DWORD tickCount = GetTickCount();

         if (lastNamestealTime + (cvar_NameStealInterval->Value * 1000) < tickCount)
         {
            StealName();
            lastNamestealTime = tickCount;
         }
      }

#ifndef X_VERSION
      if (cvar_KillStats->iValue == 1)
      {
         DrawKillStats();
      }
#endif
      switch (cvar_Clock->iValue)
      {
      case 1:
         DrawClock();
         break;

      case 2:
         DrawDigitalClock();
         break;
      }

      if ((cvar_AutoThrowback->iValue == 1) && pLocalEnt->Alive && pLocalEnt->IsValidEnt && (pLocalEnt->EntityType == TYPE_HUMAN))
      {
         if (twice_cg->snap->ps.pickupValue >= 5)
         {
            if (GetEntityByIndex(twice_cg->snap->ps.pickupEntityIndex)->EntityType == TYPE_EXPLOSIVE)
            {
               CG_ConsoleCommand(DS("+frag"));
               CG_ConsoleCommand(DS("-frag"));

               AddViewAngleDelta(Vector3<_prec>(-45.0f, 0.0f, 0.0f));
            }
         }
      }

#ifndef X_VERSION
      if (cvar_FlyCam->iValue == 1)
      {
         if (bFwd)
         {
            tmpAngles = refdef->ViewAngles;
            tmpAngles.Normalize360();
            tmpAngles.AngleVectors(&FlyCamDelta, 0, 0);
            FlyCamDelta = FlyCamDelta * cvar_FlyCamSpeed->Value;
            FlyCamPos   = FlyCamPos + FlyCamDelta;
         }

         else if (bBwd)
         {
            tmpAngles = refdef->ViewAngles;
            tmpAngles.Normalize360();
            tmpAngles.AngleVectors(&FlyCamDelta, 0, 0);
            FlyCamDelta = FlyCamDelta * cvar_FlyCamSpeed->Value;
            FlyCamPos   = FlyCamPos - FlyCamDelta;
         }

         if (bLft)
         {
            tmpAngles = refdef->ViewAngles;
            tmpAngles.Normalize360();
            tmpAngles.AngleVectors(0, &FlyCamDelta, 0);
            FlyCamDelta = FlyCamDelta * cvar_FlyCamSpeed->Value;
            FlyCamPos   = FlyCamPos - FlyCamDelta;
         }

         else if (bRgt)
         {
            tmpAngles = refdef->ViewAngles;
            tmpAngles.Normalize360();
            tmpAngles.AngleVectors(0, &FlyCamDelta, 0);
            FlyCamDelta = FlyCamDelta * cvar_FlyCamSpeed->Value;
            FlyCamPos   = FlyCamPos + FlyCamDelta;
         }
      }
#endif

      if (cvar_MovingXhair->iValue == 1)
      {
         Vector3<_prec> vHitPos;
         WorldToScreen(SpreadDisplacement + refdef->vOrigin, &vHitPos);

         if (!(vHitPos.Z < 0.01f))
         {
            DrawBox(vHitPos.X, vHitPos.Y - 10, 1, 20, CColor<_color_prec>(255, 255, 255, 255));
            DrawBox(vHitPos.X - 10, vHitPos.Y, 20, 1, CColor<_color_prec>(255, 255, 255, 255));
         }
      }

      if (cvar_Healthbar->iValue == 1)
      {
         DrawHealthbar();
      }

      if (cvar_Warnings->iValue == 1)
      {
         DrawWarnings();

#if 0
         if(dwSafetyTime >= 5000 && dwSafetyTime+5000 > GetTickCount())
         {
              DrawString((refdef->iWidth/2)-70,(refdef->iHeight/3)*2,SmallDevFont,CColor<_color_prec>(50,255,50,255),1.3f,"Clean PBSS Took");
         }
#endif
      }

      if (cvar_DVarUnlocker->iValue == 1)
      {
         *pDvarunlock |= 1;
      }
      else
      {
         if (*pDvarunlock & 1)
         {
            *pDvarunlock ^= 1;
         }
      }

#ifndef X_VERSION
      if (cvar_AntiDeath->iValue == 1)
      {
         AntiDeath();
      }
#endif

      if (cvar_WeaponLaser->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_cg_laserLight, 0);
         SAFE_CVAR_WRITE_DWORD(real_cg_laserForceOn, 1);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_cg_laserForceOn, 0);
      }

      if (cvar_NoFlash->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_cg_drawShellshock, 0);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_cg_drawShellshock, 1);
      }

      if ((cvar_NoLighting->iValue == 0) && FullBright_Switch)
      {
         __asm CALL dwDeactivateFullBright

            FullBright_Switch = false;
      }
      else if ((cvar_NoLighting->iValue == 1) && !FullBright_Switch)
      {
         __asm CALL dwActivateFullBright

            FullBright_Switch = true;
      }

      if (cvar_BulletLaser->iValue == 1)
      {
         SAFE_CVAR_WRITE_FLOAT(real_cg_firstPersonTracerChance, 1.0f);
         SAFE_CVAR_WRITE_FLOAT(real_cg_tracerchance, 1.0f);
         SAFE_CVAR_WRITE_FLOAT(real_cg_tracerScrewRadius, 0.0f);
         SAFE_CVAR_WRITE_FLOAT(real_cg_tracerSpeed, 400.0f);
      }

      else
      {
         SAFE_CVAR_WRITE_FLOAT(real_cg_tracerSpeed, 7500.0f);
      }

      if (cvar_FilmTweak->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_r_filmTweakEnable,1);
         SAFE_CVAR_WRITE_DWORD(real_r_filmUseTweaks,1);
         SAFE_CVAR_WRITE_FLOAT(real_r_filmTweakContrast,cvar_FilmContrast->Value);
         SAFE_CVAR_WRITE_FLOAT(real_r_filmTweakDesaturation,cvar_FilmDesaturation->Value);
         SAFE_CVAR_WRITE_FLOAT(real_r_filmTweakBrightness,cvar_FilmBrightness->Value);
         SAFE_CVAR_WRITE_DWORD(real_r_filmTweakInvert,cvar_FilmInvert->iValue);
         SAFE_CVAR_WRITE_DWORD(real_r_glowTweakEnable,cvar_GlowTweak->iValue);
         SAFE_CVAR_WRITE_DWORD(real_r_glowUseTweaks,cvar_GlowTweak->iValue);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_r_filmTweakEnable,0);
         SAFE_CVAR_WRITE_DWORD(real_r_filmUseTweaks,0);
         SAFE_CVAR_WRITE_DWORD(real_r_glowTweakEnable,0);
         SAFE_CVAR_WRITE_DWORD(real_r_glowUseTweaks,0);
      }

      if (cvar_EngineRadar->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_g_compassShowEnemies, 1);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_g_compassShowEnemies, 0);
      }

      if (cvar_NoFog->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_r_fog, 0);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_r_fog, 1);
      }

      if (cvar_NoEffects->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_fx_draw, 0);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_fx_draw, 1);
      }

      if (cvar_NoNVO->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_nightVisionDisableEffects, 1);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_nightVisionDisableEffects, 0);
      }

      if (cvar_NoSSO->iValue == 1)
      {
         SAFE_CVAR_WRITE_DWORD(real_cg_debug_overlay_viewport, 1);
      }

      else
      {
         SAFE_CVAR_WRITE_DWORD(real_cg_debug_overlay_viewport, 0);
      }

      if (cvar_NoBob->iValue == 1)
      {
         SAFE_CVAR_WRITE_FLOAT(real_bg_bobMax, 0.0f);
         SAFE_CVAR_WRITE_FLOAT(real_player_sprintCameraBob, 0.0f);
      }

      else
      {
         SAFE_CVAR_WRITE_FLOAT(real_bg_bobMax, 8.0f);
         SAFE_CVAR_WRITE_FLOAT(real_player_sprintCameraBob, 0.5f);
      }

      if (cvar_NoDamageFeedback->iValue == 1)
      {
         SAFE_CVAR_WRITE_FLOAT(real_bg_viewKickScale, 0.0f);
         SAFE_CVAR_WRITE_FLOAT(real_bg_viewKickMin, 0.0f);
      }

      else
      {
         SAFE_CVAR_WRITE_FLOAT(real_bg_viewKickScale, 0.2f);
         SAFE_CVAR_WRITE_FLOAT(real_bg_viewKickMin, 5.0f);
      }

      if (cvar_ForceLOD->iValue == 1)
      {
         SAFE_CVAR_WRITE_FLOAT(real_r_lodBiasRigid, -1000.0f);
         SAFE_CVAR_WRITE_FLOAT(real_r_lodBiasSkinned, -1000.0f);
      }

      else
      {
         SAFE_CVAR_WRITE_FLOAT(real_r_lodBiasRigid, 0.0f);
         SAFE_CVAR_WRITE_FLOAT(real_r_lodBiasSkinned, 0.0f);
      }

      //SAFE_CVAR_WRITE_DWORD(real_sv_disableclientconsole, 0);

      if (GetAsyncKeyState(VK_INSERT) & 1)
      {
         bMenu = !bMenu;
      }
   }

   else
   {
      DisableVisuals();
   }
}
