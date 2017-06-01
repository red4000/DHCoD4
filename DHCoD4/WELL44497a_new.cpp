/* ***************************************************************************** */
/* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*                 Makoto Matsumoto, Hiroshima University                        */
/* Notice:         This code can be used freely for personal, academic,          */
/*                 or non-commercial purposes. For commercial purposes,          */
/*                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/*                 A modified "maximally equidistributed" implementations        */
/*                 by Shin Harase, Hiroshima University.                         */
/* ***************************************************************************** */

#include <string.h>
#include "WELL44497a_new.h"

#define W 32
#define DISCARD 15
#define MASKU (0xffffffffU>>(W-DISCARD))
#define MASKL (~MASKU)

#define M1 23
#define M2 481
#define M3 229

#define MAT0POS(t,v) (v^(v>>t))
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define MAT1(v) v
#define MAT2(a,v) ((v & 1U)?((v>>1)^a):(v>>1))
#define MAT3POS(t,v) (v>>t)
#define MAT3NEG(t,v) (v<<(-(t)))
#define MAT4POS(t,b,v) (v ^ ((v>>  t ) & b))
#define MAT4NEG(t,b,v) (v ^ ((v<<(-(t))) & b))
#define MAT5(r,a,ds,dt,v) ((v & dt)?((((v<<r)^(v>>(W-r)))&ds)^a):(((v<<r)^(v>>(W-r)))&ds))
#define MAT7(v) 0

#define V0            r->STATE[r->state_i]
#define VM1Over       r->STATE[r->state_i+M1-R]
#define VM1           r->STATE[r->state_i+M1]
#define VM2Over       r->STATE[r->state_i+M2-R]
#define VM2           r->STATE[r->state_i+M2]
#define VM3Over       r->STATE[r->state_i+M3-R]
#define VM3           r->STATE[r->state_i+M3]
#define Vrm1          r->STATE[r->state_i-1]
#define Vrm1Under     r->STATE[r->state_i+R-1]
#define Vrm2          r->STATE[r->state_i-2]
#define Vrm2Under     r->STATE[r->state_i+R-2]

#define newV0         r->STATE[r->state_i-1]
#define newV0Under    r->STATE[r->state_i-1+R]
#define newV1         r->STATE[r->state_i]
#define newVRm1       r->STATE[r->state_i-2]
#define newVRm1Under  r->STATE[r->state_i-2+R]

/*output transformation parameter*/
#define newVM2Over    r->STATE[r->state_i+M2-R+1]
#define newVM2        r->STATE[r->state_i+M2+1]
#define BITMASK 0x48000000

static unsigned int case_1(CWELLRNG44497 *r);
static unsigned int case_2(CWELLRNG44497 *r);
static unsigned int case_3(CWELLRNG44497 *r);
static unsigned int case_4(CWELLRNG44497 *r);
static unsigned int case_5(CWELLRNG44497 *r);
static unsigned int case_6(CWELLRNG44497 *r);

CWELLRNG44497::CWELLRNG44497()
{
   memset(this, 0, sizeof(this));
   WELLRNG44497 = case_1;
}

void CWELLRNG44497::InitWELLRNG44497(unsigned int *init ){
   int j;
   state_i=0;
   WELLRNG44497 = case_1;
   for(j=0;j<R;j++)
      STATE[j]=init[j];
}

unsigned int case_1(CWELLRNG44497 *r){
  // r->state_i == 0
  r->z0 = (Vrm1Under & MASKL) | (Vrm2Under & MASKU);
  r->z1 = MAT0NEG(-24,V0) ^ MAT0POS(30,VM1);
  r->z2 = MAT0NEG(-10,VM2) ^ MAT3NEG(-26,VM3);
  newV1  = r->z1 ^ r->z2;
  newV0Under = MAT1(r->z0) ^ MAT0POS(20,r->z1) ^ MAT5(9,0xb729fcecU,0xfbffffffU,0x00020000U,r->z2) ^ MAT1(newV1);
  r->state_i = R-1;
  r->WELLRNG44497 = case_3;
  
  return (r->STATE[r->state_i] ^ (newVM2Over & BITMASK));
}

