#ifndef HCONFIG_H
#define HCONFIG_H

#include <cstdio>

#include "VMProtectSDK.h"
#include "hCrypt.h"
#include "hAPI.h"

#define MAX_CVAR_OPTIONSTRS    6
#define DISABLED_MAGIC_VALUE -41236.7f

#define DECLARE_COLCVAR(varName)    varName ## R, varName ## G, varName ## B, varName ## A

#define CREATE_COLCVAR(varName, displayName, dr, dg, db, da)                                                \
   strcpy(_tmpstring, displayName);\
   strcat(_tmpstring, " R");\
   varName ## R = Mngr.AddVar(_tmpstring, cvar_misc2Header, true, dr, 0.0f, 255.0f, Invisible, 1.0f, NULL, NULL, NULL, NULL, NULL, true); \
   strcpy(_tmpstring, displayName);\
   strcat(_tmpstring, " G");\
   varName ## G = Mngr.AddVar(_tmpstring, cvar_misc2Header, true, dg, 0.0f, 255.0f, Invisible, 1.0f, NULL, NULL, NULL, NULL, NULL, true); \
   strcpy(_tmpstring, displayName);\
   strcat(_tmpstring, " B");\
   varName ## B = Mngr.AddVar(_tmpstring, cvar_misc2Header, true, db, 0.0f, 255.0f, Invisible, 1.0f, NULL, NULL, NULL, NULL, NULL, true); \
   strcpy(_tmpstring, displayName);\
   strcat(_tmpstring, " A");\
   varName ## A = Mngr.AddVar(_tmpstring, cvar_misc2Header, true, da, 0.0f, 255.0f, Invisible, 1.0f, NULL, NULL, NULL, NULL, NULL, true)

#define DECLARE_VEC3CVAR(varName)    varName ## X, varName ## Y, varName ## Z

#define CREATE_VEC3CVAR(varName, displayName, dx, dy, dz)                                                \
   strcpy(_tmpstring, displayName);\
   strcat(_tmpstring, " X");\
   varName ## R = Mngr.AddVar(_tmpstring, cvar_misc2Header, true, dx, 0.0f, 255.0f, Invisible, 1.0f); \
   strcpy(_tmpstring, displayName);\
   strcat(_tmpstring, " Y");\
   varName ## G = Mngr.AddVar(_tmpstring, cvar_misc2Header, true, dy, 0.0f, 255.0f, Invisible, 1.0f); \
   strcpy(_tmpstring, displayName);\
   strcat(_tmpstring, " Z");\
   varName ## B = Mngr.AddVar(_tmpstring, cvar_misc2Header, true, dz, 0.0f, 255.0f, Invisible, 1.0f);

int fTruncate(float f);

enum CVARTYPE
{
   Toggle,
   Plus,
   Character, // not completely implemented yet
   Invisible,
   Header,
   Completely_Invisible, // only visible if it is set somehow by the user.
   Incognito,            // same as Completely_Invisible only this one is never saved
};

struct CVarManager;

struct CVar
{
   union
   {
      int          iValue;
      unsigned int uiValue;
      float        Value;
      char         *szValue;
      char         chValue;
   };
   char          *Name;
   union
   {
      int   iMin;
      float Min;
   };
   union
   {
      int   iMax;
      float Max;
   };
   union
   {
      int   iStep;
      float Step;
   };
   CVar          *parent;
   CVARTYPE      Type;
   BOOL          bInt;
   char          *Options[MAX_CVAR_OPTIONSTRS];

   CInline void  Increment()
   {
      if (bInt)
      {
		  switch(Type)
		  {
        case Plus:
            if (iValue < (iMax - iStep))
            {
               iValue += iStep;
            }

            else
            {
               iValue = iMax;
            }
        break;

        case Character:
		     // char shit
		     break;

        default:
			  if (iValue == 1)
			  {
				  iValue = 0;
			  }

			  else
			  {
				  iValue = 1;
			  }
			  break;
		  }
      }
      else
      {
         if (Type == Plus)
         {
            if (Value < (Max - Step))
            {
               Value += Step;
            }

            else
            {
               Value = Max;
            }
         }

         else
         {
            if (Value == 1.0f)
            {
               Value = 0.0f;
            }

            else
            {
               Value = 1.0f;
            }
         }
      }
   }

   CInline int   GetIValue()
   {
      if (bInt)
      {
         return(iValue);
      }
      return((int)Value);
   }

   CInline float GetFValue()
   {
      if (bInt)
      {
         return((float)iValue);
      }
      return(Value);
   }

