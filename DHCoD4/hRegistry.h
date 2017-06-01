#ifndef HREGISTRY_H
#define HREGISTRY_H

#include <windows.h>
#include <string>

bool ReadRegistryBool(LPCSTR lpKeyName);
LPCSTR ReadRegistryString(LPCSTR lpKeyName);

void WriteRegistryBool(LPCSTR lpKeyName, bool bValue);
void WriteRegistryString(LPCSTR lpKeyName, LPCSTR data);

#endif
