#ifndef HCRYPT_H
#define HCRYPT_H

#include "hDefines.h"

typedef struct
{
   unsigned char *p;
   unsigned int  s;
   unsigned int  d;
} mem_t;

void Randomize(unsigned char *in, unsigned int len);
unsigned int RandomizeMemoryThread(mem_t *m);
void RandomizeMemory(unsigned char *in, unsigned int len, unsigned int delay);

void hsrand(unsigned int x);
unsigned int hrand();

// TODO: instead of randomization, consider null'ing out to be more secure.  it is possible(unlikely, but possible) that some signateur that could be detected as a hack could be randomly generated.

extern unsigned long sDw, sLen;
#define RANDOMIZE(x, y)            Randomize((unsigned char *)x, (unsigned int)y)
#define RANDOMIZE_STR(x)           sLen = strlen(x) + 1; RANDOMIZE(x, sLen)
#define FORCE_RANDOMIZE(x, y)      VirtualProtect((LPVOID)x, (SIZE_T)y, PAGE_EXECUTE_READWRITE, &sDw); RANDOMIZE(x, y); VirtualProtect((LPVOID)x, (SIZE_T)y, sDw, &sDw)
#define FORCE_RANDOMIZE_STR(x)     sLen = strlen(x) + 1; VirtualProtect((LPVOID)x, (SIZE_T)sLen, PAGE_EXECUTE_READWRITE, &sDw); RANDOMIZE(x, sLen); VirtualProtect((LPVOID)x, (SIZE_T)sLen, sDw, &sDw)
#define SFORCE_RANDOMIZE_STR(x)    if (x) FORCE_RANDOMIZE_STR(x)

#define MFUNC(r, f)                void Begin_ ## f; r f; void End_ ## f
#define MFUNCEX(r, f)                void Begin_ ## f; void End_ ## f
#define MFBEG(r, f)                __declspec(naked)  void Begin_ ## f {}
#define MFEND(r, f)                __declspec(naked) void End_ ## f {}

#ifdef USE_RANDOMIZATION
#define MF(f)                      FORCE_RANDOMIZE(Begin_ ## f, (unsigned long)End_ ## f - (unsigned long)Begin_ ## f)
#define MFEX(f, d)                 RandomizeMemory((unsigned char *)Begin_ ## f, (unsigned long)End_ ## f - (unsigned long)Begin_ ## f, d)
#else
#define MF(f)
#define MFEX(f, d)
#endif

void Crypt(unsigned char *in, unsigned int key, unsigned int len);

#define ENCRYPT(x, y, z)    Crypt((unsigned char *)x, (unsigned int)y, (unsigned int)z)
#define DECRYPT(x, y, z)    Encrypt

void EncryptEx(unsigned char *in, unsigned int key, unsigned int len);
void DecryptEx(unsigned char *in, unsigned int key, unsigned int len);

#define ENCRYPTEX(x, y, z)          EncryptEx((unsigned char *)(x), (unsigned int)(y), (unsigned int)(z))
#define DECRYPTEX(x, y, z)          DecryptEx((unsigned char *)(x), (unsigned int)(y), (unsigned int)(z))
#define ENCRYPTSTREX(x, z)          EncryptEx((unsigned char *)&x, (unsigned int)_SEK, (unsigned int)z)
#define DECRYPTSTREX(x, z)          DecryptEx((unsigned char *)&x, (unsigned int)_SEK, (unsigned int)z)
#define FORCE_ENCRYPTEX(x, y, z)    VirtualProtect((LPVOID)x, (SIZE_T)z, PAGE_EXECUTE_READWRITE, &sDw); ENCRYPTEX(x, y, z); VirtualProtect((LPVOID)x, (SIZE_T)z, sDw, &sDw)
#define FORCE_DECRYPTEX(x, y, z)    VirtualProtect((LPVOID)x, (SIZE_T)z, PAGE_EXECUTE_READWRITE, &sDw); DECRYPTEX(x, y, z); VirtualProtect((LPVOID)x, (SIZE_T)z, sDw, &sDw)

// ? en/de crypt string functions - automatically store the length of string in a map with the pointer to string?

#define _STATIC_ENCRYPTCHAR(c, k)    (((c) ^ (k)) + (~k))
#define _STATIC_DECRYPTCHAR(c, k)    (((c) - (~k)) ^ (k))
#define _SEC(c, k)                   _STATIC_ENCRYPTCHAR((unsigned char)(c), (unsigned char)(k))
#define _SDC(c, k)                   _STATIC_DECRYPTCHAR((unsigned char)(c), (unsigned char)(k))
#define _SEK    (CRYPT_KEY)
#define _SECK(c)                     _SEC(c, _SEK)
#define _SDCK(c)                     _SDC(c, _SEK)

#define C(c)                         _SECK(c)

#define ENCSTR1(s1)                  _SECK(s1), _SECK(0)
#define DECSTR1(s1)                  _SDCK(s1), _SDCK(0)

#endif