   CInline void  Decrement()
   {
      if (bInt)
      {
         if (Type == Plus)
         {
            if (iValue > (iMin + iStep))
            {
               iValue -= iStep;
            }

            else
            {
               iValue = iMin;
            }
         }

         else
         {
            if (iValue == 1)
            {
               iValue = 0;
            }

            else
            {
               iValue = 1;
            }
         }
      }
      else
      {
         if (Type == Plus)
         {
            if (Value > (Min + Step))
            {
               Value -= Step;
            }

            else
            {
               Value = Min;
            }
         }

         else
         {
            if (Value == 1.0f)
            {
               Value = 0.0f;
            }

            else
            {
               Value = 1.0f;
            }
         }
      }
   }

   const bool    shouldDraw()
   {
      if (Type == Invisible)
      {
         return(false);
      }

      if (Type == Header)
      {
         return(true);
      }

      if (!parent)
      {
         return(true);
      }

      return((bool)(parent->iValue == 1));
   }
};


struct CVarManager
{
   CVar *Vars[256];
   unsigned int         Count;
   CInline CVar         *AddVar(char *Name, CVar *par, bool bInt = true, float Value = 0.0f, float Min = 0.0f, float Max = 1.0f, CVARTYPE Type = Toggle, float Int = 1.0f, char *opt0 = NULL, char *opt1 = NULL, char *opt2 = NULL, char *opt3 = NULL, char *opt4 = NULL, bool copyName = false)
   {
      CVar *pVar = hnew(CVar);

      pVar->bInt = bInt;
      if (bInt)
      {
         pVar->iMin   = (int)Min;
         pVar->iMax   = (int)Max;
         pVar->iValue = (int)Value;
         pVar->iStep  = (int)Int;
      }
      else
      {
         pVar->Min   = Min;
         pVar->Max   = Max;
         pVar->Value = Value;
         pVar->Step  = Int;
      }
      pVar->Name       = Name;
	  if (copyName == true)
	  {
		  pVar->Name = (char*)GlobalAlloc(GMEM_FIXED, strlen(Name) + 1);
		  strcpy(pVar->Name, Name);
	  }
      pVar->Type       = Type;
      pVar->parent     = par;
      pVar->Options[0] = opt0;
      pVar->Options[1] = opt1;
      pVar->Options[2] = opt2;
      pVar->Options[3] = opt3;
      pVar->Options[4] = opt4;
      Vars[Count++] = pVar;
      return(pVar);
   }

   CInline CVar         *AddHeader(char *Name)
   {
      CVar *pVar = hnew(CVar);

      pVar->iValue = 0;
      pVar->Name   = Name;
      pVar->Type   = Header;
      pVar->parent = NULL;
      pVar->bInt   = true;
      Vars[Count++] = pVar;
      return(pVar);
   }

   CInline CVar         *FindVarByName(char *Name)
   {
      for (unsigned int i = 0; i < Count; ++i)
      {
         if (Vars[i] && !strcmp(Vars[i]->Name, Name))
         {
            return(Vars[i]);
         }
      }

      return(NULL);
   }

   CInline CVar         *FindTopVar()
   {
      for (unsigned int i = 0; i < Count; ++i)
      {
         if (Vars[i] && (Vars[i]->Type != Header) && (Vars[i]->Type != Invisible) && (Vars[i]->Type != Completely_Invisible) && (Vars[i]->Type != Incognito))
         {
            return(Vars[i]);
         }
      }

      return(NULL);
   }

   CInline CVar         *FindBottomVar()
   {
      for (unsigned int i = Count - 1; i > 0; ++i)
      {
         if ((Vars[i]->Type != Invisible) && (Vars[i]->Type != Completely_Invisible) && (Vars[i]->Type != Incognito) && (Vars[i]->Type != Header) && Vars[i]->parent && Vars[i]->parent->iValue)
         {
            return(Vars[i]);
         }
      }


      return(NULL);
   }

   CInline CVar         *FindNextVar(CVar *frm)
   {
      BOOL bgotTofrm = FALSE;

      for (unsigned int i = 0; i < Count; ++i)
      {
         if ((bgotTofrm == TRUE) && (Vars[i]->Type != Header) && (Vars[i]->Type != Invisible) && (Vars[i]->Type != Completely_Invisible) && (Vars[i]->Type != Incognito))
         {
            return(Vars[i]);
         }

         if (Vars[i] == frm)
         {
            bgotTofrm = TRUE;
         }
      }

      return(NULL);
   }

   CInline unsigned int GetCvarIndex(CVar *cv)
   {
      for (unsigned int i = 0; i < Count; ++i)
      {
         if (Vars[i] == cv)
         {
            return(i);
         }
      }

      return((unsigned int)-1);
   }

   CInline unsigned int GetMenuCount()
   {
      int ic = 0;

      for (unsigned int i = 0; i < Count; ++i)
      {
         if ((Vars[i]->Type == Header) || (Vars[i]->parent->iValue && (Vars[i]->Type != Invisible) && (Vars[i]->Type != Completely_Invisible) && (Vars[i]->Type != Incognito)))
         {
            ic++;
         }
      }

      return(ic);
   }

