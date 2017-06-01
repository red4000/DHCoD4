/*
 Stream Cipher

DSC uses WELLRNG44497 and Keccak 1600 internally in order to maintain a state of highly random and entropic/mixed up data.
WELLRNG44497 internally mutates it's state, but Keccak-1400 is used as well for increased security. DSC's state size is
44512bits, or 43kbits. DSC should be able to encrypt up to 43kbits of data with complete security, provided that the key
is not compromised, there are no effective cryptanalysis attacks possible for this algorithm, and the key is the same size
or larger than all of the combined input data. DSC can be duplexed or sponged and used for PRNG purposes as well.
*/

#ifndef DSC_H
#define DSC_H

#include "WELL44497a_new.h"

typedef struct
{
	unsigned int  index;
	unsigned int  buffer[1400]; // state size rounded up to 50 int/200 byte boundary, for Keccak 1600
	unsigned int  rounds;
	CWELLRNG44497 r;
} DSCState;

void DSCInitialize(DSCState *state, unsigned int *key, size_t len, unsigned int rounds);
void DSCEatData(DSCState *state, unsigned int *data, size_t len);
void DSCEncrypt(DSCState *state, unsigned int *data, size_t len);
void DSCDecrypt(DSCState *state, unsigned int *data, size_t len);

#endif
