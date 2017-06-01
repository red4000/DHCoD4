#ifndef CPE_H
#define CPE_H

class CPE
{
public:
   CPE(void *ptr)
   {
      pDosHeader = NULL;
      pNtHeader  = NULL;

      pDosHeader = (PIMAGE_DOS_HEADER)ptr;
      if (pDosHeader->e_magic == IMAGE_DOS_SIGNATURE)
      {
         pNtHeader = (PIMAGE_NT_HEADERS)((LONG)pDosHeader + pDosHeader->e_lfanew - 1);
         if (pNtHeader->Signature == IMAGE_NT_SIGNATURE)
         {
            // do fun stuff
         }
         else
         {
            pNtHeader = NULL;
         }
      }
      else
      {
         pDosHeader = NULL;
      }
   }

   ~CPE()
   {
   }

   const bool IsValid()
   {
      return(pDosHeader && pNtHeader);
   }

   PIMAGE_DOS_HEADER pDosHeader;
   PIMAGE_NT_HEADERS pNtHeader;
};

#endif
