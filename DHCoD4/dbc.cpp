#include <stdlib.h>
#include <string.h>
#include "dbc.h"
#include "dsc.h"
#include "KeccakF-1600-interface.h"

// make the block cipher basically just modify the input data in order to increase it's entropy and then XOR it with the key.
// need to implement dynamic huffman compression, next...

void DBCInitialize(DBCState *state, unsigned int *key, size_t len, size_t blockSize, size_t rounds)
{
	state->rounds    = rounds;
	state->blockSize = blockSize;
	state->state     = (unsigned int *)malloc(blockSize * sizeof(unsigned int));
}

void DBCEatData(DBCState *state, unsigned int *data, size_t len)
{
}

void DBCEncrypt(DBCState *state, unsigned int *data, size_t len, size_t rounds)
{

}

void DBCDecrypt(DBCState *state, unsigned int *data, size_t len, size_t rounds)
{

}
