#ifndef HCLASSES_H
#define HCLASSES_H


// TODO: make type definitions for tag_t, font_t, shader_t and implement them

//--------------------------------------------------------------------------------------
//---------INTERNAL STRUCTURES----------------------------------------------------------
//-----------these are only used by the hack--------------------------------------------
//--------------------------------------------------------------------------------------


class Sortable_t
{
public:
   float _a;    // distance
   int   _b;    // entity num
   bool operator==(Sortable_t in);
   bool operator<=(Sortable_t in);
   bool operator<(Sortable_t in);
   bool operator>=(Sortable_t in);
   bool operator>(Sortable_t in);
};

typedef struct
{
   int serverTime;
   //int buttons;
   //int angles[3];
} Usercmd_t;


//--------------------------------------------------------------------------------------
//---------EXTERNAL STRUCTURES----------------------------------------------------------
//-----------these are used by the game-------------------------------------------------
//--------------------------------------------------------------------------------------

#ifdef GPTR
#undef GPTR
#endif // GPTR
#define GPTR    const

class CClientInfo;
class CEntity;
class CRefdef;
class CGame;
class CSnapShot;
class CViewMatrix;
class CCamera;
class CTrace;
class CWeaponData;
class CWeaponInfo;

#define FONT_SMALL_DEV             "fonts/smallDevFont"
#define FONT_BIG_DEV               "fonts/bigDevFont"
#define FONT_CONSOLE               "fonts/consoleFont"
#define FONT_BIG                   "fonts/bigFont"
#define FONT_SMALL                 "fonts/smallFont"
#define FONT_BOLD                  "fonts/boldFont"
#define FONT_NORMAL                "fonts/normalFont"
#define FONT_EXTRA_BIG             "fonts/extraBigFont"
#define FONT_OBJECTIVE             "fonts/objectiveFont"
#define CONTENTS_SOLID             0x0000001
#define CONTENTS_UNKNOWN1          0x000B000
#define CONTENTS_UNKNOWN2          0x0800000
#define CONTENTS_BODY              0x2000000
#define VISIBLE_TRACE              (CONTENTS_SOLID | CONTENTS_UNKNOWN1 | CONTENTS_UNKNOWN2)

#define FLAGS_STANDING             0x02     // standing, still
#define FLAGS_CROUCH               0x04     // crouch, still
#define FLAGS_PRONE                0x08     // prone, still
#define FLAGS_PRONE_FORWARD        0x100    // prone, moving forwards
#define FLAGS_PRONE_BACKWARD       0x200    // prone, moving backwards
#define FLAGS_STANDING_FORWARD     0x400    // standing, moving forwards
#define FLAGS_STANDING_BACKWARD    0x800    // standing, moving backwards
#define FLAGS_CROUCH_FORWARD       0x1000   // crouched, moving forwards
#define FLAGS_CROUCH_BACKWARD      0x2000   // crouched, moving backwards
#define FLAGS_CLIMBING             0x40000  // climbing a ladder
#define FLAGS_SPRINTING            0x100000 // sprinting
#define FLAGS_SCALING              0x800000 // the player is using the object to hop over/onto it

#define GENTITYNUM_BITS            10
#define MAX_GENTITIES              (1 << GENTITYNUM_BITS)
#define ENTITYNUM_WORLD            (MAX_GENTITIES - 2)

#define FLAG_01                    0x00000001
#define FLAG_02                    0x00000002
#define FLAG_03                    0x00000004
#define FLAG_04                    0x00000008
#define FLAG_05                    0x00000010
#define FLAG_06                    0x00000020
#define FLAG_07                    0x00000040
#define FLAG_08                    0x00000080
#define FLAG_09                    0x00000100
#define FLAG_10                    0x00000200
#define FLAG_11                    0x00000400
#define FLAG_12                    0x00000800
#define FLAG_13                    0x00001000
#define FLAG_14                    0x00002000
#define FLAG_15                    0x00004000
#define FLAG_16                    0x00008000
#define FLAG_17                    0x00010000
#define FLAG_18                    0x00020000
#define FLAG_19                    0x00040000
#define FLAG_20                    0x00080000
#define FLAG_21                    0x00100000
#define FLAG_22                    0x00200000
#define FLAG_23                    0x00400000
#define FLAG_24                    0x00800000
#define FLAG_25                    0x01000000
#define FLAG_26                    0x02000000
#define FLAG_27                    0x04000000
#define FLAG_28                    0x08000000
#define FLAG_29                    0x10000000
#define FLAG_30                    0x20000000
#define FLAG_31                    0x40000000
#define FLAG_32                    0x80000000

#define MENU_OPEN_FLAG             0x10

typedef enum
{
   TYPE_SMOKE         = 0,
   TYPE_HUMAN         = 1,
   TYPE_DEAD          = 2,
   TYPE_WEAPON        = 3,
   TYPE_EXPLOSIVE     = 4,
   TYPE_VEHICLE       = 6,
   TYPE_CLAYMORELASER = 8,
   TYPE_TURRET        = 11,
   TYPE_HELICOPTER    = 12,
   TYPE_AIRSTRIKE     = 13,
   TYPE_ZOMBIE        = 20,
   TYPE_OLDEXPLOSIVE  = 78,
} EntType;

