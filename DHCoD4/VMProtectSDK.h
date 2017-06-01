#pragma once

#include "hDefines.h"
#ifdef USE_VMPROTECT_SDK

#ifdef _WIN64
#pragma comment(lib, "VMProtectSDK64.lib")
#else
#pragma comment(lib, "VMProtectSDK32.lib")
#endif // _WIN64

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// protection
__declspec(dllimport) void __stdcall VMProtectBegin(const char *);

__declspec(dllimport) void __stdcall VMProtectBeginVirtualization(const char *);

__declspec(dllimport) void __stdcall VMProtectBeginMutation(const char *);

__declspec(dllimport) void __stdcall VMProtectBeginUltra(const char *);

__declspec(dllimport) void __stdcall VMProtectBeginVirtualizationLockByKey(const char *);

__declspec(dllimport) void __stdcall VMProtectBeginUltraLockByKey(const char *);

__declspec(dllimport) void __stdcall VMProtectEnd(void);

__declspec(dllimport) BOOL __stdcall VMProtectIsDebuggerPresent(BOOL);
__declspec(dllimport) BOOL __stdcall VMProtectIsVirtualMachinePresent(void);

__declspec(dllimport) BOOL __stdcall VMProtectIsValidImageCRC(void);

__declspec(dllimport) char *__stdcall VMProtectDecryptStringA(const char *value);

__declspec(dllimport) wchar_t *__stdcall VMProtectDecryptStringW(const wchar_t *value);

// licensing
enum VMProtectSerialStateFlags
{
   SERIAL_STATE_FLAG_CORRUPTED         = 0x00000001,
   SERIAL_STATE_FLAG_INVALID           = 0x00000002,
   SERIAL_STATE_FLAG_BLACKLISTED       = 0x00000004,
   SERIAL_STATE_FLAG_DATE_EXPIRED      = 0x00000008,
   SERIAL_STATE_FLAG_RUNNING_TIME_OVER = 0x00000010,
   SERIAL_STATE_FLAG_BAD_HWID          = 0x00000020,
   SERIAL_STATE_FLAG_MAX_BUILD_EXPIRED = 0x00000040,
};
#pragma pack(push, 1)
typedef struct
{
   WORD wYear;
   BYTE bMonth;
   BYTE bDay;
} VMProtectDate;
typedef struct
{
   INT           nState;                                        // VMProtectSerialStateFlags
   wchar_t       wUserName[256];                                // user name
   wchar_t       wEMail[256];                                   // email
   VMProtectDate dtExpire;                                      // date of serial number expiration
   VMProtectDate dtMaxBuild;                                    // max date of build, that will accept this key
   INT           bRunningTime;                                  // running time in minutes
   BYTE          nUserDataLength;                               // length of user data in bUserData
   BYTE          bUserData[255];                                // up to 255 bytes of user data
} VMProtectSerialNumberData;

#pragma pack(pop)
__declspec(dllimport) INT __stdcall VMProtectSetSerialNumber(const char *SerialNumber);

__declspec(dllimport) INT __stdcall VMProtectGetSerialNumberState();

__declspec(dllimport) BOOL __stdcall VMProtectGetSerialNumberData(VMProtectSerialNumberData *pData, UINT nSize);

__declspec(dllimport) INT __stdcall VMProtectGetCurrentHWID(char *HWID, UINT nSize);

#ifdef __cplusplus
}
#endif // __cplusplus

#else
#define VMProtectBeginUltra(x)
#define VMProtectBeginMutation(x)
#define VMProtectEnd()
#define VMProtectIsDebuggerPresent(x)         FALSE
#define VMProtectIsVirtualMachinePresent()    FALSE
#define VMProtectIsValidImageCRC()            TRUE
#define VMProtectDecryptStringA(x)            x
#endif // USE_VMPROTECT_SDK

#ifdef USE_EXTRA_VMP
#define DS(x)     VMProtectDecryptStringA(x)
#else
#define DS(x)     x
#endif // USE_EXTRA_VMP

#define FDS(x)    VMProtectDecryptStringA(x)
