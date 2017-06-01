#ifndef HDEFINES_H
#define HDEFINES_H

#define HVERSION    1 // 1 = X, 2 = Y, 3 = Z, 4 = (custom)

#define RUFlagAlpha 0.75

//#define DISABLED3D9
//#define COMPILE_FOR_LOADER // comment to compile for testing purposes
//#define EXCEPTION_LOGGER
//#define H47 // comment to disable new aimbot core
//#define REGISTER_DUMP
//#define STACK_DUMP // stack dump for exception logger
//#define ANTI_CRASH // anti-crash using SEH, untested for engine functions, only works 100% for X version
#define MANUAL_MAPPED // comment if not manually mapped
//#define USE_VMPROTECT_SDK // comment to not use VMProtect SDK
//#define USE_EXTRA_VMP // comment to not encrypt a few functions and (most)strings
//#define VERIFY_POINTERS // comment to not check pointers before trying to use them.  uncomment this for easier porting
//#define PBSS_BLOCKER // overrides the PBSS cleaner and turns it into a blocker
//#define MASS_MURDER // comment to disable mass murder. note: requires PBSS_CLEANER to be defined
//#define FORCE_INFOBAR
//#define USE_RANDOMIZATION

#if HVERSION == 1
//#define NORMAL_VERSION // comment to not use HWBP+winapi hooks
#define X_VERSION       // comment to not use X-method
//#define KILLSTATS // comment to not compile killstats code
#define LOG             // comment to disable log code
#define USE_LDE         // comment to not use LDE, and instead use XDE
#define PBSS_CLEANER    // comment to disable pbss cleaner
#define VISUALS         // comment to disable visuals(and some other features as well)
//#define CHAMS // comment to disable chams
//#define PB_LOGGER
//#define GHOST_CVARS // comment to not compile ghost cvars
//#define COMPILE_DEBUG // comment to disable cvar_Debug
//#define H47 // comment to disable new aimbot core
#elif HVERSION == 2
#define NORMAL_VERSION    // comment to not use HWBP+winapi hooks
//#define X_VERSION // comment to not use X-method
#define KILLSTATS         // comment to not compile killstats code
#define LOG               // comment to disable log code
#define USE_LDE           // comment to not use LDE, and instead use XDE
#define PBSS_CLEANER      // comment to disable pbss cleaner
#define VISUALS           // comment to disable visuals(and some other features as well)
#define CHAMS             // comment to disable chams
//#define PB_LOGGER
#define GHOST_CVARS       // comment to not compile ghost cvars
//#define COMPILE_DEBUG // comment to disable cvar_Debug
#elif HVERSION == 3
//#define NORMAL_VERSION // comment to not use HWBP+winapi hooks
#define X_VERSION         // comment to not use X-method
//#define KILLSTATS // comment to not compile killstats code
//#define LOG // comment to disable log code
//#define USE_LDE // comment to not use LDE, and instead use XDE
//#define PBSS_CLEANER // comment to disable pbss cleaner
//#define VISUALS // comment to disable visuals(and some other features as well)
//#define CHAMS // comment to disable chams
//#define PB_LOGGER
//#define GHOST_CVARS // comment to not compile ghost cvars
//#define COMPILE_DEBUG // comment to disable cvar_Debug
//#define H47 // comment to disable new aimbot core
#else
//#define NORMAL_VERSION // comment to not use HWBP+winapi hooks
//#define X_VERSION // comment to not use X-method
#define KILLSTATS       // comment to not compile killstats code
#define LOG             // comment to disable log code
#define USE_LDE         // comment to not use LDE, and instead use XDE
#define PBSS_CLEANER    // comment to disable pbss cleaner
#define VISUALS         // comment to disable visuals(and some other features as well)
#define CHAMS           // comment to disable chams
//#define PB_LOGGER
//#define GHOST_CVARS // comment to not compile ghost cvars
//#define COMPILE_DEBUG // comment to disable cvar_Debug
#endif

#define PRNG        2 // 1 = x32 Galois LSFR, 2 = x32 LCG, 3 = x16 Fibonacci LSFR // TODO: make some script to find and change this to a random byte.  it should be included in the regular build events.
#define CRYPT_KEY          0xBF // TODO: make some script to find and change this to a random byte.  it should be included in the regular build events.


#define DIST_DIVISOR       36.0f // TODO: move these to hClasses
#define TAG_TYPE           1
#define FONT_TYPE          1
#define SHADER_TYPE        7
#define MAX_PLAYERS        64
#define CMD_BACKUP         0x7f

#define CONFIG_FILENAME    DS("CODFISH.jpg")

#ifdef H47
#define HACK_NAME          DS("\"47\"")
#else
#define HACK_NAME          DS("codfish")
#endif // H47

#define SS_SAFETY_TIME     2500



#ifdef ANTI_CRASH
#define ANTI_CRASH_BEGIN    __try{
#define ANTI_CRASH_END \
   }                   \
   __except (1) {}
#define ANTI_CRASH_END_RETNULL \
   }                   \
   __except (1) {return NULL;}
#else
#define ANTI_CRASH_BEGIN
#define ANTI_CRASH_END
#define ANTI_CRASH_END_RETNULL
#endif

#define CInline     inline
#define NOInline    __declspec(noinline)
#define CFInline    __forceinline

#endif
