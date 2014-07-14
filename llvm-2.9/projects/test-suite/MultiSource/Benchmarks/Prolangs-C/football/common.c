/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
#include <stdio.h>
#include "definitions.h"
#include "stats.h"

TEAMS_STATS common_games;
TEAM common_teams[NUM_TEAMS+1];

extern void display_split(FILE *stream,TEAM *teams,int split,int num);

BOOL matches_any_team(int num,TEAM code)
{
  int i;
  for (i=1;i<=num;i++)
    if (common_teams[i] == code)
      return i;
  return NO;
}

void common(int num)
{
  TEAM tmp_team,temp;
  int week,game,i;
  int in_common[NUM_TEAMS+1];
  TEAM team1,team2;
  int site1,site2;

  for (tmp_team=1;tmp_team<=NUM_TEAMS; tmp_team++)
    in_common[tmp_team] = 0;

  for (temp=1;temp<=num;temp++)
    for (tmp_team=1;tmp_team<=NUM_TEAMS; tmp_team++)
      in_common[tmp_team] += team_plays[common_teams[temp]][tmp_team];

  for (temp=1;temp<=NUM_TEAMS;temp++) {
    if (in_common[temp] < num)
      in_common[temp] = NO;
    else in_common[temp] = YES;
  }
/* stats */
  for (team1=1;team1<=NUM_TEAMS; team1++) {
    common_games[team1].wins = common_games[team1].loses = 0;
    common_games[team1].ties = 0;
    common_games[team1].points_for = common_games[team1].points_against = 0;
  }
   
  for (week = 1; week <= num_games;week++) 
    for (game = 1;game<=NUM_TEAMS/2; game++) 
      for (i = 1; i<=2; i++) {
	if ( i == 1) {
	  team1 = sched[week][game][HOST]; site1 = HOST;
	  team2 = sched[week][game][VISITOR]; site2 = VISITOR;
	} else {
	  team1 = sched[week][game][VISITOR]; site1 = VISITOR;
	  team2 = sched[week][game][HOST]; site2 = HOST;
	}
	if ( team1 != 0 && in_common[team1] )
	  if (matches_any_team(num,team2)) 
	    if (scores[week][game][HOST] >= 0 &&
		scores[week][game][VISITOR] >=0) {
	      if (scores[week][game][site1] < scores[week][game][site2])
		common_games[team2].wins ++;
	      else if (scores[week][game][site1] > scores[week][game][site2])
		common_games[team2].loses ++;
	      else common_games[team2].ties ++;
	    } else common_games[team2].points_for ++;
      } 
  for (team1=1;team1<=28;team1++)
    if (common_games[team1].wins + common_games[team1].loses +
	common_games[team1].ties + common_games[team1].points_for < 4)
      common_games[team1].wins = -1;
}


int head_to_head_sweap(TEAM *teams,int num)
{
  TEAM temp1,temp2;
  BOOL sweap_win=YES,sweap_lose=YES;

  for (temp1=0;temp1<num;temp1++) {
      sweap_win = sweap_lose = YES;
      for (temp2=0;temp2<num;temp2++) 
	if (temp1 != temp2) {
	  if (h_to_h_stats[teams[temp1]][teams[temp2]].ties == 0 &&
	      h_to_h_stats[teams[temp1]][teams[temp2]].wins == 0 &&
	      h_to_h_stats[teams[temp1]][teams[temp2]].loses == 0)
	    sweap_win = sweap_lose = NO;
	  if (h_to_h_stats[teams[temp1]][teams[temp2]].ties != 0 )
	    sweap_win = sweap_lose = NO;
	  if (h_to_h_stats[teams[temp1]][teams[temp2]].loses != 0)
	    sweap_win = NO;
	  if (h_to_h_stats[teams[temp1]][teams[temp2]].wins != 0)
	    sweap_lose = NO;
	}
      if (sweap_win) {
	TEAM tmp_team;
	tmp_team = teams[0];
	teams[0] = teams[temp1];
	teams[temp1] = tmp_team;
	return 1;
      }
      if (sweap_lose) {
	TEAM tmp_team;
	tmp_team = teams[num-1];
	teams[num-1] = teams[temp1];
	teams[temp1] = tmp_team;
	return num-1;
      }
    }
  return num;
}