static unsigned int case_2(CWELLRNG44497 *r){
  // r->state_i == 1
  r->z0 = (Vrm1 & MASKL) | (Vrm2Under & MASKU);
  r->z1 = MAT0NEG(-24,V0) ^ MAT0POS(30,VM1);
  r->z2 = MAT0NEG(-10,VM2) ^ MAT3NEG(-26,VM3);
  newV1 = r->z1 ^ r->z2;
  newV0 = MAT1(r->z0) ^ MAT0POS(20,r->z1) ^ MAT5(9,0xb729fcecU,0xfbffffffU,0x00020000U,r->z2) ^ MAT1(newV1);
  r->state_i=0;
  r->WELLRNG44497 = case_1;
  return (r->STATE[r->state_i] ^ (newVM2 & BITMASK)); 
}
static unsigned int case_3(CWELLRNG44497 *r){
  // r->state_i+M1 >= R
  r->z0 = (Vrm1 & MASKL) | (Vrm2 & MASKU);
  r->z1 = MAT0NEG(-24,V0) ^ MAT0POS(30,VM1Over);
  r->z2 = MAT0NEG(-10,VM2Over) ^ MAT3NEG(-26,VM3Over);
  newV1 = r->z1 ^ r->z2;
  newV0 = MAT1(r->z0) ^ MAT0POS(20,r->z1) ^ MAT5(9,0xb729fcecU,0xfbffffffU,0x00020000U,r->z2) ^ MAT1(newV1);
  r->state_i--;
  if(r->state_i+M1<R)
    r->WELLRNG44497 = case_4;
    return (r->STATE[r->state_i] ^ (newVM2Over & BITMASK));
}

static unsigned int case_4(CWELLRNG44497 *r){
  // r->state_i+M3 >= R
  r->z0 = (Vrm1 & MASKL) | (Vrm2 & MASKU);
  r->z1 = MAT0NEG(-24,V0) ^ MAT0POS(30,VM1);
  r->z2 = MAT0NEG(-10,VM2Over) ^ MAT3NEG(-26,VM3Over);
  newV1 = r->z1 ^ r->z2;
  newV0 = MAT1(r->z0) ^ MAT0POS(20,r->z1) ^ MAT5(9,0xb729fcecU,0xfbffffffU,0x00020000U,r->z2) ^ MAT1(newV1);
  r->state_i--;
  if (r->state_i+M3 < R)
    r->WELLRNG44497 = case_5;
	return (r->STATE[r->state_i] ^ (newVM2Over & BITMASK));
}

static unsigned int case_5(CWELLRNG44497 *r){
  //r->state_i+M2 >= R
  r->z0 = (Vrm1 & MASKL) | (Vrm2 & MASKU);
  r->z1 = MAT0NEG(-24,V0) ^ MAT0POS(30,VM1);
  r->z2 = MAT0NEG(-10,VM2Over) ^ MAT3NEG(-26,VM3);
  newV1 = r->z1 ^ r->z2;
  newV0 = MAT1(r->z0) ^ MAT0POS(20,r->z1) ^ MAT5(9,0xb729fcecU,0xfbffffffU,0x00020000U,r->z2) ^ MAT1(newV1);
  r->state_i--;
  if(r->state_i+M2 < R)
    r->WELLRNG44497 = case_6;
	return (r->STATE[r->state_i] ^ (newVM2Over & BITMASK));
}

static unsigned int case_6(CWELLRNG44497 *r){
  // 2 <= r->state_i <= R-M2-1
  r->z0 = (Vrm1 & MASKL) | (Vrm2 & MASKU);
  r->z1 = MAT0NEG(-24,V0) ^ MAT0POS(30,VM1);
  r->z2 = MAT0NEG(-10,VM2) ^ MAT3NEG(-26,VM3);
  newV1 = r->z1 ^ r->z2;
  newV0 = MAT1(r->z0) ^ MAT0POS(20,r->z1) ^ MAT5(9,0xb729fcecU,0xfbffffffU,0x00020000U,r->z2) ^ MAT1(newV1);
  r->state_i--;
  if(r->state_i == 1 )
    r->WELLRNG44497 = case_2;
    return (r->STATE[r->state_i] ^ (newVM2 & BITMASK));
}