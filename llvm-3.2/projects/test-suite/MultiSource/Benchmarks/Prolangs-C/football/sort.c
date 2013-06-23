/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
#include <stdio.h>
#include "definitions.h"
#include "stats.h"
#include "common.h"
#include "io.h"

BOOL absolute_standing_lt(TEAMS_STATS info,int team1,int team2)
{
  if (WIN_PCT(info[team1]) != WIN_PCT(info[team2]))
    return (WIN_PCT(info[team1]) > WIN_PCT(info[team2]));

  if (info[team1].loses == 0 && info[team2].loses == 0)
    return (info[team1].wins > info[team2].wins);

  if (info[team1].wins == 0 && info[team2].wins == 0)
    return (info[team1].loses < info[team2].loses);

  if (a_champ(team1) != a_champ(team2))
    return a_champ(team1);
  if (a_wild_card(team1) != a_wild_card(team2))
    return a_wild_card(team1);
  if (divisions[team1][CONFERENCE] != divisions[team2][CONFERENCE])
        return( divisions[team1][CONFERENCE] < divisions[team2][CONFERENCE] );
  return(team1 < team2);
}

BOOL standing_lt(TEAMS_STATS info,int team1,int team2)
{
  if (divisions[team1][CONFERENCE] != divisions[team2][CONFERENCE]) 
    return( divisions[team1][CONFERENCE] < divisions[team2][CONFERENCE] );
  
  if (divisions[team1][DIVISION] != divisions[team2][DIVISION]) 
    return( divisions[team1][DIVISION] < divisions[team2][DIVISION] );
  
  return absolute_standing_lt(info,team1,team2);
}

BOOL conf_standing_lt(TEAMS_STATS info,int team1,int team2)
{
  if (divisions[team1][CONFERENCE] != divisions[team2][CONFERENCE]) 
    return( divisions[team1][CONFERENCE] < divisions[team2][CONFERENCE] );

  return absolute_standing_lt(info,team1,team2);
}

BOOL defence_lt(TEAMS_STATS info,int team1,int team2)
{
  if (divisions[team1][CONFERENCE] != divisions[team2][CONFERENCE]) 
    return( divisions[team1][CONFERENCE] < divisions[team2][CONFERENCE] );
  
  if (PER_GAME(info,info[team1].points_against,team1) != 
      PER_GAME(info,info[team2].points_against,team2))
    return (PER_GAME(info,info[team1].points_against,team1) < 
      PER_GAME(info,info[team2].points_against,team2));

  return (team1 < team2);
}

BOOL defence_nfl_lt(TEAMS_STATS info,int team1,int team2)
{
  if (PER_GAME(info,info[team1].points_against,team1) != 
      PER_GAME(info,info[team2].points_against,team2))
    return (PER_GAME(info,info[team1].points_against,team1) < 
      PER_GAME(info,info[team2].points_against,team2));

  return (team1 < team2);
}

BOOL offence_lt(TEAMS_STATS info,int team1,int team2)
{
  if (divisions[team1][CONFERENCE] != divisions[team2][CONFERENCE]) 
    return( divisions[team1][CONFERENCE] < divisions[team2][CONFERENCE] );
  
  if (PER_GAME(info,info[team1].points_for,team1) != 
      PER_GAME(info,info[team2].points_for,team2))
    return (PER_GAME(info,info[team1].points_for,team1) > 
      PER_GAME(info,info[team2].points_for,team2));

  return (team1 < team2);
}

BOOL offence_nfl_lt(TEAMS_STATS info,int team1,int team2)
{
  if (PER_GAME(info,info[team1].points_for,team1) != 
      PER_GAME(info,info[team2].points_for,team2))
    return (PER_GAME(info,info[team1].points_for,team1) > 
      PER_GAME(info,info[team2].points_for,team2));

  return (team1 < team2);
}


BOOL net_nfl_lt(TEAMS_STATS info,int team1,int team2)
{
  if (PER_GAME(info,NET_POINTS(info,team1),team1) != 
      PER_GAME(info,NET_POINTS(info,team2),team2))
    return (PER_GAME(info,NET_POINTS(info,team1),team1) > 
      PER_GAME(info,NET_POINTS(info,team2),team2));

  return (team1 < team2);
}

BOOL net_lt(TEAMS_STATS info,int team1,int team2)
{
  if (divisions[team1][CONFERENCE] != divisions[team2][CONFERENCE]) 
    return( divisions[team1][CONFERENCE] < divisions[team2][CONFERENCE] );
  
  return net_nfl_lt(info,team1,team2);
}

BOOL tied(TEAMS_STATS info,int team1,int team2)
{
  if (WIN_PCT(info[team1]) != WIN_PCT(info[team2]))
    return NO;

  if (info[team1].loses == 0 && info[team2].loses == 0)
    return (info[team1].wins == info[team2].wins);

  if (info[team1].wins == 0 && info[team2].wins == 0)
    return (info[team1].loses == info[team2].loses);

  return YES;
}


