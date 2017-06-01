#ifndef HNEWCORE_H
#define HNEWCORE_H


#include <windows.h>
#include "hMath.h"

#pragma pack(push, 1)

class cg_s;
class snapshot_s;
class playerState_s;
class entityState_s;
class clientState_s;
class SprintState_t;
class MantleState;
class objective_t;
class hudelem_t;
class LerpEntityState;
class trajectory_t;
class centity_s;
class playerEntity_t;
class refdef_s;
class GfxLight;
class clientActive_t;
class clSnapshot_t;

class SprintState_t
{
public:
	__int32 sprintButtonUpRequired; //0x0000 
	__int32 sprintDelay; //0x0004 
	__int32 lastSprintStart; //0x0008 
	__int32 lastSprintEnd; //0x000C 
	__int32 sprintStartMaxLength; //0x0010 
};//Size=0x0014

class MantleState
{
public:
	float yaw; //0x0000 
	__int32 timer; //0x0004 
	__int32 transIndex; //0x0008 
	__int32 flags; //0x000C 
};//Size=0x0010

class objective_t
{
public:
	__int32 state; //0x0000 
	Vector3<_prec> origin; //0x0004 
	__int32 icon; //0x0010 
	__int32 teamNum; //0x0014 
	__int32 N0051B8E5; //0x0018 
};//Size=0x001C

class hudelem_t
{
public:
char _0x0000[12];
	float N0051B8E6; //0x000C 
	float N0051B8E7; //0x0010 
	float N0051B8E8; //0x0014 
	float N0051B8E9; //0x0018 
	__int32 N0051B8EA; //0x001C 
	__int32 N0051B8EB; //0x0020 
	__int32 N0051B8EC; //0x0024 
	DWORD color; //0x0028 
char _0x002C[148];
	float N0084EC80; //0x00C0 
	DWORD glowColor; //0x00C4 
char _0x00C8[120];
};//Size=0x0140