void split_around(TEAM *teams,int num_best,int num,double *pct,double best_pct)
{
  int t1,t2;
  int i;
  TEAM temp[NUM_TEAMS];

  t1 = t2 = 0;
  for (i=0;i<num;i++) 
    if (pct[i] == best_pct) {
      temp[t1] = teams[i];
      t1 ++;
    } else {
      temp[t2+num_best] = teams[i];
      t2 ++;
    }
  for (i=0;i<num;i++)
    teams[i] = temp[i];
}

int head_to_head(TEAM *teams,int num)
{
  int i,j;
  double best_pct,pct[MAX_TEAMS_DIV];
  int num_best;
  int wins,loses,ties;
  
  if (num > MAX_TEAMS_DIV) {
    (void) printf("ERROR in head_to_head (common.c). Assumption violated.\n");
    exit(1);
  }

  best_pct = 0.0;
  num_best = 0;
  for (i=0;i<num;i++) {
    wins = loses = ties = 0;
    for (j=0;j<num;j++) 
      if (i != j) {
	wins += h_to_h_stats[teams[i]][teams[j]].wins;
	loses += h_to_h_stats[teams[i]][teams[j]].loses;
	ties += h_to_h_stats[teams[i]][teams[j]].ties;
      }
    pct[i] = (wins+loses+ties == 0 ? 0.0 :
	      ((double) wins + 0.5 * ties) / ((double) (wins + loses + ties)));
    if (pct[i] > best_pct) {
      best_pct = pct[i];
      num_best = 0;
    }
    if (pct[i] == best_pct) num_best++;
  }

  split_around(teams,num_best,num,pct,best_pct);
  return num_best;
}


/* */
int break_common(TEAM *teams,int num)
{
  int i;
  double best_pct,pct[NUM_TEAMS];
  int num_best;
  
  if (num > NUM_TEAMS) {
    (void) printf("ERROR in break_common (common.c). Assumption violated.\n");
    exit(1);
  }

  for (i=0;i<num;i++)
    common_teams[i+1] = teams[i];
  common(num);
  for (i=0;i<num;i++) {
    if (common_games[teams[i]].wins == -1) return num;
  }
  best_pct = 0.0;
  num_best = 0;
  for (i=0;i<num;i++) {
    pct[i] = WIN_PCT(common_games[teams[i]]);
    if (pct[i] > best_pct) {
      best_pct = pct[i];
      num_best = 0;
    }
    if (pct[i] == best_pct) num_best++;
  }

  split_around(teams,num_best,num,pct,best_pct);
  return num_best;
}

/* */
int div_conf_rec(TEAM *teams,int num,int which)
{
  int i;
  double best_pct,pct[NUM_TEAMS];
  int num_best;
  
  if (num > NUM_TEAMS) {
    (void) printf("ERROR in div_conf_rec (common.c). Assumption violated.\n");
    exit(1);
  }

  best_pct = 0.0;
  num_best = 0;
  for (i=0;i<num;i++) { 
    if (which == DIVISION)
      pct[i] = WIN_PCT(team_info_wi_div[teams[i]]);
    else pct[i] = WIN_PCT(team_info_wi_conf[teams[i]]);
    if (pct[i] > best_pct) {
      best_pct = pct[i];
      num_best = 0;
    }
    if (pct[i] == best_pct) num_best++;
  }

  split_around(teams,num_best,num,pct,best_pct);
  return num_best;
}

int break_net_points(TEAM *teams,int num,int which)
{
  int i;
  double best_pct,pct[NUM_TEAMS];
  int num_best;
  
  if (num > NUM_TEAMS) {
    (void) printf("ERROR in break_net_points (common.c). Assumption violated.\n");
    exit(1);
  }

  best_pct = -10000.0;
  num_best = 0;
  for (i=0;i<num;i++) { 
    if (which == DIVISION)
      pct[i] = (double) NET_POINTS(team_info_wi_div,teams[i]);
    else if (which == CONFERENCE)
      pct[i] = (double) NET_POINTS(team_info_wi_conf,teams[i]);
    else pct[i] = (double) NET_POINTS(team_info,teams[i]);
    if (pct[i] > best_pct) {
      best_pct = pct[i];
      num_best = 0;
    }
    if (pct[i] == best_pct) num_best++;
  }

  split_around(teams,num_best,num,pct,best_pct);
  return num_best;
}