#define CHAMP(champ,conf,div) if ( (champ == ERROR_0) && \
			  (divisions[rank[team_c]][CONFERENCE] == conf) && \
			  (divisions[rank[team_c]][DIVISION] == div) ) \
                                  champ = rank[team_c]


void break_ties(TEAMS_STATS info,ORDERING rank,int within,BOOL display)
{
  int i,first_of_equals;
  int team_c;
  int afc,nfc;

  if (within == DIVISION && !display) {
    nfc_east_champ = nfc_central_champ = nfc_west_champ = ERROR_0;
    afc_east_champ = afc_central_champ = afc_west_champ = ERROR_0;
  }
  if (within != NFL_0 && !display) {
    nfc_wild_card1 = nfc_wild_card2 = nfc_wild_card3 = ERROR_0;
    afc_wild_card1 = afc_wild_card2 = afc_wild_card3 = ERROR_0;
  }
  first_of_equals = 1;
  for (i=1; i <= NUM_TEAMS; i ++) 
    if ( !((i == NUM_TEAMS) ? NO :
	  (tied(info,rank[i],rank[i+1]) ? 
	   (within == DIVISION ? 
	    (divisions[rank[i]][DIVISION] == divisions[rank[i+1]][DIVISION] &&
	     divisions[rank[i]][CONFERENCE]==divisions[rank[i+1]][CONFERENCE])
	    :(within == CONFERENCE ? 
	      (divisions[rank[i]][CONFERENCE]==divisions[rank[i+1]][CONFERENCE]
	      && (a_champ(rank[i]) == a_champ(rank[i+1])))
	    : (a_champ(rank[i]) == a_champ(rank[i+1])) &&
	      (a_wild_card(rank[i]) == a_wild_card(rank[i+1]))
	      )) : NO)) ) {
      if ( i != first_of_equals) {
	break_a_tie(&(rank[first_of_equals]),i-first_of_equals+1,display);
	if (display) {
	  extern BOOL got_unused_responce;

	  prompt(stdout,stdin);
	  if (got_unused_responce) display = NO;
	}
      }
      first_of_equals = i+1;
    } 

  afc = nfc = 0;
  if (within == DIVISION)
    for (team_c=1; team_c <= NUM_TEAMS; team_c ++) {
      CHAMP(nfc_east_champ,NFC_0,EAST_0);
      CHAMP(nfc_central_champ,NFC_0,CENTRAL_0);
      CHAMP(nfc_west_champ,NFC_0,WEST_0);
      CHAMP(afc_east_champ,AFC_0,EAST_0);
      CHAMP(afc_central_champ,AFC_0,CENTRAL_0);
      CHAMP(afc_west_champ,AFC_0,WEST_0);
    } else if (within == CONFERENCE)
      for (team_c=1; team_c <= NUM_TEAMS; team_c ++) {
	if (divisions[conf_standings[team_c]][CONFERENCE] == NFC_0) {
	  if (!(a_champ(conf_standings[team_c])))
	   if (nfc == 0) {nfc++; nfc_wild_card1 = conf_standings[team_c];}
	   else if (nfc == 1) {nfc++; nfc_wild_card2 = conf_standings[team_c];}
	   else if (nfc == 2 && (iyear >= 90 || iyear <=70)) 
	     {nfc++; nfc_wild_card3 = conf_standings[team_c];}
	} else {
	  if (!(a_champ(conf_standings[team_c])))
	  if (afc == 0) {afc++; afc_wild_card1 = conf_standings[team_c];}
	  else if (afc == 1) {afc++; afc_wild_card2 = conf_standings[team_c];}
	  else if (afc == 2 && (iyear >= 90 || iyear <= 70)) 
	    {afc++; afc_wild_card3 = conf_standings[team_c];}
	}
      }
}

void sort_all(void) 
{
  sort(team_info,standing_lt,1,NUM_TEAMS,standings);
  break_ties(team_info,standings,DIVISION,NO);
  sort(team_info,conf_standing_lt,1,NUM_TEAMS,conf_standings);
  break_ties(team_info,conf_standings,CONFERENCE,NO);
  sort(team_info,absolute_standing_lt,1,NUM_TEAMS,abs_standings);
  break_ties(team_info,abs_standings,NFL_0,NO); 
  sort(team_info,defence_lt,1,NUM_TEAMS,defence_ranks);
  sort(team_info,defence_nfl_lt,1,NUM_TEAMS,defence_ranks_nfl);
  sort(team_info,offence_lt,1,NUM_TEAMS,offence_ranks);
  sort(team_info,offence_nfl_lt,1,NUM_TEAMS,offence_ranks_nfl);
  sort(team_info,net_lt,1,NUM_TEAMS,net_ranks);
  sort(team_info,net_nfl_lt,1,NUM_TEAMS,net_ranks_nfl);
}
