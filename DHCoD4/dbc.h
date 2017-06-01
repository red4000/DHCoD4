/*
 Block Cipher

DBC uses DSC internally, essentially duplexing it in order to randomize DSC's internal state. DBC can work with almost any
4-byte aligned block size.
*/

#ifndef DBC_H
#define DBC_H

typedef struct
{
	size_t        blockSize, rounds;
	unsigned int *state;
} DBCState;

void DBCInitialize(DBCState *state, unsigned int *key, size_t len, size_t blockSize, size_t rounds);
void DBCEatData(DBCState *state, unsigned int *data, size_t len);
void DBCEncrypt(DBCState *state, unsigned int *data, size_t len);
void DBCDecrypt(DBCState *state, unsigned int *data, size_t len);

#endif