class playerState_s
{
public:
	__int32 commandTime; //0x0000 
	__int32 pm_type; //0x0004 
	__int32 bobCycle; //0x0008 
	__int32 pm_flags; //0x000C 
	__int32 weapFlags; //0x0010 
	__int32 otherFlags; //0x0014 
	__int32 pm_time; //0x0018 
	Vector3<_prec> origin; //0x001C 
	Vector3<_prec> velocity; //0x0028 
	float oldVelocity[2]; //0x0034 
	__int32 weaponTime; //0x003C 
	__int32 weaponDelay; //0x0040 
	__int32 grenadeTimeLeft; //0x0044 
	__int32 throwBackGrenadeOwner; //0x0048 
	__int32 throwBackGrenadeTimeLeft; //0x004C 
	__int32 weaponRestrictKickTime; //0x0050 
	__int32 foliageSoundTime; //0x0054 
	__int32 gravity; //0x0058 
	float leanf; //0x005C 
	__int32 speed; //0x0060 
	Vector3<_prec> delta_angles; //0x0064 
	__int32 groundEntityNum; //0x0070 
	Vector3<_prec> vLadderVec; //0x0074 
	__int32 jumpTime; //0x0080 
	float jumpOriginZ; //0x0084 
	__int32 legsTimer; //0x0088 
	__int32 legsAnim; //0x008C 
	__int32 torsoTimer; //0x0090 
	__int32 torsoAnim; //0x0094 
	__int32 legsAnimDuration; //0x0098 
	__int32 torsoAnimDuration; //0x009C 
	__int32 damageTimer; //0x00A0 
	__int32 damageDuration; //0x00A4 
	__int32 flinchYawAnim; //0x00A8 
	__int32 movementDir; //0x00AC  -180..180 angle
	__int32 eFlags; //0x00B0 
	__int32 eventSequence; //0x00B4 
	int events[4]; //0x00B8 
	unsigned int eventParms[4]; //0x00C8 
	__int32 oldEventSequence; //0x00D8 
	__int32 clientNum; //0x00DC 
	__int32 offHandIndex; //0x00E0 
	__int32 offhandSecondary; //0x00E4 
	DWORD weapon; //0x00E8 
	__int32 weaponstate; //0x00EC 
	DWORD weaponShotCount; //0x00F0 
	float fWeaponPosFrac; //0x00F4  aka zoom fraction
	__int32 adsDelayTime; //0x00F8 
	__int32 spreadOverride; //0x00FC 
	__int32 spreadOverrideState; //0x0100 
	__int32 viewmodelIndex; //0x0104 
	Vector3<_prec> viewangles; //0x0108 
	__int32 viewHeightTarget; //0x0114 
	float viewHeightCurrent; //0x0118 
	__int32 viewHeightLerpTime; //0x011C 
	__int32 viewHeightLerpTarget; //0x0120 
	__int32 viewHeightLerpDown; //0x0124 
	float viewAngleClampBase[2]; //0x0128 
	float viewAngleClampRange[2]; //0x0130 
	__int32 damageEvent; //0x0138 
	__int32 damageYaw; //0x013C 
	__int32 damagePitch; //0x0140 
	__int32 damageCount; //0x0144 
	int stats[5]; //0x0148 
	int ammo[128]; //0x015C 
	int ammoclip[128]; //0x035C 
	unsigned int weapons[4]; //0x055C 
	unsigned int weaponold[4]; //0x056C 
	unsigned int weaponrechamber[4]; //0x057C 
	float proneDirection; //0x058C 
	float proneDirectionPitch; //0x0590 
	float proneTorsoPitch; //0x0594 
	__int32 viewlocked; //0x0598 
	__int32 viewlocked_entNum; //0x059C 
	__int32 cursorHint; //0x05A0 
	__int32 cursorHintString; //0x05A4 
	__int32 cursorHintEntIndex; //0x05A8 
	__int32 iCompassPlayerInfo; //0x05AC 
	__int32 radarEnabled; //0x05B0 
	__int32 locationSelectionInfo; //0x05B4 
	SprintState_t sprintState; //0x05B8  sprintState
	float fTorsoPitch; //0x05CC 
	float fWaistPitch; //0x05D0 
	float holdBreathScale; //0x05D4 
	__int32 holdBreathTimer; //0x05D8 
	float moveSpeedScaleMultiplier; //0x05DC 
	MantleState mantleState; //0x05E0  mantleState
	float meleeChargeYaw; //0x05F0 
	__int32 meleeChargeDist; //0x05F4 
	__int32 meleeChargeTime; //0x05F8 
	__int32 perks; //0x05FC 
	int actionSlotType[4]; //0x0600 
	int actionSlotParm[4]; //0x0610 
	__int32 entityEventSequence; //0x0620 
	__int32 weapAnim; //0x0624 
	float aimSpreadScale; //0x0628 
	__int32 shellshockIndex; //0x062C 
	__int32 shellshockTime; //0x0630 
	__int32 shellshockDuration; //0x0634 
	float dofNearStart; //0x0638 
	float dofNearEnd; //0x063C 
	float dofFarStart; //0x0640 
	float dofFarEnd; //0x0644 
	float dofNearBlur; //0x0648 
	float dofFarBlur; //0x064C  shit goes haywire somewhere between here and the end of the
	__int32 hudElemLastAssignedSoundID; //0x0650  struct, but this shit isn't important anyways
char _0x0654[8];
	objective_t objective[16]; //0x065C 
	char weaponmodels[128]; //0x081C 
char _0x089C[4];
	__int32 deltaTime; //0x08A0 
	hudelem_t current[31]; //0x08A4 
};//Size=0x2F64

class trajectory_t
{
public:
	__int32 trType; //0x0000 
	__int32 trTime; //0x0004 
	__int32 trDuration; //0x0008 
	Vector3<_prec> trBase; //0x000C 
	Vector3<_prec> trDelta; //0x0018 
};//Size=0x0024

class LerpEntityState
{
public:
	__int32 eFlags; //0x0000 
	trajectory_t pos; //0x0004  pos
	trajectory_t apos; //0x0028  apos
char _0x004C[28];
};//Size=0x0068

class entityState_s
{
public:
	__int32 number; //0x0000 
	__int32 eType; //0x0004 
	LerpEntityState lerp; //0x0008  lerp
	__int32 time2; //0x0070 
	__int32 otherEntityNum; //0x0074 
	__int32 attackerEntityNum; //0x0078 
	__int32 groundEntityNum; //0x007C 
	__int32 loopSound; //0x0080 
	__int32 surfType; //0x0084 
	__int32 brushmodel; //0x0088 
	__int32 item; //0x008C 
	__int32 xmodel; //0x0090 
	__int32 primaryLight; //0x0094 
	__int32 clientNum; //0x0098 
	__int32 iHeadIcon; //0x009C 
	__int32 iHeadIconTeam; //0x00A0 
	__int32 solid; //0x00A4 
	DWORD eventParm; //0x00A8 
	__int32 eventSequence; //0x00AC 
char _0x00B0[20];
	__int32 weapon; //0x00C4 
	__int32 weaponModel; //0x00C8 
	__int32 legsAnim; //0x00CC 
	__int32 torsoAnim; //0x00D0 
	__int32 scale; //0x00D4 
	__int32 eventParm2; //0x00D8 
	__int32 helicopterStage; //0x00DC 
	__int32 hintString; //0x00E0 
	__int32 grenadeInPickupRange; //0x00E4 
	__int32 vehicleXModel; //0x00E8 
	float fTorsoPitch; //0x00EC 
	float fWaistPitch; //0x00F0 
};//Size=0x00F4

