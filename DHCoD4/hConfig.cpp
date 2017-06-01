#pragma warning ( disable : 4530 )
#include <windows.h>
#include <d3d9.h>
#pragma hdrstop

#include "hConfig.h"
#include "hData.h"

char _tmpstring[256];

CVar
// Headers
*cvar_botHeader,
*cvar_bot2Header,
*cvar_bot3Header,
*cvar_d3dHeader,
*cvar_visHeader,
*cvar_vis2Header,
*cvar_radarHeader,
*cvar_removalsHeader,
*cvar_removals2Header,
*cvar_miscHeader,
*cvar_misc2Header,
*cvar_pbHeader,
*cvar_PostProcessingheader,

// Entries
*cvar_2DBoxes,
*cvar_3DBoxes,
*cvar_AimAcceleration,
*cvar_AimBoneQuantity,
*cvar_AimBoneStartIndex,
*cvar_AimDistanceOverride,
*cvar_AimFOV,
*cvar_AimKey,
*cvar_AimLerp,
*cvar_AimPredictScalarX,
*cvar_AimPredictScalarY,
*cvar_AimPredictScalarZ,
*cvar_AimPrediction,
*cvar_AimPrediction2,
*cvar_AimPrediction3,
*cvar_AimPrediction3Mode,
*cvar_AimMultiPoint,
*cvar_AimMultiPointScalar,
*cvar_AimRange,
*cvar_AimSort,
*cvar_AimSphereClip,
*cvar_AimSphereRadius,
*cvar_AimTargetInfo,
*cvar_AimTargetLock,
*cvar_AimVecZ,
*cvar_AimHeadScalar,
*cvar_AimHelmetScalar,
*cvar_AimNeckScalar,
*cvar_AimEyeScalar,
*cvar_AimViewLock,
*cvar_AimVisChecks,
*cvar_AirstrikeESP,
*cvar_AntiDeath,
*cvar_AntiAim,
*cvar_AutoFire,
*cvar_AutoFireFOV,
*cvar_AutoKnife,
*cvar_AutoKnifeMaxHostiles,
*cvar_AutoKnifeRange,
*cvar_AutoSight,
*cvar_AutoThrowback,
*cvar_AutoVote,
*cvar_AutoWall,
*cvar_AutoWallTolerance,
*cvar_AutowallUseAltSort,
*cvar_BarrelESP,
*cvar_BoneESP,
*cvar_BonerESP,
*cvar_BoxEspShadow,
*cvar_BoxEspThickness,
*cvar_BulletLaser,
*cvar_ChamAlpha,
*cvar_ChamAlphaDst,
*cvar_ChamAlphaSrc,
*cvar_ChamsUsePS1,
*cvar_Circle,
*cvar_CircleScale,
*cvar_CircleSize,
*cvar_Clock,
*cvar_ClockSize,
*cvar_ColoredWalls,
*cvar_Crosshair,
*cvar_CrosshairHeight,
*cvar_CrosshairShadow,
*cvar_CrosshairSize,
*cvar_CrosshairSpread,
*cvar_CrosshairWidth,
*cvar_DVarUnlocker,
*cvar_Debug,
*cvar_DefensiveTacticalAdvisories,
*cvar_Dodge,
*cvar_PlayerDistance,
*cvar_EngineRadar,
*cvar_EspFont,
*cvar_EspFontScale,
*cvar_EspShadow,
*cvar_ExplosiveTracing,
*cvar_Explosives,
*cvar_Film,
*cvar_FilmBrightness,
*cvar_FilmContrast,
*cvar_FilmDesaturation,
*cvar_FilmInvert,
*cvar_FilmTweak,
*cvar_FlyCam,
*cvar_FlyCamSpeed,
*cvar_ForceLOD,
*cvar_FullAuto,
*cvar_FullAutoFOV,
*cvar_GUID,
*cvar_GUIOutlineThickness,
*cvar_GhostCvars,
*cvar_GlowTweak,
*cvar_Healthbar,
*cvar_HelicopterESP,
*cvar_InfoBar,
*cvar_KillSounds,
*cvar_KillSpam,
*cvar_KillStats,
*cvar_LineESP,
*cvar_LocalPrediction,
*cvar_MassMurder,
*cvar_MassMurderValue,
*cvar_MassMurderValue2,
*cvar_MovingXhair,
*cvar_NameStealInterval,
*cvar_NameStealer,
*cvar_PlayerName,
*cvar_NoAimClimbing,
*cvar_NoAimExplosive,
*cvar_NoAimProne,
*cvar_NoBob,
*cvar_NoDamageFeedback,
*cvar_NoEffects,
*cvar_NoFlash,
*cvar_NoFog,
*cvar_NoHUDBlood,
*cvar_NoLighting,
*cvar_NoNVO,
*cvar_NoRecoil,
*cvar_NoSSO,
*cvar_NoSky,
*cvar_NoSpread,
*cvar_NoSpreadDebug1,
*cvar_NoSpreadDebug2,
*cvar_NoSpreadDebug3,
*cvar_NoSpreadDebug4,
*cvar_NoSpreadDebug5,
*cvar_NoSpreadDebug6,
*cvar_NoSpreadDebug7,
*cvar_NoSpreadDebug8,
*cvar_NoStun,
*cvar_NoSway,
*cvar_OffensiveTacticalAdvisories,
*cvar_OneHitKill,
*cvar_PBSSCleaner,
*cvar_PingCorrection,
*cvar_PingDivisor,
*cvar_PingScalar,
*cvar_PlayerAim,
*cvar_PlayerChams,
*cvar_PlayerOutlineChams,
*cvar_PlayerOutlineChamsWallhack,
*cvar_PlayerWireframe,
*cvar_Radar,
*cvar_RadarBackground,
*cvar_RadarScale,
*cvar_RadarSize,
*cvar_RadarPlayers,
*cvar_RadarExplosives,
*cvar_RadarWeapons,
*cvar_RadarHelicopters,
*cvar_RadarAirstrikes,
*cvar_RCONBlocker,
*cvar_RoundOutlines,
*cvar_Save,
*cvar_SilentAim,
*cvar_SmoothAim,
*cvar_SmoothAimAmplitude,
*cvar_SmoothAimDivisions,
*cvar_SmoothAimMethod,
*cvar_SnapLines,
*cvar_Spin,
*cvar_SpinDegrees,
*cvar_SpyCam,
*cvar_PyramidEsp,
*cvar_TeamFilter,
*cvar_TimeCorrection,
*cvar_TimeCorrectionScalar,
*cvar_TimeAdditive,
*cvar_UseAltHook,
*cvar_WallAlphaBlend,
*cvar_WallOutlineChams,
*cvar_WallOutlineChamsWallhack,
*cvar_WallWallhack,
*cvar_WallWireframe,
*cvar_Wallhack,
*cvar_Warnings,
*cvar_WeaponChams,
*cvar_WeaponEsp,
*cvar_WeaponLaser,
*cvar_WeaponOutlineChams,
*cvar_WeaponOutlineChamsWallhack,
*cvar_PlayerWeapon,
*cvar_WireframeEffects,
*cvar_XHair,
*cvar_ZombieAim,
*cvar_ZombieESP,
*cvar_pbBypass,
*cvar_pbssCleaner,

