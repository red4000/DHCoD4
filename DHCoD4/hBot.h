#ifndef HBOT_H
#define HBOT_H

void Frame();
void InitializeGame();
void RegisterTags();

extern char ConfigPath[MAX_PATH];
class CEntity;
class CClientInfo;
class CWeapon;
extern CEntity     *pLocalEnt;
extern CClientInfo *pLocalClient;
extern CWeapon     *pLocalWeapon;

extern CVar
*cvar_botHeader,
*cvar_d3dHeader,
*cvar_visHeader,
*cvar_radarHeader,
*cvar_removalsHeader,
*cvar_miscHeader,
*cvar_pbHeader,

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
*cvar_AimAcceleration,
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
*cvar_NoShake,
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

DECLARE_COLCVAR(*cvar_Crosshair),
DECLARE_COLCVAR(*cvar_CrosshairShadow),
DECLARE_COLCVAR(*cvar_Wall),
DECLARE_COLCVAR(*cvar_EnemyHid),
DECLARE_COLCVAR(*cvar_FriendlyVis),
DECLARE_COLCVAR(*cvar_EnemyVis),
DECLARE_COLCVAR(*cvar_FriendlyHid),
DECLARE_COLCVAR(*cvar_WeaponVis),
DECLARE_COLCVAR(*cvar_WeaponHid)
;

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *pExceptionInfo);
float LerpFloat(float from, float to, float frac);
void DisableVisuals();
void RegisterCvar();

extern CCVar *real_fx_draw,
*real_fx_enable,
*real_cg_tracerwidth,
*real_dynEnt_active,
*real_sv_clientSideBullets,
*real_cl_maxpackets,
*real_cl_packetdup,
*real_snaps;

extern CPlayer *GPTR cg_player;
extern float *GPTR   CMouseSensivity;
extern int *GPTR     Zoom;

extern CColor<_color_prec> background, foreground, foreground2, foreground3, glow, text, selectedText;
extern char                szClass[256], HackPath[MAX_PATH];
extern bool                bIgnoreNextCmd;
extern CVarManager         Mngr;
extern DWORD               or_logfile;
extern DWORD               EFonts[4];
extern float               fDistance[MAX_GENTITIES]; // TODO: resize to MAX_PLAYERS, and don't cache gentity distance
extern Vector3<_prec>      vHead[MAX_PLAYERS];

#endif