void break_a_tie(TEAM *teams,int num,BOOL display)
{
  BOOL ALL_SAME_DIV;
  int value,i;

  if (num <= 1) return;
  
  ALL_SAME_DIV = YES;
  for (i=1;i<num;i++)
    ALL_SAME_DIV = ALL_SAME_DIV && (divisions[teams[0]][DIVISION] ==
				     divisions[teams[i]][DIVISION]);
  if (display) {
    (void) printf("------ Break a tie (all same div: %d) -------\n",
		  ALL_SAME_DIV);
    display_split(stdout,teams,num,num);
  }

  if (ALL_SAME_DIV || num == 2) {
    value = head_to_head(teams,num);
    if (value != num) {
      if (display) {
	display_split(stdout,teams,value,num);
	(void) printf("Split because of head to head record.\n");
      }
      break_a_tie(teams,value,display);
      break_a_tie(&(teams[value]),num-value,display);
      return;
    }
  } else {
    value = head_to_head_sweap(teams,num);
    if (value != num) {
      if (display) {
	display_split(stdout,teams,value,num);
	(void) printf("Split because of head to head sweap.\n");
      }
      break_a_tie(teams,value,display);
      break_a_tie(&(teams[value]),num-value,display);
      return;
    }
  }

  if (ALL_SAME_DIV) { 
    value = div_conf_rec(teams,num,DIVISION);
    if (value != num) {
      if (display) {
	display_split(stdout,teams,value,num);
	(void) printf("Split because of divisional record.\n");
      }
      break_a_tie(teams,value,display);
      break_a_tie(&(teams[value]),num-value,display);
      return;
    }
  }

  value = div_conf_rec(teams,num,CONFERENCE);
  if (value != num) {
    if (display) {
      display_split(stdout,teams,value,num);
      (void) printf("Split because of conference record.\n");
    } 
    break_a_tie(teams,value,display);
    break_a_tie(&(teams[value]),num-value,display);
    return;
  }

  value = break_common(teams,num);
  if (value != num) {
    if (display) {
      display_split(stdout,teams,value,num);
      (void) printf("Split because of common game record.\n");
    }
    break_a_tie(teams,value,display);
    break_a_tie(&(teams[value]),num-value,display);
    return;
  }


  if (ALL_SAME_DIV) { 
    value = break_net_points(teams,num,DIVISION);
    if (value != num) {
      if (display) {
	display_split(stdout,teams,value,num);
	(void) printf("Split because of division net points.\n");
      }
      break_a_tie(teams,value,display);
      break_a_tie(&(teams[value]),num-value,display);
      return;
    }
  } else {
    value = break_net_points(teams,num,CONFERENCE);
    if (value != num) {
      if (display) {
	display_split(stdout,teams,value,num);
	(void) printf("Split because of conference net points.\n");
      }
      break_a_tie(teams,value,display);
      break_a_tie(&(teams[value]),num-value,display);
      return;
    }
  }
  value = break_net_points(teams,num,NFL_0);
  if (value != num) {
    if (display) {
      display_split(stdout,teams,value,num);
      (void) printf("Split because of net points.\n");
    } 
    break_a_tie(teams,value,display);
    break_a_tie(&(teams[value]),num-value,display);
    return;
  }

    if (display)
      (void) printf("Ordered by COIN TOSS.\n");
}


void display_split(FILE *stream,TEAM *teams,int split,int num)
{
  int i;

  for (i=0;i<num;i++) {
    (void) fprintf(stream,"%s ",team[teams[i]][FULL_NAME_0]);
    if (i==(split-1) && split != num)
      (void) fprintf(stream,"  *   ");
  }
  (void) fprintf(stream,"\n");
}
