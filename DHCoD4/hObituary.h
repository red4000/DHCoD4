#ifndef HOBITUARY_H
#define HOBITUARY_H

typedef struct
{
   int   Kills;
   int   Bashes;
   int   Headshots;
   int   Deaths;
   int   Suicides;
   int   Spree;
   float HSRatio;
} stats_t;

typedef struct
{
   int  kills;
   int  deaths;
   int  suicides;
   int  bashes;
   int  headshots;
   int  lastKillTime;
   bool multikill;
   bool killing;
   bool godlike;
   bool master;
   bool monster;
   bool SpreeSet;
   int  killsNoDeaths;
   int  lastSetSpree;
   int  lastVictim;
   int  killSpreeCount;
   int  longestSpree;
   int  fHeadshot;
} spree_t;

typedef void (*tCG_Obituary)();

void hCG_Obituary(CEntityState *ent);
void ResetStats();
void DrawKillStats();

extern tCG_Obituary oCG_Obituary;
extern stats_t      stats;
extern spree_t      spree;

#endif
