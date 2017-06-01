#ifndef CCLOAK_H
#define CCLOAK_H

#define STATUS_UNSUCCESSFUL    0xC0000001;
#define RELATIVE_JMP(source, dest)    (DWORD)(dest) - (DWORD)((DWORD)source + 5)

/*!
 * \brief Unlinks an entry from a doubly linked list
 * \author Darawk
 */
#define UNLINK(x)                \
   (x).Blink->Flink = (x).Flink; \
   (x).Flink->Blink = (x).Blink;

#include "hCrypt.h"


typedef struct LinkedModule
{
   DWORD        dwBaseAddress;
   DWORD        dwEndAddress;
   LinkedModule *Next;

   LinkedModule(DWORD dwBase, DWORD dwEnd)
   {
      dwBaseAddress = dwBase;
      dwEndAddress  = dwEnd;
      Next          = NULL;
   }

   LinkedModule()
   {
      dwBaseAddress = 0;
      dwEndAddress  = 0;
      Next          = NULL;
   }
} LINKED_MODULE, *PLINKED_MODULE;

typedef enum _MEMORY_INFORMATION_CLASS
{
   MemoryBasicInformation
} MEMORY_INFORMATION_CLASS, *PMEMORY_INFORMATION_CLASS;

typedef struct _UNICODE_STRING
{
   USHORT Length;
   USHORT MaximumLength;
   PWSTR  Buffer;
} CUNICODE_STRING, *PCUNICODE_STRING;

typedef struct _ModuleInfoNode
{
   LIST_ENTRY      LoadOrder;
   LIST_ENTRY      InitOrder;
   LIST_ENTRY      MemoryOrder;
   HMODULE         baseAddress;         //	Base address AKA module handle
   unsigned long   entryPoint;
   unsigned int    size;                //	Size of the modules image
   CUNICODE_STRING fullPath;
   CUNICODE_STRING name;
   unsigned long   flags;
   unsigned short  LoadCount;
   unsigned short  TlsIndex;
   LIST_ENTRY      HashTable;   //	A linked list of any other modules that have the same first letter
   unsigned long   timestamp;
} ModuleInfoNode, *pModuleInfoNode;

typedef struct _ProcessModuleInfo
{
   unsigned int size;                           //	Size of a ModuleInfo node?
   unsigned int initialized;
   HANDLE       SsHandle;
   LIST_ENTRY   LoadOrder;
   LIST_ENTRY   InitOrder;
   LIST_ENTRY   MemoryOrder;
} ProcessModuleInfo, *pProcessModuleInfo;

MFUNC(bool, CloakDll(HMODULE hMod));

#endif
