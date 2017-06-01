#pragma warning ( disable : 4530 )
#include "hRegistry.h"

// TODO: regcreatekeyex fails for some reason now.

bool ReadRegistryBool(LPCSTR lpKeyName)
{
   LPCSTR val = ReadRegistryString(lpKeyName);
   bool r=false;

   if (!strcmp(val, "1"))
   {
      r=true;
   }
   free((void*)val);
   return(r);
}


LPCSTR ReadRegistryString(LPCSTR lpKeyName)
{
   HKEY  hkey;
   DWORD dwDisposition;

   DWORD dwType, dwSize;

   if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\DH\\"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_EXECUTE, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
   {
      char *szTmp=(char*)malloc(1024);
      dwType = REG_SZ;
      dwSize = 1024;
      RegQueryValueExA(hkey, lpKeyName, NULL, &dwType, (PBYTE)&szTmp, &dwSize);
      RegCloseKey(hkey);

	  return szTmp;
   }
   return(0);
}


void WriteRegistryBool(LPCSTR lpKeyName, bool bValue)
{
   LPCSTR szData;

   if (bValue)
   {
      szData = "1";
   }
   else
   {
      szData = "0";
   }
   WriteRegistryString(lpKeyName, szData);
}


void WriteRegistryString(LPCSTR lpKeyName, LPCSTR data)
{
   HKEY  hkey;
   DWORD dwDisposition;

   DWORD dwType, dwSize;

   if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\DH\\"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_EXECUTE, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
   {
      dwType = REG_SZ;
      dwSize = (strlen(data) + 1);
      RegSetValueExA(hkey, lpKeyName, 0, dwType, (PBYTE)data, dwSize);

      RegCloseKey(hkey);
   }
}
