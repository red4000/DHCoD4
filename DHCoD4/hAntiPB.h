#ifndef HANTIPB_H
#define HANTIPB_H

#include "hTypes.h"

LONG WINAPI ExceptionHandlerPB(EXCEPTION_POINTERS *pExceptionInfo);

void NullMemory(void *m, size_t s);
int randomInt(int min, int max);
char *randomString(int length);
void AntiDvarCheckf(CCVar **ppCurDvar, float defaultVal, float minVal, float maxVal, CCVar** ppoverride=0);
void AntiDvarCheckdw(CCVar **ppCurDvar, DWORD defaultVal, DWORD minVal, DWORD maxVal, CCVar** ppoverride=0);

void AntiPBSSThread(LPVOID);
char *GetAddressStringAsRVA(ADDRESS_PROC ea);
char *GetCommonExceptionStr(DWORD ExceptionCode);

#define ANTI_DVAR_CHECK_DWORD(dvar, dft, min, max)    AntiDvarCheckdw(&real_##dvar, dft, min, max)
#define ANTI_DVAR_CHECK_FLOAT(dvar, dft, min, max)    AntiDvarCheckf(&real_##dvar, dft, min, max)
#define ANTI_DVAR_CHECK_DWORDEX(dvar, dft, min, max, ovr)    AntiDvarCheckdw(&real_##dvar, dft, min, max,(CCVar**)ovr)
#define ANTI_DVAR_CHECK_FLOATEX(dvar, dft, min, max, ovr)    AntiDvarCheckf(&real_##dvar, dft, min, max,(CCVar**)ovr)

extern DWORD dwPBBase;

#endif
