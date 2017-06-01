#include <windows.h>
#include <stdio.h>
#include "WELL44497a_new.h"

CWELLRNG44497 rng;
char alphabet[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

int SetGUID(char *key) // changes key to 64 byte GUID
{
	DWORD setGUID = 0x4699A0;
	int result;

	__asm
	{
		push key
		call [setGUID]
		add esp, 4
		mov result, eax
	}

	return result;
}

void GenerateKey(char *key)
{
	int key5 = 0;

	for (size_t i = 0; i < 16; i++)
	{
		key[i] = alphabet[rng.WELLRNG44497(&rng) % 36];
		key5 ^= key[i];

		for (size_t j = 8; j > 0; --j)
		{
			if (key5 & 1)
			{
				key5 ^= 0x14002;
			}

			key5 >>= 1;
		}
	}

	sprintf(&key[16], "%04X", key5);

	key[20] = 0;
}

int IsKeyValid(char *key)
{
	char  key5[5];
	DWORD checkKey = 0x4712E0;
	int   valid;

	strcpy(key5, &key[16]);
	key[16] = 0;

	__asm
	{
		mov edi, key
		lea esi, [edi + 16]
		call [checkKey]
		mov [valid], eax
	}

	strcpy(&key[16], key5);

	return valid;
}

DWORD WINAPI BlockRcon(DWORD)
{
	char key[65];
	
	unsigned int prngState[1391];

	srand(GetTickCount());

	for (size_t i = 0; i < 1391; i++)
	{
		prngState[i] = rand() ^ (rand() << 16);
	}

	rng.InitWELLRNG44497(prngState);

	for (size_t i = 0; i < 3000; i++)
	{
		rng.WELLRNG44497(&rng);
	}

	while (1)
	{
		GenerateKey(key);

		if (IsKeyValid(key))
		{
			/*
			FILE *f = fopen("keys.txt", "a");
			key[20] = '\n';
			key[21] = 0;
			fwrite((void*)key, 21, 1, f);
			fclose(f);
			*/
			SetGUID(key);
		}

		Sleep(5);
	}

	return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)BlockRcon, 0, 0, 0);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

