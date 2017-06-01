#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include "pe.h"

BOOL WINAPI DllMain(HMODULE hinstModule, DWORD dwReason, LPVOID lpvReserved);

#define IMAGE_SIZEOF_BASE_RELOCATION 8
#define MakePtr(cast, ptr, addValue) ((cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue)))
#define MakeDelta(cast, x, y)        ((cast)((DWORD_PTR)(x) - (DWORD_PTR)(y)))

PPE_IMAGE PeOpen(LPCSTR Path, ULONG Flags)
{
	PPE_IMAGE Image    = (PPE_IMAGE)malloc(sizeof(PE_IMAGE));
	BOOLEAN   Success  = FALSE;
	BOOLEAN   Writable = (BOOLEAN)((Flags & PE_OPEN_FLAG_READWRITE) != 0);

	do
	{
		if (NULL == Image)
		{
			break;
		}

		ZeroMemory(Image, sizeof(PE_IMAGE));

		if ((Image->FileHandle = CreateFileA(
				Path,
				(Writable) ? GENERIC_READ|GENERIC_WRITE : GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL)) == INVALID_HANDLE_VALUE)
		{
			break;
		}
		
		Image->FileSize = GetFileSize(Image->FileHandle, NULL);

		if ((Image->FileMappingHandle = CreateFileMappingA(
				Image->FileHandle,
				NULL,
				(Writable) ? PAGE_READWRITE : PAGE_READONLY,
				0,
				0,
				NULL)) == NULL)
		{
			break;
		}

		if ((Image->BaseAddress = (PCHAR)MapViewOfFile(
				Image->FileMappingHandle,
				(Writable) ? FILE_MAP_WRITE : FILE_MAP_READ,
				0,
				0,
				0)) == NULL)
		{
			break;
		}

		Image->EndAddress = Image->BaseAddress + Image->FileSize;
		Image->DosHeader  = (PIMAGE_DOS_HEADER)Image->BaseAddress;

		if (!BoundsCheckAddress(
				Image,
				Image->DosHeader,
				sizeof(IMAGE_DOS_HEADER)))
		{
			break;
		}

		if (Image->DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			break;
		}

		Image->NtHeaders = (PIMAGE_NT_HEADERS)(Image->BaseAddress + Image->DosHeader->e_lfanew);

		if (!BoundsCheckAddress(
				Image, 
				Image->NtHeaders, 
				sizeof(IMAGE_NT_HEADERS)))
		{
			break;
		}

		if (Image->NtHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			break;
		}

		Image->FirstSection = IMAGE_FIRST_SECTION(Image->NtHeaders);
		
		if (!BoundsCheckAddress(
				Image,
				Image->FirstSection,
				sizeof(IMAGE_SECTION_HEADER) * Image->NtHeaders->FileHeader.NumberOfSections))
		{
			break;
		}

		Success = TRUE;

	} while (0);

	if (FALSE == Success)
	{
		if (NULL != Image)
		{
			PeClose(Image);

			Image = NULL;
		}
	}

	return Image;
}

PPE_IMAGE PeOpenLoaded(PIMAGE_DOS_HEADER DosHeader)
{
	PPE_IMAGE Image = (PPE_IMAGE)malloc(sizeof(PE_IMAGE));

	if (NULL == Image)
	{
		return NULL;
	}

	ZeroMemory(Image, sizeof(PE_IMAGE));

	Image->BaseAddress = (PCHAR)DosHeader;
	Image->DosHeader   = (PIMAGE_DOS_HEADER)Image->BaseAddress;

	if (IMAGE_DOS_SIGNATURE != Image->DosHeader->e_magic)
	{
		return NULL;
	}

	Image->NtHeaders = (PIMAGE_NT_HEADERS)(Image->BaseAddress + Image->DosHeader->e_lfanew);

	if (IMAGE_NT_SIGNATURE != Image->NtHeaders->Signature)
	{
		return NULL;
	}

	Image->FirstSection = IMAGE_FIRST_SECTION(Image->NtHeaders);
	Image->EndAddress   = (PCHAR)(((DWORD)Image->BaseAddress) + Image->NtHeaders->OptionalHeader.SizeOfImage);

	return Image;
}