class clientState_s
{
public:
	__int32 clientIndex; //0x0000 
	__int32 team; //0x0004 
	__int32 modelindex; //0x0008 
	int attachModelIndex[6]; //0x000C 
	int attachTagIndex[6]; //0x0024 
	char name[32]; //0x003C 
	float maxSprintTimeMultiplier; //0x005C 
	__int32 rank; //0x0060 
};//Size=0x0064

class centity_s
{
public:
	DWORD lightingHandle; //0x0000 
char _0x0004[24];
	Vector3<_prec> origin; //0x001C 
	Vector3<_prec> angles; //0x0028 
char _0x0034[48];
	LerpEntityState currentState; //0x0064  currentState
	entityState_s nextState; //0x00CC  nextState
char _0x01C0[24];
	DWORD tree; //0x01D8  XAnimTree_s *tree
};//Size=0x01DC

class playerEntity_t
{
public:
	float fLastWeaponPosFrac; //0x0000 
	__int32 bPositionToADS; //0x0004 
	Vector3<_prec> vPositionLastOrg; //0x0008 
	float fLastIdleFactor; //0x0014 
	Vector3<_prec> vLastMoveOrg; //0x0018 
	Vector3<_prec> vLastMoveAng; //0x0024 
};//Size=0x0030

class GfxLight
{
public:
	__int8 type; //0x0000 
	__int8 canUseShadowMap; //0x0001 
char _0x0002[2];
	Vector3<_prec> color; //0x0004 
	Vector3<_prec> dir; //0x0010 
	Vector3<_prec> origin; //0x001C 
	float radius; //0x0028 
	float cosHalfFovOuter; //0x002C 
	float cosHalfFovInner; //0x0030 
	__int32 exponent; //0x0034 
	DWORD spotShadowIndex; //0x0038 
char _0x003C[4];
};//Size=0x0040

class refdef_s
{
public:
	DWORD x; //0x0000 
	DWORD y; //0x0004 
	DWORD width; //0x0008 
	DWORD height; //0x000C 
	float tanHalfFovX; //0x0010 
	float tanHalfFovY; //0x0014 
	Vector3<_prec> vieworg; //0x0018 
	Vector3<_prec> viewaxis_0; //0x0024 
	Vector3<_prec> viewaxis_1; //0x0030 
	Vector3<_prec> viewaxis_2; //0x003C 
	Vector3<_prec> viewOffset; //0x0048 
	__int32 time; //0x0054 
	float zNear; //0x0058 
	float blurRadius; //0x005C 
char _0x0060[32];
	__int32 enabled; //0x0080  GfxFilm
	float brightness; //0x0084 
	float contrast; //0x0088 
	float desaturation; //0x008C 
	__int32 invert; //0x0090 
	Vector3<_prec> tintDark; //0x0094 
	Vector3<_prec> tintLight; //0x00A0 
	__int32 glow_enabled; //0x00AC  GfxGlow
	float bloomCutoff; //0x00B0 
	float bloomDesaturation; //0x00B4 
	float bloomIntensity; //0x00B8 
	float radius; //0x00BC 
	GfxLight primaryLights[255]; //0x00C0 
	__int32 localClientNum; //0x4080 
};//Size=0x4084

class snapshot_s
{
public:
	__int32 snapFlags; //0x0000 
	__int32 ping; //0x0004 
	__int32 serverTime; //0x0008 
	playerState_s ps; //0x000C  ps
	__int32 numEntities; //0x2F70 
	__int32 numClients; //0x2F74 
	entityState_s entities[512]; //0x2F78 
	clientState_s clients[64]; //0x21778 
	__int32 serverCommandSequence; //0x23078 
};//Size=0x2307C