// Colors
DECLARE_COLCVAR(*cvar_Crosshair),
DECLARE_COLCVAR(*cvar_CrosshairShadow),
DECLARE_COLCVAR(*cvar_Wall),
DECLARE_COLCVAR(*cvar_EnemyHid),
DECLARE_COLCVAR(*cvar_FriendlyVis),
DECLARE_COLCVAR(*cvar_EnemyVis),
DECLARE_COLCVAR(*cvar_FriendlyHid),
DECLARE_COLCVAR(*cvar_WeaponVis),
DECLARE_COLCVAR(*cvar_WeaponHid),

*cvar_Dummy;
CVarManager Mngr;

void RegisterCvar() // TODO: rename // TODO: randomize this function after it has been used - MF(RegisterCvar)
{
   DecryptStrings();

#ifdef USE_EXTRA_VMP
   VMProtectBeginMutation(NULL);
#endif // USE_EXTRA_VMP
   cvar_botHeader = Mngr.AddHeader(Bots); // has 13 entries
#ifdef VISUALS
   cvar_AutoVote       = Mngr.AddVar(Auto_Vote, cvar_botHeader);
   cvar_AutoKnife      = Mngr.AddVar(Auto_Melee, cvar_botHeader);
   cvar_AutoKnifeRange = Mngr.AddVar(Auto_Melee_Range, cvar_botHeader, false, 2.9f, 1.0f, 10.0f, Plus, 0.1f);
#endif // VISUALS
   cvar_PlayerAim         = Mngr.AddVar(Player_Auto_Aim, cvar_botHeader);
   cvar_AimRange          = Mngr.AddVar(Auto_Aim_Max_Range, cvar_botHeader, false, 0.0f, 0.0f, 3000.0f, Plus, 1.0f);
   cvar_AimBoneQuantity   = Mngr.AddVar(Aim_Bone_Quantity, cvar_botHeader, true, 1.0f, 1.0f, 17.0f, Plus, 1.0f);
   cvar_AimBoneStartIndex = Mngr.AddVar(Aim_Bone_Start_Index, cvar_botHeader, true, 0.0f, 0.0f, 16.0f, Plus, 1.0f);
   cvar_AimViewLock       = Mngr.AddVar(Aim_View_Lock, cvar_botHeader);
   cvar_AimTargetLock     = Mngr.AddVar(Aim_Target_Lock, cvar_botHeader);
   cvar_AutoFire          = Mngr.AddVar(Auto_Fire, cvar_botHeader);
   cvar_AutoFireFOV       = Mngr.AddVar(Auto_Fire_FOV, cvar_botHeader, false, 0.0f, 0.0f, 360.0f, Plus, 0.5f);
   cvar_AutoSight         = Mngr.AddVar(Auto_Sight, cvar_botHeader); // TODO: Auto Sight 2 for INSTANT autosight
   cvar_AimFOV            = Mngr.AddVar(Aim_FOV, cvar_botHeader, false, 0.0f, 0.0f, 360.0f, Plus, 0.5f);
   cvar_AimKey            = Mngr.AddVar("Aim Key", cvar_botHeader, true, 0.0f, 0.0f, 360.0f, Plus, 1.0f);

   cvar_bot2Header = Mngr.AddHeader(Bots_continued); // has 12 entries
#ifdef VISUALS
   cvar_SmoothAim          = Mngr.AddVar(Smooth_Aim, cvar_bot2Header);
   cvar_SmoothAimDivisions = Mngr.AddVar(Smooth_Aim_Divisions, cvar_bot2Header, false, 15.0f, 1.0f, 360.0f, Plus, 1.0f);
   cvar_SmoothAimAmplitude = Mngr.AddVar(Smooth_Aim_Amplitude, cvar_bot2Header, false, 5.0f, 0.1f, 360.0f, Plus, 0.1f);
   cvar_SmoothAimMethod    = Mngr.AddVar(Smooth_Aim_Method, cvar_bot2Header, true, 1.0f, 1.0f, 5.0f, Plus, 1.0f);
#endif // VISUALS
   cvar_AimPrediction     = Mngr.AddVar(Aim_Prediction, cvar_bot2Header, false, 0.0f, 0.0f, 10.0f, Plus, 0.01f);
   cvar_AimPrediction2    = Mngr.AddVar("Aim Prediction 2", cvar_bot2Header, false, 0.65f, 0.0f, 10.0f, Plus, 0.01f);
   cvar_AimPrediction3    = Mngr.AddVar("Aim Prediction 3", cvar_bot2Header, false, 0.0f, 0.0f, 10.0f, Plus, 0.01f);
   cvar_AimPrediction3Mode= Mngr.AddVar("Aim Prediction 3 Mode", cvar_bot2Header, true, 0.0f, 0.0f, 1.0f, Plus, 1.0f);
   cvar_AimAcceleration   = Mngr.AddVar("Acceleration Scalar", cvar_bot2Header, false, 0.0f, 0.0f, 10.0f, Plus, 0.01f);
   cvar_AimMultiPoint     = Mngr.AddVar("Points", cvar_bot2Header, true, 1.0f, 1.0f, 100.0f, Plus, 1.0f);
   cvar_AimMultiPointScalar = Mngr.AddVar("Point Scalar", cvar_bot2Header, false, 0.025f, 0.0f, 10.0f, Plus, 0.01f);
   cvar_LocalPrediction   = Mngr.AddVar(Local_Prediction, cvar_bot2Header, false, 0.0f, 0.0f, 10.0f, Plus, 0.01f);
   cvar_PingCorrection    = Mngr.AddVar(Ping_Correction, cvar_bot2Header);
   cvar_PingDivisor       = Mngr.AddVar(Ping_Divisor, cvar_bot2Header, false, 150.0f, 1.0f, 999.0f, Plus, 1.0f);
   cvar_PingScalar        = Mngr.AddVar(Ping_Scalar, cvar_bot2Header, false, 0.5f, 0.01f, 10.0f, Plus, 0.01f);
   cvar_AutoWall          = Mngr.AddVar(Auto_Wall, cvar_bot2Header, true, 1.0f, 0.0f, 2.0f, Plus, 1.0f, "Off", "Normal", "Rapid");
   cvar_AutoWallTolerance = Mngr.AddVar(Auto_Wall_Tolerance, cvar_bot2Header, false, 0.95f, .01f, 1.0f, Plus, .05f);
   cvar_SilentAim         = Mngr.AddVar(Silent_Aim, cvar_bot2Header);

   cvar_bot3Header           = Mngr.AddHeader(Bots_continued_2); // has 10 entries
   cvar_AimSort              = Mngr.AddVar(Aim_Sorting_Method, cvar_bot3Header, true, 4.0f, 1.0f, 4.0f, Plus, 1.0f, Default, Threat, Distance, Distance_from_crosshair, Best_Aimed); // TODO: encrypt these strings.
   cvar_AimSphereClip        = Mngr.AddVar(Aim_Position_Clip, cvar_bot3Header);
   cvar_AimSphereRadius      = Mngr.AddVar(Aim_Position_Clip_Radius, cvar_bot3Header, false, 11.0f, 1.0f, 200.0f, Plus, 1.0f);
   cvar_AimVecZ              = Mngr.AddVar(Aim_Pos_Z_adjust, cvar_bot3Header, false, 1.8f, -1000.0f, 1000.0f, Plus, 0.1f);
   cvar_AimHeadScalar        = Mngr.AddVar("Head Scalar", cvar_bot3Header, false, 1.0f, 0.0f, 1.0f, Plus, 0.01f);
   cvar_AimHelmetScalar      = Mngr.AddVar("Helmet Scalar", cvar_bot3Header, false, 0.00f, 0.0f, 1.0f, Plus, 0.01f);
   cvar_AimNeckScalar        = Mngr.AddVar("Neck Scalar", cvar_bot3Header, false, 0.00f, 0.0f, 1.0f, Plus, 0.01f);
   cvar_AimEyeScalar         = Mngr.AddVar("Eye Scalar", cvar_bot3Header, false, 0.00f, 0.0f, 1.0f, Plus, 0.01f);
   cvar_TimeCorrection       = Mngr.AddVar(Time_Correction, cvar_bot3Header, true, 0.0f, 0.0f, 1.0f, Plus);
   cvar_TimeCorrectionScalar = Mngr.AddVar(Time_Correction_Scalar, cvar_bot3Header, false, 0.3f, 0.01f, 10.0f, Plus, 0.01f);
   cvar_TimeAdditive         = Mngr.AddVar("Time Additive", cvar_bot3Header, true, 0.0f, -100.0f, 100.0f, Plus);
   cvar_AimPredictScalarX    = Mngr.AddVar(Prediction_Scalar_X, cvar_bot3Header, false, 1.0f, 0.01f, 10.0f, Plus, 0.01f);
   cvar_AimPredictScalarY    = Mngr.AddVar(Prediction_Scalar_Y, cvar_bot3Header, false, 1.0f, 0.01f, 10.0f, Plus, 0.01f);
   cvar_AimPredictScalarZ    = Mngr.AddVar(Prediction_Scalar_Z, cvar_bot3Header, false, 0.8f, 0.01f, 10.0f, Plus, 0.01f);
   cvar_AimDistanceOverride  = Mngr.AddVar(Aim_Distance_Override, cvar_bot3Header, false, 10.0f, 0.0f, 100.0f, Plus, 1.0f);
#ifdef VISUALS
   cvar_AimTargetInfo = Mngr.AddVar(Draw_Target_Info, cvar_bot3Header);

   cvar_visHeader     = Mngr.AddHeader(Visuals);
   cvar_BulletLaser   = Mngr.AddVar(Bullet_Laser, cvar_visHeader);
   cvar_WeaponLaser   = Mngr.AddVar(Weapon_Laser, cvar_visHeader);
   cvar_BoneESP       = Mngr.AddVar(Bone_ESP, cvar_visHeader, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, Line, Box);
   cvar_BonerESP      = Mngr.AddVar(Boner_ESP, cvar_visHeader);
   cvar_BarrelESP     = Mngr.AddVar(Barrel_ESP, cvar_visHeader);
   cvar_LineESP       = Mngr.AddVar(Line_ESP, cvar_visHeader);
   cvar_HelicopterESP = Mngr.AddVar(Helicopter_ESP, cvar_visHeader);
   cvar_AirstrikeESP  = Mngr.AddVar(Airstrike_ESP, cvar_visHeader);
   cvar_SnapLines     = Mngr.AddVar(Snap_Lines, cvar_visHeader);

   cvar_vis2Header = Mngr.AddHeader(Visuals_continued);
   cvar_PlayerName     = Mngr.AddVar(Name_ESP, cvar_vis2Header, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, Custom, Engine);
   cvar_Explosives     = Mngr.AddVar(Explosive_ESP, cvar_vis2Header, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, Icon, Text);
   cvar_PlayerDistance = Mngr.AddVar(Distance_ESP, cvar_vis2Header);
   cvar_PlayerWeapon   = Mngr.AddVar(Weapon_ESP, cvar_vis2Header, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, Text, Icon);
   cvar_WeaponEsp      = Mngr.AddVar(Pickup_ESP, cvar_vis2Header);
   cvar_2DBoxes        = Mngr.AddVar(Bounding_Boxes, cvar_vis2Header, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, _2D, _3D);
   cvar_PyramidEsp     = Mngr.AddVar(Triangle_ESP, cvar_vis2Header);
   cvar_TeamFilter     = Mngr.AddVar(Team_Filter, cvar_vis2Header);
   cvar_EspFont        = Mngr.AddVar(ESP_Font, cvar_vis2Header, true, 0.0f, 0.0f, 3.0f, Plus, 1.0f);
   cvar_EspFontScale   = Mngr.AddVar(ESP_Font_Scale, cvar_vis2Header, false, 1.0f, 0.6f, 3.0f, Plus, 0.1f);
   cvar_EspShadow      = Mngr.AddVar(ESP_Shadow, cvar_vis2Header, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f);

   cvar_radarHeader = Mngr.AddHeader(Radar);
   cvar_Radar       = Mngr.AddVar(_2D_Radar, cvar_radarHeader);
   //cvar_RadarBackground  = Mngr.AddVar(Radar_Background, cvar_radarHeader, true, 3.0f, 0.0f, 3.0f, Plus);
   cvar_RadarPlayers     = Mngr.AddVar(Players, cvar_radarHeader, true);
   cvar_RadarExplosives  = Mngr.AddVar(Explosives, cvar_radarHeader);
   cvar_RadarWeapons     = Mngr.AddVar(Pickups, cvar_radarHeader);
   cvar_RadarHelicopters = Mngr.AddVar(Helicopters, cvar_radarHeader);
   cvar_RadarAirstrikes  = Mngr.AddVar(Airstrikes, cvar_radarHeader);
   cvar_EngineRadar      = Mngr.AddVar(Engine_Radar, cvar_radarHeader);
   cvar_RadarSize        = Mngr.AddVar(_2D_Radar_Size, cvar_radarHeader, false, 150.0f, 1.0f, 2000.0f, Plus, 1);
   cvar_RadarScale       = Mngr.AddVar(_2D_Radar_Scale, cvar_radarHeader, false, 5.0f, 1.0f, 100.0f, Plus, 0.5f);
   cvar_Circle           = Mngr.AddVar(Circle_Radar, cvar_radarHeader);
   cvar_CircleSize       = Mngr.AddVar(Circle_Size, cvar_radarHeader, false, 200.0f, 50.0f, 2000.0f, Plus, 10.0f);
#endif // VISUALS

#ifdef CHAMS
   cvar_d3dHeader        = Mngr.AddHeader(D3D);
   cvar_PlayerChams      = Mngr.AddVar(Player_Chams, cvar_d3dHeader, true, 0.0f, 0.0f, 5.0f, Plus, 1.0f);
   cvar_WeaponChams      = Mngr.AddVar(Weapon_Chams, cvar_d3dHeader, true, 0.0f, 0.0f, 5.0f, Plus, 1.0f);
   cvar_ChamAlpha        = Mngr.AddVar(Alpha_Blending, cvar_d3dHeader);
   cvar_Wallhack         = Mngr.AddVar(Wallhack, cvar_d3dHeader, true);
   cvar_PlayerWireframe  = Mngr.AddVar(Wireframe, cvar_d3dHeader, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, Dot, Wire);
   cvar_WireframeEffects = Mngr.AddVar(Wireframe_Effects, cvar_d3dHeader);
   cvar_ColoredWalls     = Mngr.AddVar(Wall_Chams, cvar_d3dHeader, true, 0.0f, 0.0f, 5.0f, Plus, 1.0f);
   cvar_WallWireframe    = Mngr.AddVar(Wall_Wireframe, cvar_d3dHeader, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, Dot, Wire);
   cvar_WallWallhack     = Mngr.AddVar(Wall_Wallhack, cvar_d3dHeader);
   cvar_WallAlphaBlend   = Mngr.AddVar(Wall_Alpha_Blending, cvar_d3dHeader);
#endif // CHAMS
   cvar_removalsHeader = Mngr.AddHeader(Removals);
   cvar_NoSpread       = Mngr.AddVar(No_Spread, cvar_removalsHeader, true, 1.0f, 0.0f, 2.0f, Plus, 1.0f);
   cvar_NoSway         = Mngr.AddVar(No_Sway, cvar_removalsHeader);
#ifdef VISUALS
   cvar_NoRecoil   = Mngr.AddVar(No_Recoil, cvar_removalsHeader, true, 0.0f, 0.0f, 101.0f, Plus, 1.0f);
   cvar_NoFlash    = Mngr.AddVar(No_Flash, cvar_removalsHeader);
   cvar_NoEffects  = Mngr.AddVar(No_Effects, cvar_removalsHeader);
   cvar_NoFog      = Mngr.AddVar(No_Fog, cvar_removalsHeader);
   cvar_NoLighting = Mngr.AddVar(No_Lighting, cvar_removalsHeader);
   cvar_NoHUDBlood = Mngr.AddVar(No_HUD_Blood, cvar_removalsHeader);

   cvar_removals2Header  = Mngr.AddHeader(Removals_continued);
   cvar_NoNVO            = Mngr.AddVar(No_Night_Vision_Overlay, cvar_removals2Header);
   cvar_NoSSO            = Mngr.AddVar(No_Sniper_Scope_Overlay, cvar_removals2Header);
   cvar_NoBob            = Mngr.AddVar(No_Bob, cvar_removals2Header);
   cvar_NoDamageFeedback = Mngr.AddVar(No_Damage_Feedback, cvar_removals2Header);
#ifndef X_VERSION
   cvar_NoSky = Mngr.AddVar(No_Sky, cvar_removals2Header);
#endif // X_VERSION

   cvar_PostProcessingheader = Mngr.AddHeader(Post_Processing);
   cvar_FilmTweak = Mngr.AddVar(Post_Processing_Enable, cvar_PostProcessingheader);
   cvar_FilmBrightness = Mngr.AddVar(Brightness, cvar_PostProcessingheader, false, 0.16f, -1, 1, Plus, 0.01f);
   cvar_FilmContrast = Mngr.AddVar(Contrast, cvar_PostProcessingheader, false, 1.2f, 0, 4, Plus, 0.05f);
   cvar_FilmDesaturation = Mngr.AddVar(Desaturation, cvar_PostProcessingheader, false, 0.2f, 0, 1, Plus, 0.01f);
   cvar_FilmInvert = Mngr.AddVar(Invert, cvar_PostProcessingheader);
   cvar_GlowTweak = Mngr.AddVar(Bloom, cvar_PostProcessingheader, true, 1.0f);
#endif // VISUALS
   cvar_miscHeader = Mngr.AddHeader(Misc);
   cvar_FullAuto   = Mngr.AddVar(Full_Auto, cvar_miscHeader);
   cvar_FullAutoFOV= Mngr.AddVar("Full Auto only in FOV", cvar_miscHeader);
#ifdef VISUALS
   cvar_Crosshair         = Mngr.AddVar(Crosshair, cvar_miscHeader, true, 0.0f, 0.0f, 4.0f, Plus, 1.0f);
   cvar_CrosshairWidth    = Mngr.AddVar(Crosshair_Width, cvar_miscHeader, false, 11.0f, 0.0f, 1000.0f, Plus, 1.0f);
   cvar_CrosshairHeight   = Mngr.AddVar(Crosshair_Height, cvar_miscHeader, false, 2.0f, 0.0f, 1000.0f, Plus, 1.0f);
   cvar_CrosshairSpread   = Mngr.AddVar(Crosshair_Spread, cvar_miscHeader, false, 16.0f, 0.0f, 1000.0f, Plus, 1.0f);
   cvar_CrosshairShadow   = Mngr.AddVar(Crosshair_Shadow, cvar_miscHeader, true, 2.0f, 0.0f, 2.0f, Plus, 1.0f);
   cvar_NameStealer       = Mngr.AddVar(Name_Stealer, cvar_miscHeader);
   cvar_NameStealInterval = Mngr.AddVar(Name_Steal_Interval, cvar_miscHeader, false, 7.0f, 1.0f, 30.0f, Plus, 1.0f);
   cvar_Clock             = Mngr.AddVar(Clock, cvar_miscHeader, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f);
   cvar_ClockSize         = Mngr.AddVar(Clock_Size, cvar_miscHeader, false, 100.0f, 50.0f, 1000.0f, Plus, 1.0f);
   cvar_Healthbar         = Mngr.AddVar(Health_Bar, cvar_miscHeader);

   cvar_misc2Header   = Mngr.AddHeader(Misc_continued);
   cvar_AutoThrowback = Mngr.AddVar(Auto_Grenade_Throwback, cvar_misc2Header);
   cvar_ForceLOD      = Mngr.AddVar(Force_LOD, cvar_misc2Header);
   cvar_Spin          = Mngr.AddVar(Spin, cvar_misc2Header, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f);
   cvar_SpinDegrees   = Mngr.AddVar(Spin_Degrees, cvar_misc2Header, false, 5.0f, -180.0f, 180.0f, Plus, 1.0f);
   cvar_AntiAim       = Mngr.AddVar("Anti Aim", cvar_misc2Header, true, 0.0f, 0.0f, 3.0f, Plus, 1.0f);
   //cvar_RCONBlocker   = Mngr.AddVar("RCON Blocker", cvar_miscHeader, true);
#ifndef X_VERSION
   cvar_AntiDeath = Mngr.AddVar(Anti_Death, cvar_misc2Header);
   cvar_KillStats = Mngr.AddVar(Kill_Stats, cvar_misc2Header);
   cvar_KillSpam  = Mngr.AddVar(Kill_Spam, cvar_misc2Header);
   cvar_KillSounds  = Mngr.AddVar(Kill_Sounds, cvar_misc2Header);
   cvar_FlyCam      = Mngr.AddVar(Fly_Cam, cvar_misc2Header);
   cvar_FlyCamSpeed = Mngr.AddVar(Fly_Cam_Speed, cvar_misc2Header, false, 3.0f, 0.1f, 20.0f, Plus, 0.5f);
#endif // X_VERSION
   cvar_Dodge        = Mngr.AddVar(Dodge, cvar_misc2Header, true, 0.0f, 0.0f, 2.0f, Plus, 1.0f, Off, Crouch, Prone);
   cvar_DVarUnlocker = Mngr.AddVar(DVar_Unlocker, cvar_misc2Header);
   cvar_Save = Mngr.AddVar(Save_Configuration, cvar_misc2Header);
#endif // VISUALS

   // --------------------------- secret / bonus cvars -------------------------------------
   cvar_Warnings          = Mngr.AddVar(Warnings, cvar_miscHeader, true, 0.f, 0.f, 1.f, Completely_Invisible, 1.f);
   cvar_AimAcceleration   = Mngr.AddVar(Aim_Acceleration, cvar_bot2Header, false, 0.0f, 0.0f, 10.0f, Incognito, 0.01f);
   cvar_UseAltHook   = Mngr.AddVar(Use_different_hook, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_AimVisChecks = Mngr.AddVar(Aim_Visibility_Checks, cvar_misc2Header, true, 1.0f, 0.0f, 1.0f, Incognito, 1.0f);
#ifdef VISUALS
   cvar_MovingXhair = Mngr.AddVar(Moving_XHair, cvar_misc2Header, true, 0.0f, 0.0f, 2.0f, Completely_Invisible, 1.0f);
#endif // VISUALS
   cvar_AutowallUseAltSort = Mngr.AddVar(Autowall_Alt_Sort, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
#ifdef COMPILE_DEBUG
   cvar_Debug = Mngr.AddVar(Debug, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
#endif // COMPILE_DEBUG
   cvar_NoSpreadDebug1  = Mngr.AddVar(No_Spread_Debug_1, cvar_misc2Header, true, 1.0f, 0.0f, 1.0f, Incognito, 1.0f);
   cvar_NoSpreadDebug2  = Mngr.AddVar(No_Spread_Debug_2, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_NoSpreadDebug3  = Mngr.AddVar(No_Spread_Debug_3, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_NoSpreadDebug4  = Mngr.AddVar(No_Spread_Debug_4, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_NoSpreadDebug5  = Mngr.AddVar(No_Spread_Debug_5, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_NoSpreadDebug6  = Mngr.AddVar(No_Spread_Debug_6, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_NoSpreadDebug7  = Mngr.AddVar(No_Spread_Debug_7, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_NoSpreadDebug8  = Mngr.AddVar(No_Spread_Debug_8, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
#ifdef MASS_MURDER
   cvar_MassMurder      = Mngr.AddVar(Mass_Murder, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Completely_Invisible, 1.0f);
   cvar_MassMurderValue = Mngr.AddVar(Mass_Murder_Value, cvar_misc2Header, false, 4294967296.0f, 0.0f, 4294967296.0f, Incognito, 1.0f);
#endif
   cvar_InfoBar           = Mngr.AddVar(Info_Bar, cvar_misc2Header, true, 1.f, 1.f, 1.f, Incognito, 0.f);
   // --------------------------- end secret / bonus cvars ---------------------------------

   // --------------------------- less used cvars - not visible in menu --------------------
#ifdef VISUALS
   cvar_AutoKnifeMaxHostiles = Mngr.AddVar(Auto_Melee_Max_Threats, cvar_misc2Header, true, 1.0f, 0.0f, 63.0f, Invisible, 1.0f);
   cvar_BoxEspThickness      = Mngr.AddVar(Bounding_Box_Thickness, cvar_misc2Header, false, 1.0f, 0.0f, 10.0f, Invisible, 1.0f);
   cvar_BoxEspShadow         = Mngr.AddVar(Bounding_Box_Shadow, cvar_misc2Header, true, 0.0f, 0.0f, 2.0f, Invisible, 1.0f);
   cvar_GUIOutlineThickness  = Mngr.AddVar(GUI_Outline_Thickness, cvar_misc2Header, false, 1.0f, 0.0f, 100.0f, Invisible, 1.0f);
   cvar_RoundOutlines        = Mngr.AddVar(Round_Outlines, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Invisible, 1.0f);
#endif // VISUALS
   cvar_NoAimExplosive = Mngr.AddVar(Dont_aim_with_explosives, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Invisible);
   cvar_NoAimProne     = Mngr.AddVar(Dont_aim_when_prone, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Invisible);
   cvar_NoAimClimbing  = Mngr.AddVar(Dont_aim_when_climbing, cvar_misc2Header, true, 0.0f, 0.0f, 1.0f, Invisible);
#ifdef CHAMS
   cvar_ChamAlphaSrc = Mngr.AddVar(Alpha_Source, cvar_misc2Header, true, D3DBLEND_SRCALPHA, 0.0f, D3DBLEND_BOTHINVSRCALPHA, Invisible, 1.0f);
   cvar_ChamAlphaDst = Mngr.AddVar(Alpha_Destination, cvar_misc2Header, true, D3DBLEND_DESTALPHA, 0.0f, D3DBLEND_BOTHINVSRCALPHA, Invisible, 1.0f);
#endif // CHAMS
   cvar_GhostCvars = Mngr.AddVar(Ghost_CVars, cvar_misc2Header, true, 1.0f, 0.0f, 1.0f, Invisible, 1.0f);
#ifdef PBSS_CLEANER
   cvar_PBSSCleaner = Mngr.AddVar(PBSS_Cleaner, cvar_misc2Header, true, 1.0f, 0.0f, 1.0f, Invisible, 1.0f);
#endif // PBSS_CLEANER
   // --------------------------- less used cvars - not visible in menu end ----------------

   // --------------------------- color cvars ----------------------------------------------
#ifdef CHAMS
   CREATE_COLCVAR(cvar_Wall, Wall, 70.0f, 10.0f, 10.0f, 255.0f);
   CREATE_COLCVAR(cvar_WeaponVis, Weapon_Visible, 255.0f, 50.0f, 255.0f, 255.0f);
   CREATE_COLCVAR(cvar_WeaponHid, Weapon_Hidden, 255.0f, 0.0f, 255.0f, 255.0f);
   CREATE_COLCVAR(cvar_EnemyHid, Chams_Enemy_Hidden, 50.0f, 50.0f, 255.0f, 255.0f);
   CREATE_COLCVAR(cvar_FriendlyVis, Chams_Friendly_Visible, 0.0f, 150.0f, 0.0f, 255.0f);
   CREATE_COLCVAR(cvar_EnemyVis, Chams_Enemy_Visible, 255.0f, 70.0f, 70.0f, 255.0f);
   CREATE_COLCVAR(cvar_FriendlyHid, Chams_Friendly_Hidden, 0.0f, 50.0f, 0.0f, 150.0f);
#endif // CHAMS
#ifdef VISUALS
   CREATE_COLCVAR(cvar_Crosshair, Crosshair, 15.0f, 255.0f, 155.0f, 255.f);
   CREATE_COLCVAR(cvar_CrosshairShadow, Crosshair_Shadow, 30.f, 30.f, 30.f, 255.f);
#endif // VISUALS
   // --------------------------- color cvars end -------------------------------------------
   memset(_tmpstring, 0, sizeof(_tmpstring));
#ifdef USE_EXTRA_VMP
   VMProtectEnd();
#endif // USE_EXTRA_VMP
}