typedef enum
{
   qfalse,
   qtrue
} qboolean;

typedef enum
{
   GLOW_NORMAL        = 0,
   GLOW_SHADOWED      = 3,
   GLOW_MORE_SHADOWED = 6
} glow_style;

typedef enum
{
   TEAM_FFA,
   TEAM_ALLIES,
   TEAM_ENEMIES,
   TEAM_SPECTATOR,
   TEAM_TEAMS
} team_t;

class CWeaponData
{
public:
   char *ViewMode;
   char unknown1[216];
   char *WeaponFx;
};

class CWeaponInfo
{
public:
   char        unknown0[4];
   CWeaponData *WeaponInfo;
   char        unknown2[48];
   char        *Name;
};

class CEntityState
{
public:
   int  number;
   int  eType;
   int  eFlags;
   char z_crap[104];
   int  otherEntityNum;
   int  otherEntityNum2;
   int  groundEntityNum;
   int  LastXpReq;
   int  XpReq;
   int  clientNum;
   char z_crap2[12];
   int  event;
   int  eventParm;
};

typedef struct
{
   float          numOfIntersections;
   Vector3<_prec> firstIntersection;
   Vector3<_prec> secondIntersection;
} morelazies_t;

class CPlayer
{
public:
   char    space_01[328];
   __int32 health;
};

class CHUD
{
public:
   float HudScaleX;
   float HudScaleY;
   float BloodSplatterScaleX;
   float BloodSplatterScaleY;
};

class CCVar
{
public:
   char  *Name;
   DWORD Handle;
   DWORD Flags;
   union
   {
      DWORD dwVal;
      float fVal;
   };
   float Float;
   char  *Description;
   DWORD FunctionAddress;
   int   LatchedInteger;
   float LatchedFloat;
};

class CClientInfo
{
public:
   char           unknown0[12];
   char           Name[16];             //000C
   __int32        Team;                 //001C
   char           unknown1[4];
   __int32        Rank;                 //0024
   char           unknown2[20];
   char           Class[32];            //003C
   char           unknown3[828];
   Vector3<_prec> Origin;               //0398
   char           unknown4[8];
   Vector3<_prec> Rotation;             //03AC
   char           unknown5[40];
   Vector3<_prec> Angles;               //03E4
   char           unknown6[132];
   __int32        State;                //0470
   char           unknown7[20];
   __int32        Shooting;             //0488
   char           unknown8[4];
   __int32        Zoomed;               //0490
   char           unknown9[24];
   __int32        WeaponNumber;         //04AC
};

class CEntity
{
public:
   BOOL           IsValidEnt; // 0
   char           unknown0[24]; // 4
   Vector3<_prec> vOrigin; // 28
   Vector3<_prec> vAngles; // 40
   char           unknown1[64]; // 52
   Vector3<_prec> vOldOrigin; // 116
   Vector3<_prec> vOldAngles; // 128
   char           unknown2[36]; // 140
   float          Roll; // 176
   int            lerpBase; // 180
   char           unknown3[20]; // 184
   __int32        ClientNumber; // 204
   __int32        EntityType; // 208
   __int32        State; // 212
   char           unknown5[12]; // 216
   Vector3<_prec> vNewOrigin; // 228
   char           unknown9[24]; // 240
   Vector3<_prec> vNewAngles; // 264
   char           unknown12[124]; // 276
   unsigned int   WeaponId;
   char           unknown14[44];
   __int32        Alive;
};

class CCompassEntity
{
public:
   int    entityIndex;
   int    time;
   vec3_t origin;
   int    team;
   int    clientNum;
};  // size = 0x1C (.28)

class CGame
{
public:
   int       clientNum;
   int       demoPlayback;
   char      z_crap[24];
   CSnapShot *CurrentSnap;
   CSnapShot *NextSnap;
};

class CSnapShot
{
public:
   char           unknown0[4]; // 0
   int            Ping; // 4
   int            ServerTime; // 8
   int            *CommandTime; // 12
   char           unk[16]; // 16
   int            PMF_Flags; // 32
   int            Unknown; // 36
   Vector3<_prec> vOrigin; // 40
   Vector3<_prec> vNewOrigin; // 52
   char           unknown1[168]; // 64
   __int32        LocalClientNumber; // 232
   char           unknown2[8]; // 236
   int            curWeapon; // 244
   int            WeaponState; // 248
   int            Fill; // 252
   float          Zoom; // 256
   char           unknown3[16]; // 260
   float          Pitch; // 276
   float          Yaw; // 280
   char           unknown4[11860]; // 284
   __int32        NumEntities;
};

class CRefdef
{
public:
   int            x;
   int            y;
   __int32        iWidth;
   __int32        iHeight;
   float          FovX;
   float          FovY;
   Vector3<_prec> vOrigin;
   Vector3<_prec> ViewAxis[3];
   char           Unknown[0x4050];
   Vector3<_prec> ViewAngles;
};

