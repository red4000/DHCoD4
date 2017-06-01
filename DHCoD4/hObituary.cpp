// TODO: don't count kills / etc if you're in killcam watching you die - cg.demoplayback
// TODO: make a hud killspam option

#pragma warning ( disable : 4530 )
#include <windows.h>
#pragma hdrstop

#include "hMath.h"
#include "hClasses.h"
#include "hFunctions.h"
#include "hConfig.h"
#include "hAim.h"
#include "hEsp.h"
#include "hObituary.h"
#include "hBot.h"
#include "hAPI.h"

stats_t stats;
spree_t spree;

float iStatsX, iStatsY, iStatsW, iStatsH;
char  sKills[32];
char  sKnives[32];
char  sHeadshots[32];
char  sDeaths[32];
char  sSuicides[32];
char  sSpree[32];
char  sHSRatio[32];

tCG_Obituary oCG_Obituary = NULL;

/*
 * int Killed_List[MAX_PLAYERS] =
 * {
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 * };
 *
 * int Killed_Me_List[MAX_PLAYERS] =
 * {
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 *      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 * };
 *
 * int soundcounter = -1;
 * clock_t newtime, oldtime, timedif;
 *
 * void h_CG_Obituary( )
 * {
 *      CEntityState * ent         = NULL;
 *      _asm mov ent, eax
 *
 *      int             iTarget     = ent->otherEntityNum;
 *      int             iAttacker   = ent->otherEntityNum2;
 *      int             iMod        = ent->EventParam;
 *
 *      clientInfo_t *  ciTarget    = &clientinfo[iTarget];
 *      clientInfo_t *  ciAttacker  = &clientinfo[iAttacker];
 *      centity_t *     ceTarget    = &cg_entities[iTarget];
 *      centity_t *     ceAttacker  = &cg_entities[iAttacker];
 *
 *      if( ciAttacker && ciAttacker->clientNum == cg->clientNum )
 *      {
 *              if( ciTarget->clientNum == cg->clientNum )
 *              {
 *                      Stats.Deaths++;
 *                      Stats.Suicides++;
 *                      Stats.Spree = 0;
 *
 *                      if( Settings.Killsounds->Value.bValue )
 *                              KillSound( "Dead" );
 *              }
 *              else
 *              {
 *                      if( Stats.TotalKills == 0 )
 *                      {
 *                              Stats.TotalKills++;
 *                              Stats.Spree++;
 *
 *                              if( Settings.Killsounds->Value.bValue )
 *                                      KillSound( "Firstblood" );
 *
 *                              if( Settings.Killspam->Value.bValue )
 *                              {
 *                                      if( Killed_List[ciTarget->clientNum] > 1 )
 *                                              SendConsoleCommand( 0, 0, va( "say ^1%s ^7has been killed by %s ^1[^7%i^1] ^7times! ^1FIRST BLOOD!", ciTarget->Name, clientinfo[cg->clientNum].Name, Killed_List[ciTarget->clientNum] ) );
 *                                      else
 *                                              SendConsoleCommand( 0, 0, va( "say ^1%s ^7has been killed by %s ^1[^7%i^1] ^7time! ^1FIRST BLOOD!", ciTarget->Name, clientinfo[cg->clientNum].Name, Killed_List[ciTarget->clientNum] ) );
 *                              }
 *                      }
 *                      else
 *                      {
 *                              Stats.TotalKills++;
 *                              Stats.Spree++;
 *
 *                              if( ( iMod & 0xFFFFFF7F ) == 7 )
 *                              {
 *                                      Stats.Bashes++;
 *                                      if( Settings.Killsounds->Value.bValue )
 *                                              CreateThread( 0, 0, KillSound, "Bash", 0, 0 );
 *                              }
 *                              else if( ( iMod & 0xFFFFFF7F ) == 8 )
 *                              {
 *                                      Stats.Headshots++;
 *                                      if( Settings.Killsounds->Value.bValue )
 *                                              KillSound( "Headshot" );
 *                              }
 *
 *                              if( Settings.Killsounds->Value.bValue )
 *                              {
 *                                      oldtime = newtime;
 *                                      newtime = clock();
 *                                      timedif = ( newtime - oldtime ) / 1500;
 *
 *                                      if( timedif <= 1 ){ soundcounter++; }
 *                                      if( timedif > 1 ){ soundcounter = 0; }
 *
 *                                      if( soundcounter == 1 ){ KillSound( "Doublekill" ); }
 *                                      if( soundcounter == 2 ){ KillSound( "Triplekill" ); }
 *                                      if( soundcounter == 3 ){ KillSound( "Multikill" ); }
 *                                      if( soundcounter == 4 ){ KillSound( "Ultrakill" ); }
 *                                      if( soundcounter >= 5 ){ KillSound( "Monsterkill" ); }
 *
 *                                      if( Stats.Spree == 10 ){ KillSound( "Killingspree" ); }
 *                                      if( ( Stats.Spree == 20  ||  Stats.Spree == 100  ||  Stats.Spree == 220  ) ){ KillSound( "Dominating" ); }
 *                                      if( ( Stats.Spree == 30  ||  Stats.Spree == 120  ||  Stats.Spree == 240  ) ){ KillSound( "Godlike" ); }
 *                                      if( ( Stats.Spree == 40  ||  Stats.Spree == 140  ||  Stats.Spree == 260  ) ){ KillSound( "Unstoppable" ); }
 *                                      if( ( Stats.Spree == 50  ||  Stats.Spree == 160  ||  Stats.Spree == 280  ) ){ KillSound( "Rampage" ); }
 *                                      if( ( Stats.Spree == 60  ||  Stats.Spree == 180  ||  Stats.Spree == 300  ) ){ KillSound( "Holyshit" ); }
 *                                      if( ( Stats.Spree == 70  ||  Stats.Spree == 200  ||  Stats.Spree == 320  ) ){ KillSound( "Dominating" ); }
 *                                      if( ( Stats.Spree == 80  ||  Stats.Spree == 220  ||  Stats.Spree == 340  ) ){ KillSound( "Killingspree" ); }
 *                              }
 *
 *                              Killed_List[ciTarget->clientNum]++;
 *
 *                              if( Settings.Killspam->Value.bValue )
 *                              {
 *                                      if( Killed_List[ciTarget->clientNum] > 1 )
 *                                              SendConsoleCommand( 0, 0, va( "say ^9%s ^1has been killed by %s ^1[^9%i^1] times^9!", ciTarget->Name, clientinfo[cg->clientNum].Name, Killed_List[ciTarget->clientNum] ) );
 *                                      else
 *                                              SendConsoleCommand( 0, 0, va( "say ^9%s ^1has been killed by %s ^1[^9%i^1] time^9!", ciTarget->Name, clientinfo[cg->clientNum].Name, Killed_List[ciTarget->clientNum] ) );
 *                              }
 *                      }
 *              }
 *      }
 *      else if( ciTarget->clientNum == cg->clientNum )
 *      {
 *              Stats.Deaths++;
 *              Stats.Spree = 0;
 *
 *              Killed_Me_List[ciTarget->clientNum]++;
 *
 *              if( Settings.Killsounds->Value.bValue )
 *                      KillSound( "Dead" );
 *      }
 *
 *      if( Stats.TotalKills == 0 )
 *              Stats.HSRatio = 0.0f;
 *      else
 *              Stats.HSRatio = (float)Stats.Headshots / (float)Stats.TotalKills * 100.0f;
 *
 *      _asm mov eax, ent
 *
 *      oCG_Obituary( );
 * }
 */
