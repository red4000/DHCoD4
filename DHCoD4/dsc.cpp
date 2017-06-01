#include <vector>
#include <stdlib.h>
#include <string.h>
#include "dsc.h"
#include "WELL44497a_new.h"
#include "KeccakF-1600-interface.h"

void DSCExpandData(DSCState *state, unsigned int *data, size_t len);

void DSCInitialize(DSCState *state, unsigned int *key, size_t len, unsigned int rounds)
{
	unsigned int prngState[1391];

	memset(state, 0, sizeof(DSCState));
	memset(prngState, 0, sizeof(prngState));

	DSCExpandData(state, key, len);

	//srand(state->buffer[0] ^ state->buffer[1]);

	for (size_t i = 0; i < 1391; i++)
	{
		//prngState[i] = state->buffer[i] ^ (unsigned int)rand() ^ (unsigned int)(rand() << 16
		prngState[i] = state->buffer[i];
	}

	state->r.InitWELLRNG44497(prngState);

	state->rounds = rounds;
}

void DSCExpandData(DSCState *state, unsigned int *data, size_t len)
{
	memset(state->buffer, 0, sizeof(state->buffer));

	KeccakPermutation((unsigned char*)state->buffer);
	
	for (size_t i = 0; i < len; i++)
	{
		state->buffer[i] ^= data[i];
	}
	
	KeccakPermutation((unsigned char*)state->buffer);

	for (size_t i = 50; i < 1400; i += 50)
	{
		KeccakPermutation((unsigned char*)&state->buffer[i]);
		KeccakAbsorb((unsigned char*)&state->buffer[i], (unsigned char*)&state->buffer[i - 50], 25);
	}
}

void DSCEatData(DSCState *state, unsigned int *data, size_t len)
{
	DSCExpandData(state, data, len);
	
	for (size_t i = 0; i < 1391; i++)
	{
		state->r.STATE[i] ^= state->buffer[i];
	}
}

void DSCSubstitute(DSCState *state, unsigned int *data, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		/*
		std::vector<unsigned char> unused;

		for (size_t i = 0; i < 256; i++)
		{
			unused.push_back((unsigned char)i);
		}

		unsigned char sbox[256];

		for (size_t i = 0; i < 256; i++)
		{
			size_t j = state->r.WELLRNG44497(&state->r) % unused.size();
			sbox[i]  = unused[j];
			unused.erase(unused.begin() + j);
		}

#if 0
		unsigned int swapped[256];

		memset(swapped, 0, sizeof(swapped));

		for (size_t i = 0; i < 256; i++)
		{
			char c = sbox[i];

			if (0 == swapped[c])
			{
				sbox[c]    = i;
				sbox[i]    = c;
				swapped[c] = 1;
			}
		}
#endif

		// sbox is successfully generated, but it is not used yet. this algorithm can be reused for PBox generation.
		*/
		
		data[i] = state->r.WELLRNG44497(&state->r);
	}

	// NEW ALGORITHM:
	//  FOR EACH byte IN data:
	//    FOR index 0..255:
	//      SBOX[index] = Random()
	//    IF decrypting THEN:
	//      REVERSE SBOX
	//    byte = SBOX[byte]

	// Use similar algorithm for permutation.
}

void DSCPermute(DSCState *state, unsigned int *data, size_t len)
{
	// NOTE: both of these permutation algorithms could be improved

	// Permute state (Random plain changes)
	for (size_t i = 0; i < len; i++)
	{
		unsigned int mask              = state->r.WELLRNG44497(&state->r);
		unsigned int original          = state->r.STATE[i];

		state->r.STATE[i]              = (state->r.STATE[i] & ~mask) | (state->r.STATE[(i + 1) % 1391] & mask);
		state->r.STATE[(i + 1) % 1391] = (state->r.STATE[(i + 1) % 1391] & ~mask) | (original & mask);
	}

#if 0
	// Permute data (simple/weak algorithm)
	unsigned int mask     = state->r.WELLRNG44497(&state->r),
		         i        = state->r.WELLRNG44497(&state->r) % len,
				 i2       = state->r.WELLRNG44497(&state->r) % len,
				 original = data[i];
	
	data[i]  = (data[i] & ~mask) | (data[i2] & mask);
	data[i2] = (data[i2] & ~mask) | (original & mask);
#endif
}

void DSCEncrypt(DSCState *state, unsigned int *data, size_t len)
{
	unsigned int originalData[1391];
	
	memcpy(originalData, data, len * sizeof(int));

	for (size_t i = 0; i < state->rounds; i++)
	{
		DSCSubstitute(state, data, len);
		DSCPermute(state, data, len);
	}
	
	DSCEatData(state, originalData, len);
}

void DSCDecrypt(DSCState *state, unsigned int *data, size_t len)
{
	for (size_t i = 0; i < state->rounds; i++)
	{
		DSCSubstitute(state, data, len);
		DSCPermute(state, data, len);
	}

	DSCEatData(state, data, len);
}