class CCamera
{
public:
   char           unknown0[24];
   char           MapName[16];
   char           unknown1[100];
   float          Fov;
   char           unknown2[20];
   Vector3<_prec> vOrigin;
   Vector3<_prec> Velocity;
   float          ViewAngleX;
   float          ViewAngleY;
   char           unknown9[20];
   Vector3<_prec> ViewAngles;
   char           unk[64];
   Vector3<_prec> vNewOrigin;
};

class CViewMatrix
{
public:
   char           unknown0[72];
   Vector3<_prec> Recoil[3];
   Vector3<_prec> ViewAngles;
   char           unknown1[24];
   float          unk2;
   float          unk;
   char           unkown2[4];
   Vector3<_prec> ViewAxis[3];
   Vector3<_prec> vOrigin;
};

class CTrace
{
public:
   float          Fraction;
   Vector3<_prec> surfaceDirection;
   int            surfaceFlags_1; // TODO: rename to materialNum
   int            surfaceFlags_2;
   char           *surfaceName;
   int            contents;
   short          EntityNumber;
   BYTE           unknown1[6];
   char           allsolid;
   char           startsolid;
   int            planeInvalid;
   BYTE           unknown2[2];
   Vector3<_prec> end;
   int            hitSelf;
   int            materialType;
};

class CBulletTrace
{
public:
   int            weaponEntIndex;
   int            ignoreEntIndex;
   float          damageMultiplier;
   int            methodOfDeath;
   Vector3<_prec> origStart;
   Vector3<_prec> start;
   Vector3<_prec> end;
   Vector3<_prec> dir;
};

typedef enum
{
   UNKNOWN,
   CONSOLE,
   KILLPOSITION,
   CENTER,
} echo_type_t;

class CUsercmd
{
public:
   int  serverTime; // 0
   int  buttons; // 4
   int  angles[3]; // 8
   char WeaponIndex; // 20
   char space; // 21
   char forward; // 22
   char right; // 23 
   char up; // 24
   char space2[7]; // 25
};