void ResetStats()
{
#ifdef KILLSTATS
   memset(&stats, 0, sizeof(stats_t));
   memset(&spree, 0, sizeof(spree_t));
   memset(&sKills, 0, sizeof(sKills));
   memset(&sKnives, 0, sizeof(sKnives));
   memset(&sHeadshots, 0, sizeof(sHeadshots));
   memset(&sDeaths, 0, sizeof(sDeaths));
   memset(&sSuicides, 0, sizeof(sSuicides));
   memset(&sSpree, 0, sizeof(sSpree));
   memset(&sHSRatio, 0, sizeof(sHSRatio));
#endif
}


// TODO: make text drawn in scale, and have y position compensated by scaling
void DrawKillStats() // TODO: move to hGUI
{
   iStatsW = 145;
   iStatsH = 132;
   iStatsX = refdef->iWidth - 15 - iStatsW;
   iStatsY = refdef->iHeight - 15 - (iStatsH / 2) * 2.5f;

   if (cvar_Radar->iValue == 1)
   {
      iStatsY = RadarY + cvar_RadarSize->Value + 6.0f;
   }

   DrawOutline(iStatsX, iStatsY, iStatsW, iStatsH, cvar_GUIOutlineThickness->Value, background);
   DrawBox(iStatsX, iStatsY + 20, iStatsW, 2, background);

   DrawBox(iStatsX, iStatsY, iStatsW, 20, foreground);
   DrawGlowText(ConsoleFont, (iStatsX + iStatsW / 2) - (strlen("Kill Stats") / 2 * 9), iStatsY + 17, 1.0f, 1.0f, 0.0f, selectedText, GLOW_SHADOWED, glow, "Kill Stats");

   DrawBox(iStatsX, iStatsY + 22, iStatsW, iStatsH - 22, foreground);

   DrawGlowText(ConsoleFont, iStatsX + 5, iStatsY + 39, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, "Kills");
   DrawGlowText(ConsoleFont, iStatsX + 5, iStatsY + 54, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, "Headshots");
   DrawGlowText(ConsoleFont, iStatsX + 5, iStatsY + 69, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, "Knives");
   DrawGlowText(ConsoleFont, iStatsX + 5, iStatsY + 84, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, "Deaths");
   DrawGlowText(ConsoleFont, iStatsX + 5, iStatsY + 99, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, "Suicides");
   DrawGlowText(ConsoleFont, iStatsX + 5, iStatsY + 114, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, "Killstreak");
   DrawGlowText(ConsoleFont, iStatsX + 5, iStatsY + 129, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, "HS Ratio");

   DrawGlowText(ConsoleFont, iStatsX + iStatsW - 25, iStatsY + 39, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, sKills);
   DrawGlowText(ConsoleFont, iStatsX + iStatsW - 25, iStatsY + 54, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, sHeadshots);
   DrawGlowText(ConsoleFont, iStatsX + iStatsW - 25, iStatsY + 69, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, sKnives);
   DrawGlowText(ConsoleFont, iStatsX + iStatsW - 25, iStatsY + 84, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, sDeaths);
   DrawGlowText(ConsoleFont, iStatsX + iStatsW - 25, iStatsY + 99, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, sSuicides);
   DrawGlowText(ConsoleFont, iStatsX + iStatsW - 25, iStatsY + 114, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, sSpree);
   DrawGlowText(ConsoleFont, iStatsX + iStatsW - 35, iStatsY + 129, 1.0f, 1.0f, 0.0f, text, GLOW_SHADOWED, glow, sHSRatio);
}