   CInline unsigned int IsLastHeader(CVar *hed)
   {
      for (unsigned int i = GetCvarIndex(hed) + 1; i < Count; ++i)
      {
         if (Vars[i]->Type == Header)
         {
            return(i);
         }
      }

      return(0);
   }

   CInline bool         IsSecondHeader(CVar *hed)
   {
      int hc = 0;

      for (unsigned int i = 0; i < Count; ++i)
      {
         if (Vars[i]->Type == Header)
         {
            if ((++hc == 2) && (Vars[i] == hed))
            {
               return(true);
            }
         }
      }

      return(false);
   }

   CInline unsigned int FindBottomHeader()
   {
      for (unsigned int i = Count - 1; i > 0; --i)
      {
         if (Vars[i] && (Vars[i]->Type == Header))
         {
            if (Vars[i]->iValue == 0)
            {
               return(i);
            }

            else
            {
               return(i);
            }
         }
      }

      return(NULL);
   }

   CInline unsigned int GetUpHeader(CVar *hed)
   {
      if (GetCvarIndex(hed) == 0)
      {
         return(FindBottomHeader());
      }

      for (unsigned int i = GetCvarIndex(hed) - 1; i > 0; --i)
      {
         if (Vars[i]->Type == Header)
         {
            return(i);
         }
      }

      return(FindBottomHeader());
   }

   bool                 Save(char *File)
   {
      FILE *pFile = NULL;

      fopen_s(&pFile, File, DS("w"));

      if (pFile == NULL)
      {
         return(false);
      }

      char Line[256];

      for (unsigned int i = 0; i < Count; ++i)
      {
         if ((Vars[i]->Type != Header) && strcmp(Vars[i]->Name, DS("Save Configuration")) && (Vars[i]->Type != Incognito) && ((Vars[i]->Type != Completely_Invisible) || (Vars[i]->Value != 0.0f)))
         {
            int clmped = fTruncate(Vars[i]->GetFValue());

            if (clmped == -1)
            {
#ifdef X_VERSION
               sprintf_s(Line, DS("%s=%.2f\n"), Vars[i]->Name, Vars[i]->GetFValue());
#else
               char szfix[32];
               char *fix = gcvt((double)Vars[i]->GetFValue(), 5, szfix);
               sprintf_s(Line, DS("%s=%s\n"), Vars[i]->Name, fix);
#endif
            }

            else
            {
               sprintf_s(Line, DS("%s=%i\n"), Vars[i]->Name, clmped);
            }

            fwrite(Line, strlen(Line), 1, pFile);
         }
      }

      fclose(pFile);
      return(true);
   }

   CInline bool         Load(char *File)
   {
      FILE *pFile = NULL;

      fopen_s(&pFile, File, DS("r"));

      if (pFile == NULL)
      {
         return(false);
      }

      char Line[256];
      char Name[256];

      while (fgets(Line, 256, pFile) != NULL)
      {
         char *Equal = strchr(Line, '=');

         if (Equal == NULL)
         {
            continue;
         }

         int Len = (Equal - Line);
         strncpy_s(Name, Line, Len);
         Name[Len] = '\0';
         CVar *pVar = FindVarByName(Name);

         if (pVar)
         {
            ++Equal;
            if(pVar->Value == DISABLED_MAGIC_VALUE)
            {
               pVar->Value = 0.0f;
            }
            else
            {
               if (pVar->bInt)
               {
                  pVar->iValue = atoi(Equal);
               }
               else
               {
                  pVar->Value = (float)atof(Equal);
               }
            }
         }
      }

      for (unsigned int i = 0; i < Count; ++i)
      {
         if(Vars[i]->Value == DISABLED_MAGIC_VALUE)
            Vars[i]->Value = 0.0f;
      }

      fclose(pFile);
      return(true);
   }

   CInline void         RandomizeData()
   {
      for (unsigned int i = 0; i < Count; ++i)
      {
         CVar *pVar = Vars[i];
         FORCE_RANDOMIZE_STR(pVar->Name);
         pVar->Name = (char *)hrand();

         /*
          * for(unsigned int oi = 0; oi < MAX_CVAR_OPTIONSTRS; ++oi)
          * {
          *      SFORCE_RANDOMIZE_STR(pVar->Options[oi]);
          *      pVar->Options[oi] = (char*)hrand();
          * }
          */
         pVar->iStep  = hrand();
         pVar->iMin   = hrand();
         pVar->iMax   = hrand();
         pVar->parent = (CVar *)hrand();
         pVar->bInt   = (hrand() % 2) == 1;
         pVar->Type   = (CVARTYPE)hrand();
      }
      Count = hrand();
   }
};

#endif
