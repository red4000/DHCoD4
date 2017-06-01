#ifndef HCHAMS_H
#define HCHAMS_H

MFUNC(DWORD WINAPI, ChamThread(LPVOID));

#define Sky      ((NumVertices == 20 && primCount == 10))
#define Rain     (m_Stride == 20 && (NumVertices == 4096 && primCount == 2048))
#define Smoke    ((NumVertices == 16384))
#define Walls    ((m_Stride == 44))

enum DrawnType
{
   DRAWN_NOTHING = 0,
   DRAWN_TEAM1,
   DRAWN_TEAM2,
   DRAWN_LAST_TEAM,
   DRAWN_WEAPON,
};

typedef struct
{
   char      *Model;
   DrawnType Drawn;
   size_t    len;   //strlen
} DrawnModel;

#endif