void PlaySpreeSound()
{
   switch (spree.killSpreeCount)
   {
   case 2:
      PLAY_SOUND("Sounds/doublekill.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2DOUBLEKILL", '2'));
      break;

   case 3:
      PLAY_SOUND("Sounds/triplekill.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2TRIPPLEKILL", '2'));
      break;

   case 4:
      PLAY_SOUND("Sounds/ultrakill.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2ULTRAKILL", '2'));
      break;

   case 5:
      PLAY_SOUND("Sounds/killingspree.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2KILLINGSPREE", '2'));
      break;

   case 6:
      PLAY_SOUND("Sounds/multikill.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2MULTIKILL", '2'));
      break;

   case 7:
      PLAY_SOUND("Sounds/godlike.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2GODLIKE", '2'));
      break;

   case 8:
      PLAY_SOUND("Sounds/monsterkill.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2MONSTERKILL", '2'));
      break;

   case 9:
      PLAY_SOUND("Sounds/rampage.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2RAMPAGE", '2'));
      break;

   case 10:
      PLAY_SOUND("Sounds/dominating.wav");
      CG_EchoToConsolef(CENTER, RandomizeColors("^2DOMINATING", '2'));
      break;


   default:
      int iSound = random(0, 30);

      switch (iSound)
      {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
         PLAY_SOUND("Sounds/ludacriskill.wav");
         CG_EchoToConsolef(CENTER, RandomizeColors("^2OWNAGE", '2'));
         break;

      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
         PLAY_SOUND("Sounds/wickedsick.wav");
         CG_EchoToConsolef(CENTER, RandomizeColors("^2WICKED SICK", '2'));
         break;

      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
         PLAY_SOUND("Sounds/holyshit.wav");
         CG_EchoToConsolef(CENTER, RandomizeColors("^2HOLY SHIT", '2'));
         break;

      case 0:
      case 9:
      case 10:
      case 29:
      case 30:
      case 19:
      case 20:
         PLAY_SOUND("Sounds/megakill.wav");
         CG_EchoToConsolef(CENTER, RandomizeColors("^2MEGAKILL", '2'));
         break;

      default:
         __assume(0);
      }

      break;
   }
}