class cg_s
{
public:
	__int32 clientNum; //0x0000 
	__int32 localClientNum; //0x0004 
	__int32 demoType; //0x0008 
	__int32 cubemapShot; //0x000C 
	__int32 cubemapSize; //0x0010 
	__int32 renderScreen; //0x0014 
	__int32 latestSnapshotNum; //0x0018 
	__int32 latestSnapshotTime; //0x001C 
	snapshot_s* snap; //0x0020 
	snapshot_s* nextSnap; //0x0024 
	snapshot_s activeSnapshots[2]; //0x0028 
	float frameInterpolation; //0x46120 
	__int32 frameTime; //0x46124 
	__int32 time; //0x46128 
	__int32 oldTime; //0x4612C 
	__int32 physicsTime; //0x46130 
	__int32 mapRestart; //0x46134 
	__int32 renderingThirdPerson; //0x46138 
   playerState_s predictedPlayerState;
	centity_s predictedPlayerEntity; //0x4613C  predictedPlayerEntity
	playerEntity_t playerEntity; //0x46318  playerEntity
	__int32 predictedErrorTime; //0x46348 
	Vector3<_prec> predictedError; //0x4634C 
	float landChange; //0x46358 
	__int32 landTime; //0x4635C 
	float heightToCeiling; //0x46360 
	refdef_s refdef; //0x46364  refdef
	Vector3<_prec> refdefViewAngles; //0x4A3E8 
	Vector3<_prec> lastVieworg; //0x4A3F4 
	Vector3<_prec> swayViewAngles; //0x4A400 
	Vector3<_prec> swayAngles; //0x4A40C 
	Vector3<_prec> swayOffset; //0x4A418 
	int iEntityLastType[1024]; //0x4A424 
	int iEntityLastXModel[1024]; //0x4B424 
	float zoomSensitivity; //0x4C424 
	__int8 isLoading; //0x4C428 
	char objectiveText[1024]; //0x4C429 
	char scriptMainMenu[256]; //0x4C829 
char _0x4C929[3];
	__int32 N01AD2CB6; //0x4C92C 
char _0x4C930[57];
};//Size=0x4C969

class clSnapshot_t
{
public:
	__int32 valid; //0x0000 
	__int32 snapFlags; //0x0004 
	__int32 serverTime; //0x0008 
	__int32 messageNum; //0x000C 
	__int32 deltaNum; //0x0010 
	__int32 ping; //0x0014 
	__int32 cmdNum; //0x0018 
	playerState_s ps; //0x001C  ps
	__int32 numEntities; //0x2F80 
	__int32 numClients; //0x2F84 
	__int32 parseEntitiesNum; //0x2F88 
	__int32 parseClientsNum; //0x2F8C 
	__int32 serverCommandNum; //0x2F90 
};//Size=0x2F94

class clientActive_t
{
public:
	__int8 usingAds; //0x0000 
char _0x0001[3];
	__int32 timeoutcount; //0x0004 
	clSnapshot_t snap; //0x0008  snap
	__int32 serverTime; //0x2F9C 
	__int32 oldServerTime; //0x2FA0 
	__int32 oldFrameServerTime; //0x2FA4 
	__int32 serverTimeDelta; //0x2FA8 
	__int32 oldSnapServerTime; //0x2FAC 
	__int32 extrapolatedSnapshot; //0x2FB0 
	__int32 newSnapshots; //0x2FB4 
char _0x2FB8[1064];
};//Size=0x33E0

class BulletFireParams
{
public:
  int weaponEntIndex;
  int ignoreEntIndex;
  float damageMultiplier;
  int methodOfDeath;
  float origStart[3]; // 16
  float start[3]; // 28
  float end[3]; // 44
  float dir[3]; // 56
};

enum TraceHitType
{
  TRACE_HITTYPE_NONE = 0x0,
  TRACE_HITTYPE_ENTITY = 0x1,
  TRACE_HITTYPE_DYNENT_MODEL = 0x2,
  TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
};

#pragma pack(push, 2)
class trace_t
{
public:
  float fraction;
  float normal[3];
  int surfaceFlags;
  int contents;
  const char *material;
  TraceHitType hitType;
  unsigned __int16 hitId;
  unsigned __int16 modelIndex;
  unsigned __int16 partName;
  unsigned __int16 partGroup;
  bool allsolid;
  bool startsolid;
  bool walkable;
};
#pragma pack(pop)

#pragma pack(push, 4)
class BulletTraceResults
{
public:
  trace_t trace;
  void *hitEnt; // gentity_s*
  float hitPos[3];
  bool ignoreHitEnt;
};
#pragma pack(pop)

extern cg_s *_cg;
extern clientActive_t *_cl;

#pragma pack(pop)


#endif
