#ifndef HAPI_H
#define HAPI_H

#include "hTypes.h"
#include "hCrypt.h"

enum
{
   OS_XP,
   OS_VISTA,
};

class Sortable_t;

MFUNC(ADDRESS_PROC, FindPattern(ADDRESS_PROC dwdwAdd, ADDRESS_PROC dwLen, BYTE * bMask, char *szMask, BOOL wipeMaskStr = TRUE));
int randomInt(int min, int max);
char *RandomizeColors(char *str);
void UpToLow(char *str);
void SpaceToUnderscore(char *str);
void SeedRandom();
void appendTrailSlash(char *output);
int random(int min, int max);
int inl_strlen(char *s);
bool houstin_strstr(const char *input, const char *subStr, size_t slStr2);
size_t houstin_strlen(const char *input);
int fTruncate(float f);
char *RandomizeColors(char *str, char col);
char *GetDirectoryFile(char *filename);
char *GetDirectoryFileEx(char *filename);
DWORD WINAPI hPlaySound(char *HPath);
DWORD GetOSType();
BOOL EnumThreads(DWORD *threadIds, DWORD *count, DWORD exclude = 0, DWORD pid = GetCurrentProcessId());
void SuspendThreads(DWORD *threadIds, DWORD count, BOOL resume = FALSE);

#define hnew(t) ((t*)malloc(sizeof(t)))


extern char HackPath[MAX_PATH];


template <class _T> void hsort(_T* a, size_t num) // slow
{
	size_t ci=0; // current index - no need to sort things at the left of the array if it's already sorted
	while(ci < num-1)
	{
		// find extreme(min or max) value
		size_t _mini = ci; // index of extreme
		for(size_t i = ci+1; i < num; ++i)
		{
#if 0
			if(a[i] < a[_mini])
#endif
			if(a[i] > a[_mini])
				_mini = i;
		}

		// continue when extreme value is already in correct position, or remaining values are all equal
		if(_mini == ci)
		{
			ci++;
			continue;
		}

		// swap current index with extreme value
		_T _tmp = a[ci];
		a[ci++] = a[_mini];
		a[_mini] = _tmp;
	}
}


class CPath // just a simple wrapper for GetDirectoryFileEx, that frees the memory in the destructor
{
public:
  CPath(char *path);
  ~CPath();

  char *GetPath();

  char *m_path;
};

#endif