void hCG_Obituary(CEntityState *ent)
{
   if (ent)
   {
      int     iTarget     = ent->otherEntityNum;
      int     iAttacker   = ent->otherEntityNum2;
      int     iMod        = ent->eventParm;
      CEntity *ciTarget   = NULL;
      CEntity *ciAttacker = NULL;

      if ((iTarget >= 0) && (iTarget < MAX_PLAYERS) && pLocalEnt->Alive)
      {
         ciTarget = GetEntityByIndex(iTarget);

         if ((iAttacker >= 0) && (iAttacker < MAX_PLAYERS))
         {
            ciAttacker = GetEntityByIndex(iAttacker);
         }

         else
         {
            iAttacker = ENTITYNUM_WORLD;
         }

         if (ciAttacker && (ciAttacker->ClientNumber == cg->clientNum))
         {
            if (ciTarget->ClientNumber == cg->clientNum)
            {
               if (GetClientByIndex(cg->clientNum)->Team != TEAM_SPECTATOR)
               {
                  if (cvar_KillSpam->iValue)
                  {
                     spree.killSpreeCount = 0;
                  }

                  stats.Spree = 0;
                  ++stats.Suicides;

                  if (cvar_KillSounds->iValue)
                  {
                     ++spree.suicides;
                     spree.killsNoDeaths  = 0;
                     spree.killSpreeCount = 0;

                     PLAY_SOUND("Sounds/slap.wav");
                  }
               }
            }

            else
            {
               if ((iMod & 0xFFFFFF7F) == 7)
               {
                  ++stats.Kills;
                  ++stats.Bashes;

                  if (cvar_KillSounds->iValue)
                  {
                     ++spree.kills;
                     ++spree.bashes;

                     PLAY_SOUND("Sounds/bash.wav");

                     if (spree.killsNoDeaths > spree.longestSpree)
                     {
                        spree.longestSpree = spree.killsNoDeaths;
                     }
                  }
               }

               else if ((iMod & 0xFFFFFF7F) == 8)
               {
                  ++stats.Kills;
                  ++stats.Headshots;
                  ++stats.Spree;

                  if (cvar_KillSounds->iValue)
                  {
                     ++spree.kills;
                     ++spree.headshots;

                     if (spree.killsNoDeaths > spree.longestSpree)
                     {
                        spree.longestSpree = spree.killsNoDeaths;
                     }

                     spree.lastVictim = iTarget;
                     ++spree.killsNoDeaths;

                     if (spree.killsNoDeaths > spree.longestSpree)
                     {
                        spree.longestSpree = spree.killsNoDeaths;
                     }

                     if ((cg->CurrentSnap->ServerTime - spree.lastKillTime) > (3500))
                     {
                        spree.killSpreeCount = 1;
                        spree.fHeadshot      = 0;
                     }

                     else
                     {
                        ++spree.killSpreeCount;
                     }

                     if (spree.killSpreeCount == 1)
                     {
                        spree.fHeadshot = 1;
                        PLAY_SOUND("Sounds/headshot.wav");
                     }

                     spree.lastKillTime = cg->CurrentSnap->ServerTime;

                     if (spree.killSpreeCount > 1)
                     {
                        PlaySpreeSound();
                     }
                  }

                  if (cvar_KillSpam->iValue)
                  {
                     if (ciAttacker && (ciAttacker->ClientNumber == cg->clientNum) && ciAttacker->Alive && (ciTarget->ClientNumber != cg->clientNum))
                     {
                        if (GetClientByIndex(ciTarget->ClientNumber)->Name != GetClientByIndex(cg->clientNum)->Name)
                        {
                           CG_ConsoleCommandf(RandomizeColors("say ^4%s ^3was slain with a ^2HEADSHOT ^5by ^1" HACK_NAME "!\n"), GetClientByIndex(ciTarget->ClientNumber)->Name);
                        }
                     }
                  }
               }

               else
               {
                  if (cvar_KillSpam->iValue)
                  {
                     if (ciAttacker && (ciAttacker->ClientNumber == cg->clientNum) && ciAttacker->Alive && (ciTarget->ClientNumber != cg->clientNum))
                     {
                        if (GetClientByIndex(ciTarget->ClientNumber)->Name != GetClientByIndex(cg->clientNum)->Name)
                        {
                           CG_ConsoleCommandf(RandomizeColors("say ^4%s ^3was slain by ^1" HACK_NAME "!\n"), GetClientByIndex(ciTarget->ClientNumber)->Name);
                        }
                     }
                  }

                  ++stats.Kills;
                  ++stats.Spree;

                  if (cvar_KillSounds->iValue)
                  {
                     ++spree.kills;

                     if (spree.killsNoDeaths > spree.longestSpree)
                     {
                        spree.longestSpree = spree.killsNoDeaths;
                     }

                     if ((cg->CurrentSnap->ServerTime - spree.lastKillTime) > (3500))
                     {
                        spree.killSpreeCount = 1;
                        spree.fHeadshot      = 0;
                     }

                     else
                     {
                        spree.killSpreeCount++;
                     }

                     spree.lastKillTime = cg->CurrentSnap->ServerTime;

                     if (spree.killSpreeCount > 1)
                     {
                        PlaySpreeSound();
                     }

                     spree.lastVictim = iTarget;
                     spree.killsNoDeaths++;
                  }
               }
            }
         }

         else if (ciTarget->ClientNumber == cg->clientNum)
         {
            if (cvar_KillSpam->iValue)
            {
               spree.killSpreeCount = 0;
            }

            ++stats.Deaths;
            stats.Spree = 0;

            if (cvar_KillSounds->iValue)
            {
               ++spree.deaths;

               switch (spree.deaths)
               {
               case 1:
                  PLAY_SOUND("Sounds/deads/dead1.wav");
                  break;

               case 2:
                  PLAY_SOUND("Sounds/deads/dead2.wav");
                  break;

               case 3:
                  PLAY_SOUND("Sounds/deads/dead3.wav");
                  break;

               case 4:
                  PLAY_SOUND("Sounds/deads/dead4.wav");
                  break;

               case 5:
                  PLAY_SOUND("Sounds/deads/dead5.wav");
                  break;

               case 6:
                  PLAY_SOUND("Sounds/deads/dead6.wav");
                  break;

               case 7:
                  PLAY_SOUND("Sounds/deads/dead7.wav");
                  break;

               case 8:
                  PLAY_SOUND("Sounds/deads/dead8.wav");
                  break;

               case 9:
                  PLAY_SOUND("Sounds/deads/dead9.wav");
                  break;

               default:
                  char szDeathSound[128];
                  sprintf_s(szDeathSound, "Sounds/deads/dead%i.wav", random(1, 9));
                  PLAY_SOUND(szDeathSound);
                  break;
               }

               spree.killsNoDeaths  = 0;
               spree.killSpreeCount = 0;
               spree.headshots      = 0;
            }
         }
      }

      _itoa_s(stats.Kills, sKills, 10);
      _itoa_s(stats.Headshots, sHeadshots, 10);
      _itoa_s(stats.Bashes, sKnives, 10);
      _itoa_s(stats.Deaths, sDeaths, 10);
      _itoa_s(stats.Suicides, sSuicides, 10);
      _itoa_s(stats.Spree, sSpree, 10);
      
      if (stats.Kills != 0)
      {
         stats.HSRatio = ((float)stats.Headshots / (float)stats.Kills) * 100.0f;
      }
      else
      {
         if (stats.Headshots != 0)
         {
            stats.HSRatio = 100.0f;
         }
         else
         {
            stats.HSRatio = 0.0f;
         }
      }
      sprintf_s(sHSRatio, "%i", (int)stats.HSRatio);
   }
}