class CWeapon
{
public:
   char  _pad0[0x4];                       // 0000
   char  *displayName;                     // 0004
   char  *AIOverlayDescription;            // 0008
   void  *gunModel;                        // 000C, XModel*
   void  *gunModel2;                       // 0010, XModel*
   void  *gunModel3;                       // 0014, XModel*
   void  *gunModel4;                       // 0018, XModel*
   void  *gunModel5;                       // 001C, XModel*
   void  *gunModel6;                       // 0020, XModel*
   void  *gunModel7;                       // 0024, XModel*
   void  *gunModel8;                       // 0028, XModel*
   void  *gunModel9;                       // 002C, XModel*
   void  *gunModel10;                      // 0030, XModel*
   void  *gunModel11;                      // 0034, XModel*
   void  *gunModel12;                      // 0038, XModel*
   void  *gunModel13;                      // 003C, XModel*
   void  *gunModel14;                      // 0040, XModel*
   void  *gunModel15;                      // 0044, XModel*
   void  *gunModel16;                      // 0048, XModel*
   void  *handModel;                       // 004C, XModel*
   char  _pad1[0x4];                       // 0050
   char  *idleAnim;                        // 0054
   char  *emptyIdleAnim;                   // 0058
   char  *fireAnim;                        // 005C
   char  *holdFireAnim;                    // 0060
   char  *lastShotAnim;                    // 0064
   char  *rechamberAnim;                   // 0068
   char  *meleeAnim;                       // 006C
   char  *meleeChargeAnim;                 // 0070
   char  *reloadAnim;                      // 0074
   char  *reloadEmptyAnim;                 // 0078
   char  *reloadStartAnim;                 // 007C
   char  *reloadEndAnim;                   // 0080
   char  *raiseAnim;                       // 0084
   char  *firstRaiseAnim;                  // 0088
   char  *dropAnim;                        // 008C
   char  *altRaiseAnim;                    // 0090
   char  *altDropAnim;                     // 0094
   char  *quickRaiseAnim;                  // 0098
   char  *quickDropAnim;                   // 009C
   char  *emptyRaiseAnim;                  // 00A0
   char  *emptyDropAnim;                   // 00A4
   char  *sprintInAnim;                    // 00A8
   char  *sprintLoopAnim;                  // 00AC
   char  *sprintOutAnim;                   // 00B0
   char  *detonateAnim;                    // 00B4
   char  *nightVisionWearAnim;             // 00B8
   char  *nightVisionRemoveAnim;           // 00BC
   char  *adsFireAnim;                     // 00C0
   char  *adsLastShotAnim;                 // 00C4
   char  *adsRechamberAnim;                // 00C8
   char  *adsUpAnim;                       // 00CC
   char  *adsDownAnim;                     // 00D0
   char  *modeName;                        // 00D4
   int   hideTags;                         // 00D8, Type 33
   char  _pad2[0xC];                       // 00DC
   int   notetrackSoundMap;                // 00E8, Type 34
   char  _pad3[0x3C];                      // 00EC
   int   playerAnimType;                   // 0128, Type 20
   int   weaponType;                       // 012C, Type 12
   int   weaponClass;                      // 0130, Type 13
   int   penetrateType;                    // 0134, Type 15
   int   impactType;                       // 0138, Type 16
   int   inventoryType;                    // 013C, Type 26
   int   fireType;                         // 0140, Type 27
   int   offhandClass;                     // 0144, Type 19
   int   stance;                           // 0148, Type 17
   int   viewFlashEffect;                  // 014C, Type 8
   int   worldFlashEffect;                 // 0150, Type 8
   int   pickupSound;                      // 0154, Type 11
   int   pickupSoundPlayer;                // 0158, Type 11
   int   ammoPickupSound;                  // 015C, Type 11
   int   ammoPickupSoundPlayer;            // 0160, Type 11
   int   projectileSound;                  // 0164, Type 11
   int   pullbackSound;                    // 0168, Type 11
   int   pullbackSoundPlayer;              // 016C, Type 11
   int   fireSound;                        // 0170, Type 11
   int   fireSoundPlayer;                  // 0174, Type 11
   int   loopFireSound;                    // 0178, Type 11
   int   loopFireSoundPlayer;              // 017C, Type 11
   int   stopFireSound;                    // 0180, Type 11
   int   stopFireSoundPlayer;              // 0184, Type 11
   int   lastShotSound;                    // 0188, Type 11
   int   lastShotSoundPlayer;              // 018C, Type 11
   int   emptyFireSound;                   // 0190, Type 11
   int   emptyFireSoundPlayer;             // 0194, Type 11
   int   meleeSwipeSound;                  // 0198, Type 11
   int   meleeSwipeSoundPlayer;            // 019C, Type 11
   int   meleeHitSound;                    // 01A0, Type 11
   int   meleeMissSound;                   // 01A4, Type 11
   int   rechamberSound;                   // 01A8, Type 11
   int   rechamberSoundPlayer;             // 01AC, Type 11
   int   reloadSound;                      // 01B0, Type 11
   int   reloadSoundPlayer;                // 01B4, Type 11
   int   reloadEmptySound;                 // 01B8, Type 11
   int   reloadEmptySoundPlayer;           // 01BC, Type 11
   int   reloadStartSound;                 // 01C0, Type 11
   int   reloadStartSoundPlayer;           // 01C4, Type 11
   int   reloadEndSound;                   // 01C8, Type 11
   int   reloadEndSoundPlayer;             // 01CC, Type 11
   int   detonateSound;                    // 01D0, Type 11
   int   detonateSoundPlayer;              // 01D4, Type 11
   int   nightVisionWearSound;             // 01D8, Type 11
   int   nightVisionWearSoundPlayer;       // 01DC, Type 11
   int   nightVisionRemoveSound;           // 01E0, Type 11
   int   nightVisionRemoveSoundPlayer;     // 01E4, Type 11
   int   altSwitchSound;                   // 01E8, Type 11
   int   altSwitchSoundPlayer;             // 01EC, Type 11
   int   raiseSound;                       // 01F0, Type 11
   int   raiseSoundPlayer;                 // 01F4, Type 11
   int   firstRaiseSound;                  // 01F8, Type 11
   int   firstRaiseSoundPlayer;            // 01FC, Type 11
   int   putawaySound;                     // 0200, Type 11
   int   putawaySoundPlayer;               // 0204, Type 11
   int   bounceSound;                      // 0208, Type 23
   int   viewShellEjectEffect;             // 020C, Type 8
   int   worldShellEjectEffect;            // 0210, Type 8
   int   viewLastShotEjectEffect;          // 0214, Type 8
   int   worldLastShotEjectEffect;         // 0218, Type 8
   int   reticleCenter;                    // 021C, Type 10
   int   reticleSide;                      // 0220, Type 10
   int   reticleCenterSize;                // 0224
   int   reticleSideSize;                  // 0228
   int   reticleMinOfs;                    // 022C
   int   activeReticleType;                // 0230, Type 21
   float standMoveF;                       // 0234
   float standMoveR;                       // 0238
   float standMoveU;                       // 023C
   float standRotP;                        // 0240
   float standRotY;                        // 0244
   float standRotR;                        // 0248
   float duckedOfsF;                       // 024C
   float duckedOfsR;                       // 0250
   float duckedOfsU;                       // 0254
   float duckedMoveF;                      // 0258
   float duckedMoveR;                      // 025C
   float duckedMoveU;                      // 0260
   float duckedRotP;                       // 0264
   float duckedRotY;                       // 0268
   float duckedRotR;                       // 026C
   float proneOfsF;                        // 0270
   float proneOfsR;                        // 0274
   float proneOfsU;                        // 0278
   float proneMoveF;                       // 027C
   float proneMoveR;                       // 0280
   float proneMoveU;                       // 0284
   float proneRotP;                        // 0288
   float proneRotY;                        // 028C
   float proneRotR;                        // 0290
   float posMoveRate;                      // 0294
   float posProneMoveRate;                 // 0298
   float standMoveMinSpeed;                // 029C
   float duckedMoveMinSpeed;               // 02A0
   float proneMoveMinSpeed;                // 02A4
   float posRotRate;                       // 02A8
   float posProneRotRate;                  // 02AC
   float standRotMinSpeed;                 // 02B0
   float duckedRotMinSpeed;                // 02B4
   float proneRotMinSpeed;                 // 02B8
   void  *worldModel;                      // 02BC, XModel*
   void  *worldModel2;                     // 02C0, XModel*
   void  *worldModel3;                     // 02C4, XModel*
   void  *worldModel4;                     // 02C8, XModel*
   void  *worldModel5;                     // 02CC, XModel*
   void  *worldModel6;                     // 02D0, XModel*
   void  *worldModel7;                     // 02D4, XModel*
   void  *worldModel8;                     // 02D8, XModel*
   void  *worldModel9;                     // 02DC, XModel*
   void  *worldModel10;                    // 02E0, XModel*
   void  *worldModel11;                    // 02E4, XModel*
   void  *worldModel12;                    // 02E8, XModel*
   void  *worldModel13;                    // 02EC, XModel*
   void  *worldModel14;                    // 02F0, XModel*
   void  *worldModel15;                    // 02F4, XModel*
   void  *worldModel16;                    // 02F8, XModel*
   void  *worldClipModel;                  // 02FC, XModel*
   void  *rocketModel;                     // 0300, XModel*
   void  *knifeModel;                      // 0304, XModel*
   void  *worldKnifeModel;                 // 0308, XModel*
   int   hudIcon;                          // 030C, Type 10
   int   hudIconRatio;                     // 0310, Type 29
   int   ammoCounterIcon;                  // 0314, Type 10
   int   ammoCounterIconRatio;             // 0318, Type 30
   int   ammoCounterClip;                  // 031C, Type 28
   int   startAmmo;                        // 0320
   char  *ammoName;                        // 0324
   char  _pad4[0x4];                       // 0328
   char  *clipName;                        // 032C
   int   AmmoIndex;                        //330
   int   maxAmmo;                          // 0334
   int   clipSize;                         // 0338
   int   shotCount;                        // 033C
   char  *sharedAmmoCapName;               // 0340
   char  _pad6[0x4];                       // 0344
   int   sharedAmmoCap;                    // 0348
   int   damage;                           // 034C
   int   playerDamage;                     // 0350
   int   meleeDamage;                      // 0354
   char  _pad7[0x4];                       // 0358
   int   fireDelay;                        // 035C, Type 7
   int   meleeDelay;                       // 0360, Type 7
   int   meleeChargeDelay;                 // 0364, Type 7
   int   detonateDelay;                    // 0368, Type 7
   int   fireTime;                         // 036C, Type 7
   int   rechamberTime;                    // 0370, Type 7
   int   rechamberBoltTime;                // 0374, Type 7
   int   holdFireTime;                     // 0378, Type 7
   int   detonateTime;                     // 037C, Type 7
   int   meleeTime;                        // 0380, Type 7
   int   meleeChargeTime;                  // 0384, Type 7
   int   reloadTime;                       // 0388, Type 7
   int   reloadShowRocketTime;             // 038C, Type 7
   int   reloadEmptyTime;                  // 0390, Type 7
   int   reloadAddTime;                    // 0394, Type 7
   int   reloadStartTime;                  // 0398, Type 7
   int   reloadStartAddTime;               // 039C, Type 7
   int   reloadEndTime;                    // 03A0, Type 7
   int   dropTime;                         // 03A4, Type 7
   int   raiseTime;                        // 03A8, Type 7
   int   altDropTime;                      // 03AC, Type 7
   int   altRaiseTime;                     // 03B0, Type 7
   int   quickDropTime;                    // 03B4, Type 7
   int   quickRaiseTime;                   // 03B8, Type 7
   int   firstRaiseTime;                   // 03BC, Type 7
   int   emptyRaiseTime;                   // 03C0, Type 7
   int   emptyDropTime;                    // 03C4, Type 7
   int   sprintInTime;                     // 03C8, Type 7
   int   sprintLoopTime;                   // 03CC, Type 7
   int   sprintOutTime;                    // 03D0, Type 7
   int   nightVisionWearTime;              // 03D4, Type 7
   int   nightVisionWearTimeFadeOutEnd;    // 03D8, Type 7
   int   nightVisionWearTimePowerUp;       // 03DC, Type 7
   int   nightVisionRemoveTime;            // 03E0, Type 7
   int   nightVisionRemoveTimePowerDown;   // 03E4, Type 7
   int   nightVisionRemoveTimeFadeInStart; // 03E8, Type 7
   int   fuseTime;                         // 03EC, Type 7
   int   aifuseTime;                       // 03F0, Type 7
   BOOL  requireLockonToFire;              // 03F4
   BOOL  noAdsWhenMagEmpty;                // 03F8
   BOOL  avoidDropCleanup;                 // 03FC
   float autoAimRange;                     // 0400
   float aimAssistRange;                   // 0404
   float aimAssistRangeAds;                // 0408
   float aimPadding;                       // 040C
   float enemyCrosshairRange;              // 0410
   BOOL  crosshairColorChange;             // 0414
   float moveSpeedScale;                   // 0418
   float adsMoveSpeedScale;                // 041C
   float sprintDurationScale;              // 0420
   float adsZoomFov;                       // 0424
   float adsZoomInFrac;                    // 0428
   float adsZoomOutFrac;                   // 042C
   int   adsOverlayShader;                 // 0430, Type 10
   int   adsOverlayShaderLowRes;           // 0434, Type 10
   int   adsOverlayReticle;                // 0438, Type 14
   int   adsOverlayInterface;              // 043C, Type 25
   float adsOverlayWidth;                  // 0440
   float adsOverlayHeight;                 // 0444
   float adsBobFactor;                     // 0448
   float adsViewBobMult;                   // 044C
   float hipSpreadStandMin;                // 0450
   float hipSpreadDuckedMin;               // 0454
   float hipSpreadProneMin;                // 0458
   float hipSpreadMax;                     // 045C
   float hipSpreadDuckedMax;               // 0460
   float hipSpreadProneMax;                // 0464
   float hipSpreadDecayRate;               // 0468
   float hipSpreadFireAdd;                 // 046C
   float hipSpreadTurnAdd;                 // 0470
   float hipSpreadMoveAdd;                 // 0474
   float hipSpreadDuckedDecay;             // 0478
   float hipSpreadProneDecay;              // 047C
   float hipReticleSidePos;                // 0480
   int   adsTransInTime;                   // 0484, Type 7
   int   adsTransOutTime;                  // 0488, Type 7
   float adsIdleAmount;                    // 048C
   float hipIdleAmount;                    // 0490
   float adsIdleSpeed;                     // 0494
   float hipIdleSpeed;                     // 0498
   float idleCrouchFactor;                 // 049C
   float idleProneFactor;                  // 04A0
   float gunMaxPitch;                      // 04A4
   float gunMaxYaw;                        // 04A8
   float swayMaxAngle;                     // 04AC
   float swayLerpSpeed;                    // 04B0
   float swayPitchScale;                   // 04B4
   float swayYawScale;                     // 04B8
   float swayHorizScale;                   // 04BC
   float swayVertScale;                    // 04C0
   float swayShellShockScale;              // 04C4
   float adsSwayMaxAngle;                  // 04C8
   float adsSwayLerpSpeed;                 // 04CC
   float adsSwayPitchScale;                // 04D0
   float adsSwayYawScale;                  // 04D4
   float adsSwayHorizScale;                // 04D8
   float adsSwayVertScale;                 // 04DC
   BOOL  rifleBullet;                      // 04E0
   BOOL  armorPiercing;                    // 04E4
   BOOL  boltAction;                       // 04E8
   BOOL  aimDownSight;                     // 04EC
   BOOL  rechamberWhileAds;                // 04F0
   float adsViewErrorMin;                  // 04F4
   float adsViewErrorMax;                  // 04F8
   BOOL  cookOffHold;                      // 04FC
   BOOL  clipOnly;                         // 0500
   BOOL  adsFire;                          // 0504
   BOOL  cancelAutoHolsterWhenEmpty;       // 0508
   BOOL  suppressAmmoReserveDisplay;       // 050C
   BOOL  enhanced;                         // 0510
   BOOL  laserSightDuringNightvision;      // 0514
   int   killIcon;                         // 0518, Type 10
   int   killIconRatio;                    // 051C, Type 31
   BOOL  flipKillIcon;                     // 0520
   int   dpadIcon;                         // 0524, Type 10
   int   dpadIconRatio;                    // 0528, Type 32
   BOOL  noPartialReload;                  // 052C
   BOOL  segmentedReload;                  // 0530
   int   reloadAmmoAdd;                    // 0534
   int   reloadStartAdd;                   // 0538
   char  *altWeapon;                       // 053C
   char  _pad8[0x4];                       // 0540
   int   dropAmmoMin;                      // 0544
   int   dropAmmoMax;                      // 0548
   BOOL  blocksProne;                      // 054C
   BOOL  silenced;                         // 0550
   int   explosionRadius;                  // 0554
   int   explosionRadiusMin;               // 0558
   int   explosionInnerDamage;             // 055C
   int   explosionOuterDamage;             // 0560
   float damageConeAngle;                  // 0564
   int   projectileSpeed;                  // 0568
   int   projectileSpeedUp;                // 056C
   int   projectileSpeedForward;           // 0570
   int   projectileActivateDist;           // 0574
   float projectileLifetime;               // 0578
   float timeToAccelerate;                 // 057C
   float projectileCurvature;              // 0580
   void  *projectileModel;                 // 0584, XModel*
   int   projExplosionType;                // 0588, Type 18
   int   projExplosionEffect;              // 058C, Type 8
   BOOL  projExplosionEffectForceNormalUp; // 0590
   int   projDudEffect;                    // 0594, Type 8
   int   projExplosionSound;               // 0598, Type 11
   int   projDudSound;                     // 059C, Type 11
   BOOL  projImpactExplode;                // 05A0
   int   stickiness;                       // 05A4, Type 24
   BOOL  hasDetonator;                     // 05A8
   BOOL  timedDetonation;                  // 05AC
   BOOL  rotate;                           // 05B0
   BOOL  holdButtonToThrow;                // 05B4
   BOOL  freezeMovementWhenFiring;         // 05B8
   float lowAmmoWarningThreshold;          // 05BC
   float parallelDefaultBounce;            // 05C0
   float parallelBarkBounce;               // 05C4
   float parallelBrickBounce;              // 05C8
   float parallelCarpetBounce;             // 05CC
   float parallelClothBounce;              // 05D0
   float parallelConcreteBounce;           // 05D4
   float parallelDirtBounce;               // 05D8
   float parallelFleshBounce;              // 05DC
   float parallelFoliageBounce;            // 05E0
   float parallelGlassBounce;              // 05E4
   float parallelGrassBounce;              // 05E8
   float parallelGravelBounce;             // 05EC
   float parallelIceBounce;                // 05F0
   float parallelMetalBounce;              // 05F4
   float parallelMudBounce;                // 05F8
   float parallelPaperBounce;              // 05FC
   float parallelPlasterBounce;            // 0600
   float parallelRockBounce;               // 0604
   float parallelSandBounce;               // 0608
   float parallelSnowBounce;               // 060C
   float parallelWaterBounce;              // 0610
   float parallelWoodBounce;               // 0614
   float parallelAsphaltBounce;            // 0618
   float parallelCeramicBounce;            // 061C
   float perpendicularCeramicBounce;       // 061C
   float parallelPlasticBounce;            // 0620
   float perpendicularPlasticBounce;       // 0620
   float parallelRubberBounce;             // 0624
   float perpendicularRubberBounce;        // 0624
   float parallelCushionBounce;            // 0628
   float parallelFruitBounce;              // 062C
   float parallelPaintedMetalBounce;       // 0630
   float perpendicularDefaultBounce;       // 0634
   float perpendicularBarkBounce;          // 0638
   float perpendicularBrickBounce;         // 063C
   float perpendicularCarpetBounce;        // 0640
   float perpendicularClothBounce;         // 0644
   float perpendicularConcreteBounce;      // 0648
   float perpendicularDirtBounce;          // 064C
   float perpendicularFleshBounce;         // 0650
   float perpendicularFoliageBounce;       // 0654
   float perpendicularGlassBounce;         // 0658
   float perpendicularGrassBounce;         // 065C
   float perpendicularGravelBounce;        // 0660
   float perpendicularIceBounce;           // 0664
   float perpendicularMetalBounce;         // 0668
   float perpendicularMudBounce;           // 066C
   float perpendicularPaperBounce;         // 0670
   float perpendicularPlasterBounce;       // 0674
   float perpendicularRockBounce;          // 0678
   float perpendicularSandBounce;          // 067C
   float perpendicularSnowBounce;          // 0680
   float perpendicularWaterBounce;         // 0684
   float perpendicularWoodBounce;          // 0688
   float perpendicularAsphaltBounce;       // 068C
   char  _pad9[0xC];                       // 0690
   float perpendicularCushionBounce;       // 069C
   float perpendicularFruitBounce;         // 06A0
   float perpendicularPaintedMetalBounce;  // 06A4
   int   projTrailEffect;                  // 06A8, Type 8
   float projectileRed;                    // 06AC
   float projectileGreen;                  // 06B0
   float projectileBlue;                   // 06B4
   int   guidedMissileType;                // 06B8, Type 22
   float maxSteeringAccel;                 // 06BC
   int   projIgnitionDelay;                // 06C0
   int   projIgnitionEffect;               // 06C4, Type 8
   int   projIgnitionSound;                // 06C8, Type 11
   float adsAimPitch;                      // 06CC
   float adsCrosshairInFrac;               // 06D0
   float adsCrosshairOutFrac;              // 06D4
   int   adsGunKickReducedKickBullets;     // 06D8
   float adsGunKickReducedKickPercent;     // 06DC
   float adsGunKickPitchMin;               // 06E0
   float adsGunKickPitchMax;               // 06E4
   float adsGunKickYawMin;                 // 06E8
   float adsGunKickYawMax;                 // 06EC
   float adsGunKickAccel;                  // 06F0
   float adsGunKickSpeedMax;               // 06F4
   float adsGunKickSpeedDecay;             // 06F8
   float adsGunKickStaticDecay;            // 06FC
   float adsViewKickPitchMin;              // 0700
   float adsViewKickPitchMax;              // 0704
   float adsViewKickYawMin;                // 0708
   float adsViewKickYawMax;                // 070C
   float adsViewKickCenterSpeed;           // 0710
   char  _pad10[0x8];                      // 0714
   float adsSpread;                        // 071C
   int   hipGunKickReducedKickBullets;     // 0720
   float hipGunKickReducedKickPercent;     // 0724
   float hipGunKickPitchMin;               // 0728
   float hipGunKickPitchMax;               // 072C
   float hipGunKickYawMin;                 // 0730
   float hipGunKickYawMax;                 // 0734
   float hipGunKickAccel;                  // 0738
   float hipGunKickSpeedMax;               // 073C
   float hipGunKickSpeedDecay;             // 0740
   float hipGunKickStaticDecay;            // 0744
   float hipViewKickPitchMin;              // 0748
   float hipViewKickPitchMax;              // 074C
   float hipViewKickYawMin;                // 0750
   float hipViewKickYawMax;                // 0754
   float hipViewKickCenterSpeed;           // 0758
   char  _pad11[0x8];                      // 075C
   float fightDist;                        // 0764
   float maxDist;                          // 0768
   char  *aiVsAiAccuracyGraph;             // 076C
   char  *aiVsPlayerAccuracyGraph;         // 0770
   char  _pad12[0x20];                     // 0774
   int   adsReloadTransTime;               // 0794, Type 7
   float leftArc;                          // 0798
   float rightArc;                         // 079C
   float topArc;                           // 07A0
   float bottomArc;                        // 07A4
   float accuracy;                         // 07A8
   float aiSpread;                         // 07AC
   float playerSpread;                     // 07B0
   float minVertTurnSpeed;                 // 07B4
   float minHorTurnSpeed;                  // 07B8
   float maxVertTurnSpeed;                 // 07BC
   float maxHorTurnSpeed;                  // 07C0
   float pitchConvergenceTime;             // 07C4
   float yawConvergenceTime;               // 07C8
   float suppressionTime;                  // 07CC
   float maxRange;                         // 07D0
   float animHorRotateInc;                 // 07D4
   float playerPositionDist;               // 07D8
   char  *useHintString;                   // 07DC
   char  *dropHintString;                  // 07E0
   char  _pad13[0x8];                      // 07E4
   float horizViewJitter;                  // 07EC
   float vertViewJitter;                   // 07F0
   char  *script;                          // 07F4
   char  _pad14[0x8];                      // 07F8
   float minDamage;                        // 0800
   int   minPlayerDamage;                  // 0804
   float maxDamageRange;                   // 0808
   float minDamageRange;                   // 080C
   float destabilizationRateTime;          // 0810
   float destabilizationCurvatureMax;      // 0814
   int   destabilizeDistance;              // 0818
   float locNone;                          // 081C
   float locHelmet;                        // 0820
   float locHead;                          // 0824
   float locNeck;                          // 0828
   float locTorsoUpper;                    // 082C
   float locTorsoLower;                    // 0830
   float locRightArmUpper;                 // 0834
   float locLeftArmUpper;                  // 0838
   float locRightArmLower;                 // 083C
   float locLeftArmLower;                  // 0840
   float locRightHand;                     // 0844
   float locLeftHand;                      // 0848
   float locRightLegUpper;                 // 084C
   float locLeftLegUpper;                  // 0850
   float locRightLegLower;                 // 0854
   float locLeftLegLower;                  // 0858
   float locRightFoot;                     // 085C
   float locLeftFoot;                      // 0860
   float locGun;                           // 0864
   char  *fireRumble;                      // 0868
   char  *meleeImpactRumble;               // 086C
   float adsDofStart;                      // 0870
   float adsDofEnd;                        // 0874
};

