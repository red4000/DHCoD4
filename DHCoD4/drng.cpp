#include <vector>
#include <string.h>
#include <stdlib.h>
#include "drng.h"

void GenerateSBox(unsigned char *sbox)
{
	std::vector<unsigned char> unused;

	for (size_t i = 0; i < 256; i++)
	{
		unused.push_back((unsigned char)i);
	}

	for (size_t i = 0; i < 256; i++)
	{
		size_t j = rand() % unused.size();
		sbox[i]  = unused[j];
		unused.erase(unused.begin() + j);
	}
}

/*
void CalculateKeccakPeriod()
{
	unsigned int buffer[1400];
	unsigned int firstPermutation[1400];

	memset(buffer, 0, sizeof(buffer));

	KeccakPermutation((unsigned char*)buffer);

	memcpy(firstPermutation, buffer, sizeof(buffer));

	size_t i = 0;

	while (i < 0xFFFFFFFF)
	{
		i++;
		
		KeccakPermutation((unsigned char*)buffer);

		if (0 == memcmp(buffer, firstPermutation, sizeof(buffer)))
		{
			break;
		}
	}

	printf("Keccak 1400 period is %i\n", i);
}
*/

void DRNGRandomize(unsigned char *buffer, size_t len, size_t rounds)
{
	unsigned char sbox[256];

	for (size_t h = 0; h < rounds; h++)
	{
		for (size_t i = 0, j = 0; i < len; i++)
		{
			if (0 == j)
			{
				GenerateSBox(sbox);
			}

			j++;

			if (256 == j)
			{
				j = 0;
			}

			buffer[i] ^= sbox[j];
		}
	}
}
