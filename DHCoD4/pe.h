#ifndef PE_H
#define PE_H

#define BoundsCheckAddress(Image, Address, Size) \
	(((ULONG_PTR)(Address) >= (ULONG_PTR)(Image)->BaseAddress)        && \
	 ((ULONG_PTR)(Address) <  (ULONG_PTR)(Image)->EndAddress)         && \
	 ((ULONG_PTR)(Address) + Size >= (ULONG_PTR)(Image)->BaseAddress) && \
	 ((ULONG_PTR)(Address) + Size <= (ULONG_PTR)(Image)->EndAddress))

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

typedef struct _PE_IMAGE
{
	HANDLE                FileHandle;
	HANDLE                FileMappingHandle;
	ULONG                 FileSize;
	PCHAR                 BaseAddress;
	PCHAR                 EndAddress;
	PIMAGE_DOS_HEADER     DosHeader;
	PIMAGE_NT_HEADERS     NtHeaders;
	PIMAGE_SECTION_HEADER FirstSection;
} PE_IMAGE, *PPE_IMAGE;

#define PE_OPEN_FLAG_READONLY  0x0000
#define PE_OPEN_FLAG_READWRITE 0x1000

PPE_IMAGE PeOpen(LPCSTR Path, ULONG Flags);
PPE_IMAGE PeOpenLoaded(PIMAGE_DOS_HEADER DosHeader);
DWORD PeUnhookIAT(PPE_IMAGE Image);
BOOL PeVerify(PPE_IMAGE Image);
VOID PeSpliceImports(PPE_IMAGE Image);
VOID PeWipe(PPE_IMAGE Image);
VOID PeSync(PPE_IMAGE Image);
VOID PeClose(PPE_IMAGE Image);

PCHAR PeImageRvaToVa(PPE_IMAGE Image, ULONG Rva);
PIMAGE_SECTION_HEADER PeGetSectionHeader(PPE_IMAGE Image, ULONG Rva);

#define PE_BASE_RELOC_ENUM_SECURITY 0x1

BOOL PeRemap(PPE_IMAGE Image);

#endif
