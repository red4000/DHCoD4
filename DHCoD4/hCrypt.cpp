#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hDefines.h"
#include "hCrypt.h"

unsigned long sDw, sLen;

#if PRNG == 1
/* x32 Galois LSFR */
unsigned int rseed = 1;
unsigned int hrand()
{
   // taps: 32 31 29 1; characteristic polynomial: x^32 + x^31 + x^29 + x + 1
   rseed = (rseed >> 1u) ^ (-(rseed & 1u) & 0xD0000001u);
   return(rseed);
}


#elif PRNG == 2
/* x32 LCG */
unsigned int rseed = 0;
#ifdef RAND_MAX
#undef RAND_MAX
#endif
#define RAND_MAX    ((1U << 31) - 1)
unsigned int hrand()
{
   return(rseed = (rseed * 1103515245      // a
                   + 12345)                // c
                  & RAND_MAX);             // m
}


#else
/* x16 Fibonacci LSFR */
unsigned short rseed = 0xACE1u;
unsigned int hrand()
{
   // taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1
   unsigned short sRand[2];

   for (unsigned int i = 0; i < 2; ++i)
   {
      unsigned short bit = ((rseed >> 0) ^ (rseed >> 2) ^ (rseed >> 3) ^ (rseed >> 5)) & 1;
      rseed    = (rseed >> 1) | (bit << 15);
      sRand[i] = rseed;
   }
   return((unsigned int)((sRand[0] << 16) | ((sRand[1]) & 0xffff)));
}


#endif


void hsrand(unsigned int x)
{
   rseed = x;
}


void Process(unsigned char *in, unsigned int key, unsigned int len, unsigned char (__cdecl *f)(unsigned char))
{
   hsrand(key);
   for (unsigned int i = 0; i < len; ++i)
   {
#if USE_RANDOMIZATION
      in[i] = f(in[i]);
#endif
   }
}


unsigned char randXorTransform(unsigned char c)
{
   return(c ^ (unsigned char)(hrand() % 0xff));
}


unsigned char randTransform(unsigned char c)
{
   c;
   return((unsigned char)(hrand() % 0xff));
}


NOInline void Crypt(unsigned char *in, unsigned int key, unsigned int len)
{
   Process(in, key, len, randXorTransform);
}


NOInline void Randomize(unsigned char *in, unsigned int len)
{
   Process(in, rseed, len, randTransform);
}


unsigned int RandomizeMemoryThread(mem_t *m)
{
   // TODO: make it possible to specify a thread id or handle to wait on instead of a time.
   Sleep(m->d);
   FORCE_RANDOMIZE(m->p, m->s);
   RANDOMIZE(m, sizeof(mem_t));
   delete m;
   return(0);
}


void RandomizeMemory(unsigned char *in, unsigned int len, unsigned int delay)
{
   mem_t *pm = new mem_t;

   pm->p = in;
   pm->s = len;
   pm->d = delay;
   CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RandomizeMemoryThread, (LPVOID)pm, 0, 0);
}


void EncryptEx(unsigned char *in, unsigned int key, unsigned int len)
{
   key %= 255;
   for (unsigned int i = 0; i < len; ++i)
   {
      in[i] = _STATIC_ENCRYPTCHAR(in[i], (unsigned char)key);
   }
}


void DecryptEx(unsigned char *in, unsigned int key, unsigned int len)
{
   key %= 255;
   for (unsigned int i = 0; i < len; ++i)
   {
      in[i] = _STATIC_DECRYPTCHAR(in[i], (unsigned char)key);
   }
}