DWORD PeUnhookIAT(PPE_IMAGE Image)
{
	DWORD fixedImports = 0;

	if (Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress && Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
		
		pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		
		while (NULL != pImportDesc->OriginalFirstThunk ||
			   NULL != pImportDesc->FirstThunk)
		{
			DWORD rvaINT = pImportDesc->OriginalFirstThunk;
			DWORD rvaIAT = pImportDesc->FirstThunk;

			if (NULL == rvaINT)
			{
				rvaINT = rvaIAT;
			}

			PIMAGE_THUNK_DATA pINT = MakePtr(PIMAGE_THUNK_DATA, rvaINT, Image->BaseAddress);
			PIMAGE_THUNK_DATA pIAT = MakePtr(PIMAGE_THUNK_DATA, rvaIAT, Image->BaseAddress);

			PIMAGE_IMPORT_BY_NAME pOrdinalName;

			HMODULE module = LoadLibraryA((char*)(((DWORD)Image->BaseAddress) + pImportDesc->Name));

			if (NULL == module)
			{
				return FALSE;
			}

			PPE_IMAGE image = PeOpenLoaded((PIMAGE_DOS_HEADER)module);

			while (NULL != pIAT->u1.AddressOfData &&
				   NULL != pINT->u1.AddressOfData)
			{
				pOrdinalName    = MakePtr(PIMAGE_IMPORT_BY_NAME, pINT->u1.AddressOfData, Image->BaseAddress);
				DWORD oldImport = pIAT->u1.Function;

				if (pINT->u1.AddressOfData & 0x80000000)
				{
					pIAT->u1.Function = (DWORD)GetProcAddress(module, (char*)((DWORD)pINT->u1.AddressOfData & 0x7FFFFFFF));
				}
				else
				{
					pIAT->u1.Function = (DWORD)GetProcAddress(module, MakePtr(char*, pINT->u1.AddressOfData + 2, Image->BaseAddress));
				}

				if (oldImport != pIAT->u1.Function)
				{
					fixedImports++;
				}
		
				pINT++;
				pIAT++;
			}

			free(image);

			pImportDesc++;
		}
	}

	return fixedImports;
}