class twice_playerstate_t
{
public:
   char xx[1440];          // 0
   int  pickupValue;       // 1440
   char xxx[4];            // 1444
   int  pickupEntityIndex; // 1448
};

class snapshot_t
{
public:
   int                 snapFlags;
   int                 ping;
   int                 serverTime;
   twice_playerstate_t ps;
};

class twice_cg_t
{
public:
   int        clientNum;
   BYTE       unknown0[16];
   bool       isInGame;
   int        latestSnapshotNum;
   int        latestSnapshotTime;
   snapshot_t *snap;
   snapshot_t *nextSnap;
};

class CPlayerState
{
public:
   DWORD          time;       //0x0000
   char           _0x0004[24];
   Vector3<_prec> origin;     //0x001C
   Vector3<_prec> vel;        //0x0028
   char           _0x0028[8];
   DWORD          weaponTime; //0x003C
};                            //Size=0x0148

extern CRefdef *GPTR        refdef;
extern CGame *GPTR          cg;
extern CCompassEntity *GPTR compassEntities;
extern twice_cg_t *GPTR     twice_cg;
extern CCamera *GPTR        camera;
extern CViewMatrix *GPTR    viewmatrix;
extern CWeaponInfo *GPTR    cg_weapons;
extern CPlayerState *GPTR   predictedPlayerState;
extern CHUD *GPTR           hud;

#endif