BOOL PeVerify(PPE_IMAGE Image)
{
	IMAGE_BASE_RELOCATION *reloc = MakePtr(IMAGE_BASE_RELOCATION*, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	
	for (ULONG nBytes = 0;
		nBytes < Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size &&
		NULL != reloc->VirtualAddress;)
	{
		ULONG *locBase  = MakePtr(ULONG*, Image->BaseAddress, reloc->VirtualAddress);
		ULONG numRelocs = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(USHORT);

		USHORT *locData = (USHORT*)(reloc + 1);

		for (ULONG i = 0; i < numRelocs; i++, locData++)   
		{
			if (((*locData >> 12) & IMAGE_REL_BASED_HIGHLOW))
			{
				if ((*MakePtr(ULONG*, locBase, (*locData & 0x0FFF))) > ((ULONG)Image->BaseAddress) &&
					(*MakePtr(ULONG*, locBase, (*locData & 0x0FFF))) <= ((ULONG)Image->EndAddress))
				{
					continue;
				}
				else
				{
					return FALSE;
				}
			}
		}

		nBytes += reloc->SizeOfBlock;
		reloc   = (IMAGE_BASE_RELOCATION *)locData;
	}
	
	if (Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress && Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
		
		pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		
		while (NULL != pImportDesc->OriginalFirstThunk ||
			   NULL != pImportDesc->FirstThunk)
		{
			DWORD rvaINT = pImportDesc->OriginalFirstThunk;
			DWORD rvaIAT = pImportDesc->FirstThunk;

			if (NULL == rvaINT)
			{
				rvaINT = rvaIAT;
			}

			PIMAGE_THUNK_DATA pINT = MakePtr(PIMAGE_THUNK_DATA, rvaINT, Image->BaseAddress);
			PIMAGE_THUNK_DATA pIAT = MakePtr(PIMAGE_THUNK_DATA, rvaIAT, Image->BaseAddress);

			PIMAGE_IMPORT_BY_NAME pOrdinalName;

			HMODULE module  = LoadLibraryA((char*)(((DWORD)Image->BaseAddress) + pImportDesc->Name));

			if (NULL == module)
			{
				return FALSE;
			}

			PPE_IMAGE image = PeOpenLoaded((PIMAGE_DOS_HEADER)module);

			while (1)
			{
				if (NULL == pIAT->u1.AddressOfData ||
					NULL == pINT->u1.AddressOfData)
				{
					break;
				}

				pOrdinalName = MakePtr(PIMAGE_IMPORT_BY_NAME, pINT->u1.AddressOfData, Image->BaseAddress);

				if (pIAT->u1.Function < (DWORD)image->BaseAddress ||
					pIAT->u1.Function > (DWORD)image->EndAddress)
				{
					free(image);
					return FALSE;
					//simple code to unhook import:
					//pIAT->u1.Function = (DWORD)GetProcAddress(module, (char*)pINT->u1.AddressOfData);
				}
		
				pINT++;
				pIAT++;
			}

			free(image);

			pImportDesc++;
		}
	}

	PIMAGE_SECTION_HEADER Current;
	ULONG                 Index;

	for (Index = 0, Current = Image->FirstSection;
	     Index < Image->NtHeaders->FileHeader.NumberOfSections;
	     Index++, Current++)
	{
		// TODO: verify section/virtual page protection flags
	}

	return TRUE;
}

VOID PeSpliceImports(PPE_IMAGE Image)
{
	// TODO process hack's code and replaces calls to imports with direct calls to the API, or better yet, calls to randomly generated obfuscated trampolines to the API
}

VOID PeWipe(PPE_IMAGE Image)
{
	IMAGE_BASE_RELOCATION *reloc = MakePtr(IMAGE_BASE_RELOCATION*, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

	for (ULONG nBytes = 0;
		nBytes < Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size &&
		NULL != reloc->VirtualAddress;)
	{
		ULONG *locBase = MakePtr(ULONG*, Image->BaseAddress, reloc->VirtualAddress);
		ULONG numRelocs = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(USHORT);

		USHORT *locData = (USHORT*)(reloc + 1);

		for (ULONG i = 0; i < numRelocs; i++, locData++)   
		{
			if (((*locData >> 12) & IMAGE_REL_BASED_HIGHLOW))
			{
				*locData = 0;
			}
		}

		nBytes += reloc->SizeOfBlock;
		
		memset(reloc, 0, sizeof(IMAGE_BASE_RELOCATION));

		reloc = (IMAGE_BASE_RELOCATION *)locData;
	}

	if (Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress && Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
		DWORD                    importsStartRVA;

		importsStartRVA = Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

		if (NULL == importsStartRVA)
		{
			return;
		}

		pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, Image->BaseAddress, importsStartRVA);
		
		while (NULL != pImportDesc->OriginalFirstThunk ||
			   NULL != pImportDesc->FirstThunk)
		{
			DWORD rvaINT = pImportDesc->OriginalFirstThunk;
			DWORD rvaIAT = pImportDesc->FirstThunk;

			if (NULL == rvaINT)
			{
				rvaINT = rvaIAT;
			}

			PIMAGE_THUNK_DATA pINT = MakePtr(PIMAGE_THUNK_DATA, rvaINT, Image->BaseAddress);
			PIMAGE_THUNK_DATA pIAT = MakePtr(PIMAGE_THUNK_DATA, rvaIAT, Image->BaseAddress);

			PIMAGE_IMPORT_BY_NAME pOrdinalName;

			while (NULL != pIAT->u1.AddressOfData &&
				   NULL != pINT->u1.AddressOfData)
			{
				pOrdinalName           = MakePtr(PIMAGE_IMPORT_BY_NAME, pINT->u1.AddressOfData, Image->BaseAddress);
				pINT->u1.AddressOfData = 0;
				pOrdinalName->Hint     = 0;

				memset(pOrdinalName->Name, 0, strlen(pOrdinalName->Name));
		
				pINT++;
				pIAT++;
			}

			memset(pImportDesc, 0, sizeof(PIMAGE_IMPORT_DESCRIPTOR));

			pImportDesc++;
		}
	}
	
	memset(Image->FirstSection, 0, sizeof(IMAGE_SECTION_HEADER) * Image->NtHeaders->FileHeader.NumberOfSections);

	if (Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress && Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size)
	{
		memset(MakePtr(void*, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress), 0, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size);
	}

	if (Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress && Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size)
	{
		memset(MakePtr(void*, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress), 0, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size);
	}

	if (Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress && Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
	{
		memset(MakePtr(void*, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress), 0, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);
	}

	if (Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress && Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size)
	{
		memset(MakePtr(void*, Image->BaseAddress, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress), 0, Image->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size);
	}
	 
	memset(Image->DosHeader, 0, Image->NtHeaders->OptionalHeader.SizeOfHeaders);
	memset(Image->DosHeader, 0, sizeof(IMAGE_DOS_HEADER));
	memset(Image->NtHeaders, 0, sizeof(IMAGE_NT_HEADERS));
	memset(MakePtr(void*, Image->DosHeader, sizeof(IMAGE_DOS_HEADER)), 0, MakeDelta(size_t, Image->NtHeaders, MakePtr(void*, Image->DosHeader, sizeof(IMAGE_DOS_HEADER)))); // junk inserted by M$ linker, and DOS loader
}

VOID PeSync(PPE_IMAGE Image)
{
	// Synchronize the PE image mapping to disk.
	FlushViewOfFile(
			Image->BaseAddress,
			Image->FileSize);
}

VOID PeClose(PPE_IMAGE Image)
{
	if (Image->BaseAddress)
	{
		UnmapViewOfFile(Image->BaseAddress);
	}

	if (Image->FileMappingHandle)
	{
		CloseHandle(Image->FileMappingHandle);
	}

	if (Image->FileHandle &&
	    INVALID_HANDLE_VALUE != Image->FileHandle)
	{
		CloseHandle(Image->FileHandle);
	}

	free(Image);
}

PCHAR PeImageRvaToVa(PPE_IMAGE Image, ULONG Rva)
{
	PIMAGE_SECTION_HEADER Current;
	ULONG                 Index;

	for (Index = 0, Current = Image->FirstSection;
	     Index < Image->NtHeaders->FileHeader.NumberOfSections;
	     Index++, Current++)
	{
		if ((Rva >= (Current->VirtualAddress)) &&
		    (Rva  < (Current->VirtualAddress + Current->Misc.VirtualSize)))
		{
			return Image->BaseAddress + (Rva - Current->VirtualAddress) + Current->PointerToRawData;
		}
	}

	return NULL;
}

PIMAGE_SECTION_HEADER PeGetSectionHeader(PPE_IMAGE Image, ULONG Rva)
{
	PIMAGE_SECTION_HEADER Current;
	ULONG                 Index;

	for (Index = 0, Current = Image->FirstSection;
	     Index < Image->NtHeaders->FileHeader.NumberOfSections;
	     Index++, Current++)
	{
		if ((Rva >= Current->VirtualAddress) &&
		    (Rva  < Current->VirtualAddress + Current->Misc.VirtualSize))
		{
			return Current;
		}
	}

	return NULL;
}

__declspec(naked) void DllCall_stub()
{
	#define STUB_LEN 23

	__asm
	{
		push	0x12345678
		push	1
		push	[esp + 0xC]
		mov		eax, 0x12345678
		call	eax
		xor		eax, eax
		ret
	}
}

BOOL PeRemap(PPE_IMAGE Image)
{
	ULONG displacement     = (rand() % 4096) & ~0xF;

	ULONG_PTR NewImageBase = (ULONG_PTR)VirtualAlloc(NULL,
													 Image->NtHeaders->OptionalHeader.SizeOfImage + displacement, 
													 MEM_COMMIT | MEM_RESERVE,
													 PAGE_EXECUTE_READWRITE);
		
	if (NULL == NewImageBase)
	{
		return FALSE;
	}

	NewImageBase = MakePtr(ULONG_PTR, NewImageBase, displacement);
		
	memcpy((void*)NewImageBase, (void*)Image->BaseAddress, Image->NtHeaders->OptionalHeader.SizeOfImage);

	PPE_IMAGE NewInstance = PeOpenLoaded((PIMAGE_DOS_HEADER)NewImageBase);

	NewInstance->NtHeaders->OptionalHeader.ImageBase = (DWORD)Image->BaseAddress;

	IMAGE_BASE_RELOCATION *reloc = MakePtr(IMAGE_BASE_RELOCATION*, NewImageBase, NewInstance->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	
	for (ULONG nBytes = 0, delta = MakeDelta(ULONG, NewImageBase, NewInstance->NtHeaders->OptionalHeader.ImageBase);
		nBytes < NewInstance->NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;)
	{
		if (NULL == reloc->VirtualAddress)
		{
			break;
		}

		ULONG  *locBase   = MakePtr(ULONG*, NewImageBase, reloc->VirtualAddress);
		ULONG   numRelocs = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(USHORT);
		USHORT *locData   = (USHORT*)(reloc + 1);

		for (ULONG i = 0; i < numRelocs; i++, locData++)   
		{
			if (((*locData >> 12) & IMAGE_REL_BASED_HIGHLOW))
			{
				*MakePtr(ULONG*, locBase, (*locData & 0x0FFF)) += delta;
			}
		}

		nBytes += reloc->SizeOfBlock;
		reloc   = (IMAGE_BASE_RELOCATION *)locData;
	}
	
	NewInstance->NtHeaders->OptionalHeader.ImageBase = (DWORD)NewImageBase;

	DWORD oldprot;
	VirtualProtect((LPVOID)DllCall_stub, STUB_LEN, PAGE_EXECUTE_READWRITE, (PDWORD)&oldprot);

	*MakePtr(DWORD*, DllCall_stub, 12) = MakePtr(DWORD, NewImageBase, MakeDelta(DWORD, DllMain, Image->BaseAddress));

	*MakePtr(DWORD*, DllCall_stub, 1) = (DWORD)Image->BaseAddress;

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DllCall_stub, (LPVOID)NewImageBase, 0, 0);

	return TRUE;
}
