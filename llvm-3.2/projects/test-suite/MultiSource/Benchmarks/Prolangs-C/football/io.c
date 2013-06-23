/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "stats.h"
#include "sort.h"
#include "common.h"

#define MAX_RESP_LEN 50
char responce[MAX_RESP_LEN+1];
BOOL got_unused_responce = NO;

void prompt(FILE *output,FILE *input)
{
  extern char *get_responce(FILE *input);
  char *str;

  (void) fprintf(output,"Hit return to continue: ");
  str = get_responce(input);
  if (*str != '\0')
    got_unused_responce = YES; 
}

#define find_conf_rank(r,t) (divisions[t][CONFERENCE] == NFC_0 ? \
			     find_teams_rank(r,t) : \
			     find_teams_rank(r,t) - NUM_TEAMS/2)

int str_to_int(char *s)
{
  int value;
  char *temp;

  value=0;
  for (temp=s; *temp != '\0'; temp++)
    {value = value*10 + (( (*temp >= '0') && (*temp <= '9') ) ?
			 (*temp) - '0' : 0);
   }  
  return value;
}

int find_teams_rank(ORDERING rank,int team_code)
{
  int i;

  for (i=1;i<=NUM_TEAMS;i++)
    if (rank[i]==team_code) return i;
  return 0;
}

int white_space(char A)
{
  static int commentP=0;
  if (A == '%') {commentP = YES; return 1;}
  if (commentP) {
    if (A == '\n') commentP = 0;
    return 1;
  }

  if (A == ' ' || A == '\n' || A == '\t')
    return 1;
  else return 0;
}

void clear_screen(FILE *output)
{
  (void) fprintf(output,"\n\n=========================================\n");
}

void lower_case(char *str)
{
  for (;*str != '\0';str++)
    if ( 'A' <= *str && *str <= 'Z')
      *str = *str + ('a' - 'A');
}

char *skip_white_space(char *str)
{
  for (;white_space(*str) && *str != '\0'; str ++);
  return str;
}
BOOL end_of_line(char *str)
{
  return *str == '\0';
}

TEAM find_next_name(FILE *output,char *str)
{
  char *last,last_ch;
  TEAM value;

  str = skip_white_space(str);
  for (last = str; !white_space(*last); last++) {}
  last_ch = *last;
  *last = '\0';
  value = find_name(output,str);
  *last = last_ch;
  return value;
}

BOOL matches(char *str1,char *comm, int min_len_for_match)
{
  int length;

  for (length=0;!white_space(*(str1+length)) && *(str1+length) != '\0'; 
       length ++);

  if (length < min_len_for_match)
    return NO;
  else
    if (length > strlen(comm))
      return (!strncmp(str1,comm,strlen(comm))) && white_space(*(str1+strlen(comm)));
    else
      return (!strncmp(str1,comm,length));
}

char *skip_first(char *str)
{
      for (;!white_space(*str) && *str != '\0'; str ++);
      for (;white_space(*str) && *str != '\0'; str ++);
      return (str);
}

char *get_responce(FILE *input)
{
  char *resp;
  extern int in_fileP;

  if (!got_unused_responce) {
    (void) fgets(responce,MAX_RESP_LEN,input);
    if (in_fileP) (void) fprintf(stdout,"%s\n",responce);

    lower_case(responce);

    for (resp=responce+strlen(responce)-1;
	 resp >= responce && white_space(*resp);
	 resp --)
	*resp = '\0';
  }
  else got_unused_responce = NO;

  for (resp=responce; *resp != '\0' && white_space(*resp); resp++);
  return(resp);
}

int find_nth_place_team(ORDERING standings,int conf,int div,int place)
{
  int team_code;
  int next_found_is;

  next_found_is = 1;
  
  for (team_code=1; team_code <= NUM_TEAMS; team_code ++)
    if ((divisions[standings[team_code]][CONFERENCE] == conf || 
	 conf == WILD_CARD) &&
	(divisions[standings[team_code]][DIVISION] == div ||
	 div == WILD_CARD))
      if (next_found_is == place) return standings[team_code];
      else next_found_is ++;
  return 0;
}

#define PRINT_RESULT(week) \
  if (week > num_games) (void) fprintf(output, \
	"                            "); \
  else { int i,score,opp_score; \
    for (i=1;sched[week][i][HOST] != team_code && \
	 sched[week][i][VISITOR] != team_code && i <= NUM_TEAMS/2; i++) ; \
    if (i == NUM_TEAMS/2+1) \
      (void) fprintf(output,"                            "); \
    else {if (sched[week][i][VISITOR] == team_code) {\
      (void) fprintf(output,"at %-15s ", \
		     team[sched[week][i][HOST]][FULL_NAME_0]); \
      score = scores[week][i][VISITOR]; \
      opp_score = scores[week][i][HOST]; } \
      else {(void) fprintf(output,"%-15s    ", \
		     team[sched[week][i][VISITOR]][FULL_NAME_0]); \
      score = scores[week][i][HOST]; \
      opp_score = scores[week][i][VISITOR]; } \
    if (score == -1 || opp_score == -1 ) \
      (void) fprintf(output,"         "); \
    else {if (score == opp_score) \
             (void) fprintf(output,"T"); \
	  else if (score > opp_score) \
		(void) fprintf(output,"W"); \
	  else (void) fprintf(output,"L"); \
	    (void) fprintf(output,"%2d-%2d   ",score,opp_score);} \
	  }} 

void display_info(FILE *output,int team_code)
{
  int week;
  int tmp_team;

  clear_screen(output);

  (void) fprintf(output,"%s team information.\n\n",
		 team[team_code][FULL_NAME_0]);

  for (week=1;week <= 21; week ++) {
    PRINT_RESULT(week); 
    switch (week) {
    case 1:
      (void) fprintf(output,"%s %s",CONF_STR(team_code),
		     DIVISION_STR(team_code));
      break;
    case 2:
      DISP_REC_HEADER(output); break;
    case 3: case 4: case 5: case 6: case 7:
      if (tmp_team = find_nth_place_team(standings,
					 divisions[team_code][CONFERENCE],
					 divisions[team_code][DIVISION],
					 week-2))
	DISP_RECORD(output,tmp_team);
      break;
    case 9:
      (void) fprintf(output,"Record           %2d-%d-%d      %sRank %d/%d",
		     team_info[team_code].wins,
		     team_info[team_code].loses,team_info[team_code].ties,
		     (team_info[team_code].wins < 10 && 
		      team_info[team_code].loses < 10 ? "  " : " "),
		     find_conf_rank(conf_standings,team_code),
		     find_teams_rank(abs_standings,team_code));
      break;
    case 10:
      (void) fprintf(output,"Points scored   %3d [%6.1f]   Rank %d/%d",
		     team_info[team_code].points_for,
		     PER_GAME(team_info,team_info[team_code].points_for,
			      team_code),
		     find_conf_rank(offence_ranks,team_code),
		     find_teams_rank(offence_ranks_nfl,team_code));
      break;
    case 11:
      (void) fprintf(output,"Points allowed  %3d [%6.1f]   Rank %d/%d",
		     team_info[team_code].points_against,
		     PER_GAME(team_info,team_info[team_code].points_against,
			      team_code),
		     find_conf_rank(defence_ranks,team_code),
		     find_teams_rank(defence_ranks_nfl,team_code));
      break;
    case 12:
      (void) fprintf(output,"Net Points     %4d [%6.1f]   Rank %d/%d",
		     NET_POINTS(team_info,team_code),
		     PER_GAME(team_info,NET_POINTS(team_info,team_code),
			      team_code),
		     find_conf_rank(net_ranks,team_code),
		     find_teams_rank(net_ranks_nfl,team_code));
      break;
    case 14: 
      (void) fprintf(output,"Major Tie-breakers:"); break;
    case 15:
     (void) fprintf(output,"-(1/1) Head to Head"); break;
    case 16: 
      (void) fprintf(output,"-(2/ ) Divisional Record          %d-%d-%d",
		     team_info_wi_div[team_code].wins,
		     team_info_wi_div[team_code].loses,
		     team_info_wi_div[team_code].ties);
      break;
    case 17:
      (void) fprintf(output,"-(3/2) Conference Record          %d-%d-%d",
		     team_info_wi_conf[team_code].wins,
		     team_info_wi_conf[team_code].loses,
		     team_info_wi_conf[team_code].ties);
      break;
    case 18:
      (void) fprintf(output,"-(4/3) Common Games"); break;
    case 19:
      (void) fprintf(output,"-(5/ ) Net points in %s %s  %d [%3.1f]",
		     CONF_STR(team_code),DIVISION_STR(team_code),
		     NET_POINTS(team_info_wi_div,team_code),
		     PER_GAME(team_info_wi_div,
			      NET_POINTS(team_info_wi_div,team_code),
			      team_code));
      break;
    case 20:
      (void) fprintf(output,"-( /4) Avg. net points in %s     %3.1f",
		     CONF_STR(team_code),
		     PER_GAME(team_info_wi_conf,
			      NET_POINTS(team_info_wi_conf,team_code),
			      team_code));
      break;
    case 21:
      (void) fprintf(output,"-(6/5) Total net points           %d [%3.1f]",
		     NET_POINTS(team_info,team_code),
		     PER_GAME(team_info,NET_POINTS(team_info,team_code),
			      team_code));
      break;
    default: 
      break;
    }

    (void) fprintf(output,"\n");
  }
}

BOOL is_any(TEAM *teams,int num,TEAM code)
{
  int i;
  for (i=0;i<num;i++)
    if (teams[i] == code) return YES;
  return NO;
}

/* ===================================================== */
void display_multi_tiebreaker(FILE *output,TEAM *teams,int num)
{
  int i;
  BOOL same_conf,same_div;
  TEAM tmp_team,tmp_team1;

  same_conf = same_div = YES;

  if (num < 2) return;
  
  common_teams[1] = teams[0];
  for (i=1;i<num;i++) {
    common_teams[i+1] = teams[i];
    same_conf = same_conf && (divisions[teams[i]][CONFERENCE] ==
			      divisions[teams[0]][CONFERENCE]);
    same_div = same_div && (divisions[teams[i]][DIVISION] ==
			    divisions[teams[0]][DIVISION]);
  }
  if (!same_conf) {
    (void) fprintf(output,"ERROR: All teams must be from the same conference.\n");
    return;
  }
  clear_screen(output);

  if (!same_div) {
    return;
  }

  (void) fprintf(output,"                                          ");
  (void) fprintf(output,"            Div\n");
  (void) fprintf(output,"                             Div.   Conf. ");
  (void) fprintf(output,"Common      Net  Net\n");
  (void) fprintf(output,"Team           Record HtoH  Record Record ");
  (void) fprintf(output,"Games       Pts  Pts\n\n");
  for (tmp_team1=1; tmp_team1 <= NUM_TEAMS; tmp_team1 ++) {
    tmp_team= conf_standings[tmp_team1];
    if (is_any(teams,num,tmp_team)) {
      (void) fprintf(output,"%-13s %2d-%2d-%1d ",
		     team[tmp_team][FULL_NAME_0],
		     team_info[tmp_team].wins,
		     team_info[tmp_team].loses,
		     team_info[tmp_team].ties);
      {
	int wins,loses,ties;
	int tmp;

	wins = loses = ties = 0;
	for (tmp=0;tmp<num;tmp++)
	  if (teams[tmp] != tmp_team) {
	    wins += h_to_h_stats[tmp_team][teams[tmp]].wins;
	    loses += h_to_h_stats[tmp_team][teams[tmp]].loses;
	    ties += h_to_h_stats[tmp_team][teams[tmp]].ties;
	  }
	(void) fprintf(output,"%1d-%1d-%1d",wins,loses,ties);
      }      
      (void) fprintf(output," %1d-%1d-%1d",
		   team_info_wi_div[tmp_team].wins,
		   team_info_wi_div[tmp_team].loses,
		   team_info_wi_div[tmp_team].ties);
      (void) fprintf(output," %2d-%2d-%1d",
		   team_info_wi_conf[tmp_team].wins,
		   team_info_wi_conf[tmp_team].loses,
		   team_info_wi_conf[tmp_team].ties);

      common(num);
      if ( (common_games[tmp_team].wins != -1)) {
	(void) fprintf(output,"%2d-%2d-%1d[%2d]",
	       common_games[tmp_team].wins,common_games[tmp_team].loses,
	       common_games[tmp_team].ties,common_games[tmp_team].points_for);
      } else                /* 123456789012 */
	(void) fprintf(output,"            ");

      (void) fprintf(output,"%4d", NET_POINTS(team_info_wi_div,tmp_team));
      (void) fprintf(output," %4d",NET_POINTS(team_info,tmp_team));
      (void) fprintf(output,"\n");
    }
  }
}

/* ===================================================== */
void display_tiebreaker(FILE *output,TEAM team_code)
{
  TEAM tmp_team,tmp_team1;


  clear_screen(output);

  common_teams[1] = team_code;
  (void) fprintf(output,"                                          ");
  (void) fprintf(output,"Common Games Rec.[left] Div Conf\n");
  (void) fprintf(output,"                       vs.   Div.   Conf. ");
  (void) fprintf(output,"----------------------- Net  Net  Net\n");
  (void) fprintf(output,"Team           Record  %3s  Record Record ",
		 team[team_code][ABBR_0]);
  (void) fprintf(output,"   Team         %3s     Pts  Pts  Pts\n\n",
		 team[team_code][ABBR_0]);
  for (tmp_team1=1; tmp_team1 <= NUM_TEAMS; tmp_team1 ++) {
    tmp_team= conf_standings[tmp_team1];
    if (divisions[tmp_team][CONFERENCE] == divisions[team_code][CONFERENCE]){
      common_teams[2] = tmp_team;
      (void) fprintf(output,"%-13s %2d-%2d-%1d ",
		     team[tmp_team][FULL_NAME_0],
		     team_info[tmp_team].wins,
		     team_info[tmp_team].loses,
		     team_info[tmp_team].ties);
      if (team_code != tmp_team) 
	(void) fprintf(output,"%1d-%1d-%1d",
		       h_to_h_stats[tmp_team][team_code].wins,
		       h_to_h_stats[tmp_team][team_code].loses,
		       h_to_h_stats[tmp_team][team_code].ties);
      else (void) fprintf(output,"     ");
      
    if (divisions[tmp_team][DIVISION] == divisions[team_code][DIVISION])
      (void) fprintf(output," %1d-%1d-%1d",
		   team_info_wi_div[tmp_team].wins,
		   team_info_wi_div[tmp_team].loses,
		   team_info_wi_div[tmp_team].ties);
      else (void) fprintf(output,"      ");
      (void) fprintf(output," %2d-%2d-%1d",
		   team_info_wi_conf[tmp_team].wins,
		   team_info_wi_conf[tmp_team].loses,
		   team_info_wi_conf[tmp_team].ties);

      common(2);
      if ( tmp_team != team_code && (common_games[team_code].wins != -1 &&
	  common_games[tmp_team].wins != -1)) {
	(void) fprintf(output," %2d-%2d-%1d[%2d] %2d-%2d-%1d[%2d]",
	       common_games[tmp_team].wins,common_games[tmp_team].loses,
	       common_games[tmp_team].ties,common_games[tmp_team].points_for,
	       common_games[team_code].wins,common_games[team_code].loses,
	       common_games[team_code].ties,common_games[team_code].points_for
		       );
      } else                /* 123456789012345678901234 */
	(void) fprintf(output,"                        ");

      if (divisions[tmp_team][DIVISION] == divisions[team_code][DIVISION])
	(void) fprintf(output,"%4d", NET_POINTS(team_info_wi_div,tmp_team));
      else (void) fprintf(output,"    ");

      (void) fprintf(output,"%5.1f",
		     PER_GAME(team_info_wi_conf,
			      NET_POINTS(team_info_wi_conf,tmp_team),
			      tmp_team));

      (void) fprintf(output," %4d",NET_POINTS(team_info,tmp_team));
      (void) fprintf(output,"\n");

    }
  }
}
      
/* ===================================================== */
void display_records(FILE *output)
{
  TEAM tmp_team,tmp_team1;


  clear_screen(output);
  (void) fprintf(output,"                 NFC                 ");
  (void) fprintf(output,"    ");
  (void) fprintf(output,"                 AFC\n\n"); 
  (void) fprintf(output,"                        W  L  T  Pct.");
  (void) fprintf(output,"    ");
  (void) fprintf(output,"                        W  L  T  Pct.\n");
  for (tmp_team1=1; tmp_team1 <= NUM_TEAMS/2; tmp_team1 ++) {
    tmp_team = conf_standings[tmp_team1];
    (void) fprintf(output,"%2d/%2d) %-14s",tmp_team1,
		   find_teams_rank(abs_standings,tmp_team),
		   team[tmp_team][FULL_NAME_0]); 
    if (a_champ(tmp_team)) (void) fprintf(output,"*");
    else if (a_wild_card(tmp_team)) (void) fprintf(output,"+");
    else (void) fprintf(output," ");
    (void) fprintf(output," %2d-%2d-%2d %s.%03d",
		     team_info[tmp_team].wins,
		     team_info[tmp_team].loses,
		     team_info[tmp_team].ties,
		     DISP1_PCT(team_info[tmp_team]), 
		     DISP2_PCT(team_info[tmp_team]));
    (void) fprintf(output,"    ");
    tmp_team = conf_standings[tmp_team1+NUM_TEAMS/2];
    (void) fprintf(output,"%2d/%2d) %-14s",tmp_team1,
		   find_teams_rank(abs_standings,tmp_team),
		   team[tmp_team][FULL_NAME_0]);
    if (a_champ(tmp_team)) (void) fprintf(output,"*");
    else if (a_wild_card(tmp_team)) (void) fprintf(output,"+");
    else (void) fprintf(output," ");
    (void) fprintf(output," %2d-%2d-%2d %s.%03d\n",
		     team_info[tmp_team].wins,
		     team_info[tmp_team].loses,
		     team_info[tmp_team].ties,
		     DISP1_PCT(team_info[tmp_team]), 
		     DISP2_PCT(team_info[tmp_team]));
  }
  (void) fprintf(output,"\n* Division winner\n+ Wildcard\n");
}

void display_week(FILE *output,int week)
{
  int game;

  clear_screen(output);
  (void) fprintf(output,"                                    Week %d\n\n",week);

  for (game=1;game < NUM_TEAMS /2 + 2; game += 3) {
    (void) fprintf(output,"%-16s",team[sched[week][game][VISITOR]][FULL_NAME_0]);
    if (scores[week][game][VISITOR] < 0)
      (void) fprintf(output,"          ");
    else (void) fprintf(output,"  %2d      ",scores[week][game][VISITOR]);
    if (game + 1 <= NUM_TEAMS/2) {
      (void) fprintf(output,"%-16s",team[sched[week][game+1][VISITOR]][FULL_NAME_0]);
      if (scores[week][game+1][VISITOR] < 0)
	(void) fprintf(output,"          ");
      else (void) fprintf(output,"  %2d      ",scores[week][game+1][VISITOR]);
    }
    if (game + 2 <= NUM_TEAMS/2) {
      (void) fprintf(output,"%-16s",team[sched[week][game+2][VISITOR]][FULL_NAME_0]);
      if (scores[week][game+2][VISITOR] < 0)
	(void) fprintf(output,"          ");
      else (void) fprintf(output,"  %2d      ",scores[week][game+2][VISITOR]);
    }
    (void) fprintf(output,"\n");
    (void) fprintf(output,"%-16s",team[sched[week][game][HOST]][FULL_NAME_0]);
    if (scores[week][game][HOST] < 0)
      (void) fprintf(output,"          ");
    else (void) fprintf(output,"  %2d      ",scores[week][game][HOST]);
    if (game + 1 <= NUM_TEAMS/2) {
      (void) fprintf(output,"%-16s",team[sched[week][game+1][HOST]][FULL_NAME_0]);
      if (scores[week][game+1][HOST] < 0)
	(void) fprintf(output,"          ");
      else (void) fprintf(output,"  %2d      ",scores[week][game+1][HOST]);
    }
    if (game + 2 <= NUM_TEAMS/2) {
      (void) fprintf(output,"%-16s",team[sched[week][game+2][HOST]][FULL_NAME_0]);
      if (scores[week][game+2][HOST] < 0)
	(void) fprintf(output,"          ");
      else (void) fprintf(output,"  %2d       ",scores[week][game+2][HOST]);
    }
    (void) fprintf(output,"\n\n");
  }
}

void enter_prompt_week(FILE *output,FILE *input,int week)
{
  int game;
  char *resp;
  BOOL ABORT;

  clear_screen(output);
  (void) fprintf(output, "Enter blank line to leave a score unchanged, \n");
  (void) fprintf(output, "QUIT to return to menu, or new value for the score.\n");
  (void) fprintf(output,"\nWeek %d\n\n",week);

  ABORT = NO;
  for (game=1;game <= NUM_TEAMS /2; game ++) {
    if (!ABORT && sched[week][game][VISITOR] != 0 &&
	sched[week][game][HOST] != 0) {

      (void) fprintf(output,"%-16s",team[sched[week][game][VISITOR]][FULL_NAME_0]);
      if (scores[week][game][VISITOR] < 0)
	(void) fprintf(output,"         ? ");
      else (void) fprintf(output,"  %2d     ? ",scores[week][game][VISITOR]);
      resp = get_responce(input);
      if (matches(resp,"quit",1)) ABORT = YES;
      if ( *resp != '\0' && !ABORT) {
	(void) sscanf(resp,"%d",&(scores[week][game][VISITOR]));
	current_with_disk = NO;
      }

      if (!ABORT) {
	(void) fprintf(output,"%-16s",team[sched[week][game][HOST]][FULL_NAME_0]);
	if (scores[week][game][HOST] < 0)
	  (void) fprintf(output,"         ? ");
	else (void) fprintf(output,"  %2d     ? ",scores[week][game][HOST]);
	resp = get_responce(input);
	if (matches(resp,"quit",1)) ABORT = YES;
	if ( *resp != '\0' && !ABORT) {
	  (void) sscanf(resp,"%d",&(scores[week][game][HOST]));
	  current_with_disk = NO;
	}
      }
      (void) fprintf(output,"\n");
    }
  }
}

void enter_week(FILE *output,FILE *input,int week)
{
  int game;
  char *resp;
  BOOL ABORT;
  TEAM team_code;
  BOOL FOUND_GAME;
  int FIRST,SECOND;

  clear_screen(output);
  (void) fprintf(output, "Enter blank line to leave a score unchanged, \n");
  (void) fprintf(output, "QUIT to return to menu, or new value for the score.\n");
  (void) fprintf(output,"\nWeek %d\n\n",week);

  ABORT = NO;
  while (!ABORT) {
    team_code = 0;
    while (((team_code == 0) || (team_code == NONE_0)) && !ABORT) {
      (void) fprintf(output,"Enter either team: ");
      resp = get_responce(input);
      if (end_of_line(responce) || matches(responce,"quit",1))
	ABORT = YES;
      else
	team_code = find_name(output,resp);
    }

    FOUND_GAME=NO;
    for (game=1;(!ABORT && (game<=NUM_TEAMS/2) && !FOUND_GAME);game++)
	 {
	   if ((sched[week][game][HOST] == team_code) &&
	       (sched[week][game][VISITOR] != NONE_0)) {
	     FIRST = HOST;
	     SECOND = VISITOR;
	     FOUND_GAME=YES;
	   }
	   if ((sched[week][game][VISITOR] == team_code) &&
	       (sched[week][game][HOST] != NONE_0)) {
	     FIRST = VISITOR;
	     SECOND = HOST;
	     FOUND_GAME=YES;
	   }
	 }
    if (!ABORT)
      if (!FOUND_GAME) 
	(void) fprintf(output,"No game found for %s in week %d\n",
		       team[team_code][FULL_NAME_0], week);
      else {
	game --;
	(void) fprintf(output,"%-16s",team[sched[week][game][FIRST]][FULL_NAME_0]);
	if (scores[week][game][FIRST] < 0)
	  (void) fprintf(output,"         ? ");
	else (void) fprintf(output,"  %2d     ? ",scores[week][game][FIRST]);
	resp = get_responce(input);
	if (matches(resp,"quit",1)) ABORT = YES;
	if ( *resp != '\0' && !ABORT) {
	  (void) sscanf(resp,"%d",&(scores[week][game][FIRST]));
	  current_with_disk = NO;
	}
	
	if (!ABORT) {
	  (void) fprintf(output,"%-16s",team[sched[week][game][SECOND]][FULL_NAME_0]);
	  if (scores[week][game][SECOND] < 0)
	    (void) fprintf(output,"         ? ");
	  else (void) fprintf(output,"  %2d     ? ",scores[week][game][SECOND]);
	  resp = get_responce(input);
	  if (matches(resp,"quit",1)) ABORT = YES;
	  if ( *resp != '\0' && !ABORT) {
	    (void) sscanf(resp,"%d",&(scores[week][game][SECOND]));
	    current_with_disk = NO;
	  }
	}
	(void) fprintf(output,"\n");
      }
  } 
}
	 

void display_rankings(FILE *output)
{
  int t;

  clear_screen(output);

  (void) fprintf(output,"\n");
  (void) fprintf(output,"                                  Rankings\n");
  (void) fprintf(output,"                   NFC                                       AFC\n");
  (void) fprintf(output,"                 REC.  OFF.  DEF.  NET                    REC.  OFF.  DEF.  NET\n\n");
  for (t=1;t<=NUM_TEAMS/2;t++) {
    (void) fprintf(output,"%-14s%1s %2d/%-2d %2d/%-2d %2d/%-2d %2d/%-2d %-14s%1s %2d/%-2d %2d/%-2d %2d/%-2d %2d/%-2d\n",
		   team[t][FULL_NAME_0],
		   (a_champ(t) ? "*" : (a_wild_card(t) ? "+" : " ")),
		   find_teams_rank(conf_standings,t),
		   find_teams_rank(abs_standings,t),
		   find_teams_rank(offence_ranks,t),
		   find_teams_rank(offence_ranks_nfl,t),
		   find_teams_rank(defence_ranks,t),
		   find_teams_rank(defence_ranks_nfl,t),
		   find_teams_rank(net_ranks,t),
		   find_teams_rank(net_ranks_nfl,t),
		   team[t+NUM_TEAMS/2][FULL_NAME_0],
		   (a_champ(t+NUM_TEAMS/2) ? "*" : 
		    (a_wild_card(t+NUM_TEAMS/2) ? "+" : " ")),
		   find_teams_rank(conf_standings,t+NUM_TEAMS/2)-NUM_TEAMS/2,
		   find_teams_rank(abs_standings,t+NUM_TEAMS/2),
		   find_teams_rank(offence_ranks,t+NUM_TEAMS/2)-NUM_TEAMS/2,
		   find_teams_rank(offence_ranks_nfl,t+NUM_TEAMS/2),
		   find_teams_rank(defence_ranks,t+NUM_TEAMS/2)-NUM_TEAMS/2,
		   find_teams_rank(defence_ranks_nfl,t+NUM_TEAMS/2),
		   find_teams_rank(net_ranks,t+NUM_TEAMS/2)-NUM_TEAMS/2,
		   find_teams_rank(net_ranks_nfl,t+NUM_TEAMS/2));
  }
  (void) fprintf(output,"\n");
}

void display_defence(FILE *output)
{
  int t;

  clear_screen(output);

  (void) fprintf(output,"\n");
  (void) fprintf(output,"                            Defensive Rankings\n");
  (void) fprintf(output,"                NFC                              AFC\n\n");

  for (t=1;t<=NUM_TEAMS/2;t++) {
    (void) fprintf(output,"%2d/%-2d) %-15s %-5.1f %3d   %2d/%-2d) %-15s %-5.1f %3d\n",
		   t,find_teams_rank(defence_ranks_nfl,defence_ranks[t]),
		   team[defence_ranks[t]][FULL_NAME_0],
		   PER_GAME(team_info,
			    team_info[defence_ranks[t]].points_against,
			    defence_ranks[t]),
		   team_info[defence_ranks[t]].points_against,
		   t,
		   find_teams_rank(defence_ranks_nfl,
				   defence_ranks[t+NUM_TEAMS/2]),
		   team[defence_ranks[t+NUM_TEAMS/2]][FULL_NAME_0],
		   PER_GAME(team_info,
			    team_info[defence_ranks[t+NUM_TEAMS/2]].points_against,
			    defence_ranks[t+NUM_TEAMS/2]),
		   team_info[defence_ranks[t+NUM_TEAMS/2]].points_against);

  }
  (void) fprintf(output,"\n");
}

void display_offence(FILE *output)
{
  int t;

  clear_screen(output);

  (void) fprintf(output,"\n");
  (void) fprintf(output,"                            Offensive Rankings\n");
  (void) fprintf(output,"                NFC                              AFC\n\n");

  for (t=1;t<=NUM_TEAMS/2;t++) {
    (void) fprintf(output,"%2d/%-2d) %-15s %-5.1f %3d   %2d/%-2d) %-15s %-5.1f %3d\n",
		   t,find_teams_rank(offence_ranks_nfl,offence_ranks[t]),
		   team[offence_ranks[t]][FULL_NAME_0],
		   PER_GAME(team_info,team_info[offence_ranks[t]].points_for,
			    offence_ranks[t]),
		   team_info[offence_ranks[t]].points_for,
		   t,
		   find_teams_rank(offence_ranks_nfl,
				   offence_ranks[t+NUM_TEAMS/2]),
		   team[offence_ranks[t+NUM_TEAMS/2]][FULL_NAME_0],
		   PER_GAME(team_info,
			    team_info[offence_ranks[t+NUM_TEAMS/2]].points_for,
			    offence_ranks[t+NUM_TEAMS/2]),
		   team_info[offence_ranks[t+NUM_TEAMS/2]].points_for);

  }
  (void) fprintf(output,"\n");
}

void display_net(FILE *output)
{
  int t;

  clear_screen(output);

  (void) fprintf(output,"\n");
  (void) fprintf(output,"                            Net Points Rankings\n");
  (void) fprintf(output,"                NFC                              AFC\n\n");

  for (t=1;t<=NUM_TEAMS/2;t++) {
    (void) fprintf(output,"%2d/%-2d) %-15s %-5.1f %4d   %2d/%-2d) %-15s %-5.1f %4d\n",
		   t,find_teams_rank(net_ranks_nfl,net_ranks[t]),
		   team[net_ranks[t]][FULL_NAME_0],
		   PER_GAME(team_info,
			    NET_POINTS(team_info,net_ranks[t]),net_ranks[t]),
		   NET_POINTS(team_info,net_ranks[t]),
		   t,
		   find_teams_rank(net_ranks_nfl,net_ranks[t+NUM_TEAMS/2]),
		   team[net_ranks[t+NUM_TEAMS/2]][FULL_NAME_0],
		   PER_GAME(team_info,
			    NET_POINTS(team_info,net_ranks[t+NUM_TEAMS/2]),
			    net_ranks[t+NUM_TEAMS/2]),
		   NET_POINTS(team_info,net_ranks[t+NUM_TEAMS/2]));

  }
  (void) fprintf(output,"\n");
}

void display_standings(FILE *output)
{
  int t;

  clear_screen(output);

  (void) fprintf(output,
    "\n                   NFC                                      AFC\n");
  (void) fprintf(output,
    "\n                  EAST                                     EAST\n");
  DISP_REC_HEADER(output); 
  (void) fprintf(output,"   ");
  DISP_REC_HEADER(output); 
  (void) fprintf(output,"\n");

  for (t = 1 ; t <= 5; t ++) {
    DISP_RECORD(output,standings[t]);
    (void) fprintf(output,"   ");
    DISP_RECORD(output,standings[t+14]);
    (void) fprintf(output,"\n");
  }
  (void) fprintf(output,
      "\n                 CENTRAL                                  CENTRAL\n");
  for (t = 6 ; t <= 9; t ++) {
    DISP_RECORD(output,standings[t]);
    (void) fprintf(output,"   ");
    DISP_RECORD(output,standings[t+14]);
    (void) fprintf(output,"\n");
  }
  DISP_RECORD(output,standings[10]);
  (void) fprintf(output,"\n");
  (void) fprintf(output,
	  "                                                           WEST\n");
  (void) fprintf(output,"                  WEST                   ");
  DISP_RECORD(output,standings[24]);
  (void) fprintf(output,"\n");
  for (t = 11 ; t <= 14; t ++) {
    DISP_RECORD(output,standings[t]);
    (void) fprintf(output,"   ");
    DISP_RECORD(output,standings[t+14]);
    (void) fprintf(output,"\n");
  }
  (void) fprintf(output,"\n");
}

void display_team(FILE *output)
{
  int team_code;

  clear_screen(output);

  for (team_code=1; team_code <= NUM_TEAMS/2; team_code ++)
    (void) fprintf(output,"%-15s [%-3s]     %-15s [%-3s]\n",
		   team[team_code][FULL_NAME_0],team[team_code][ABBR_0],
		   team[team_code+NUM_TEAMS/2][FULL_NAME_0],
		   team[team_code+NUM_TEAMS/2][ABBR_0]);
}

void display_main_menu(FILE *output,FILE *input)
{
  char *resp;
  BOOL valid_input;

  valid_input=NO;

  if (!got_unused_responce) {
    clear_screen(output);

    (void) fprintf(output,"Enter week <num>\n");
    (void) fprintf(output,"Enter week <num> with-prompts\n");
    (void) fprintf(output,"Display all-ranks\n");
    (void) fprintf(output,"Display defensive-rank\n");
    (void) fprintf(output,"Display information-about <team-name/abbr>\n");
    (void) fprintf(output,"Display menu\n");
    (void) fprintf(output,"Display offensive-rank\n");
    (void) fprintf(output,"Display net-rank\n");
    (void) fprintf(output,"Display records\n");
    (void) fprintf(output,"Display standings\n");
    (void) fprintf(output,"Display team-names/abbr\n");
    (void) fprintf(output,"Display tie-breaker-information-for <team-name/abbr>\n");
    (void) fprintf(output,"Display week <num>\n");
    (void) fprintf(output,"How-ties-broken conference\n");
    (void) fprintf(output,"How-ties-broken division\n");
    (void) fprintf(output,"Quit\n");
    (void) fprintf(output,"Save\n");
    if (!current_with_disk)
      (void) fprintf(output,"[WARNING: Database is not current with disk.]\n");
  }

  while (!valid_input) {
    if (!got_unused_responce) 
      (void) fprintf(output,"\n                Enter option: ");

    resp = get_responce(input);

    if (matches(resp,"quit",1) || feof(input)) {
      (void) fprintf(output,"\n");
      terminate = YES;
      valid_input = YES;
    }
    if (matches(resp,"how-ties-broken",1)) {
      resp = skip_first(resp);
      if (matches(resp,"division",1)) {
	(void) fprintf(output,"\n");
	valid_input = YES;
	break_ties(team_info,standings,DIVISION,YES);
      } else if (matches(resp,"conference",1)) {
	(void) fprintf(output,"\n");
	valid_input = YES;
	break_ties(team_info,conf_standings,CONFERENCE,YES);
      }
    } else
    if (matches(resp,"display",1)) {

      resp = skip_first(resp);
      if (matches(resp,"menu",1)) 
	valid_input = YES;
      else if (matches(resp,"standings",1)) {
	valid_input = YES;
	display_standings(output);
	prompt(output,input);
      }
      else if (matches(resp,"information-about",1)) {
	int team_code;

	resp = skip_first(resp);
	if ((team_code = find_name(output,resp))) {
	  valid_input = YES;
	  display_info(output,team_code);
	  prompt(output,input);
	}
      }
      else if (matches(resp,"tie-breaker-information-for",2)) {
	TEAM code,team_code[NUM_TEAMS];
	int num,i;
	num = 0;
	resp = skip_first(resp);
	resp = skip_white_space(resp);
	while (*resp != '\0')
	  if ((code = find_next_name(output,resp))) {
	    BOOL temp;
	    temp = YES;
	    for (i=0;i<num;i++) if (team_code[i] == code) temp = NO;
	    if (temp) {
	      team_code[num] = code;
	      num ++;
	    }
	    resp = skip_first(resp);
	    resp = skip_white_space(resp);
	    if (*resp == '\0') valid_input = YES;
	  } else {*resp = '\0'; num = 0;}

	if (num == 1 && valid_input) {
	  display_tiebreaker(output,team_code[0]); 
	  prompt(output,input);
	}
	else if (valid_input) {
	  display_multi_tiebreaker(output,team_code,num); 
	  prompt(output,input);
	}
      }
      else if (matches(resp,"records",1)) {
	valid_input = YES;
	display_records(output);
	prompt(output,input);
      }
      else if (matches(resp,"all-ranks",1)) {
	valid_input = YES;
	display_rankings(output);
	prompt(output,input);
      }
      else if (matches(resp,"defensive-rank",1)) {
	valid_input = YES;
	display_defence(output);
	prompt(output,input);
      }
      else if (matches(resp,"offensive-rank",1)) {
	valid_input = YES;
	display_offence(output);
	prompt(output,input);
      }
      else if (matches(resp,"net-rank",1)) {
	valid_input = YES;
	display_net(output);
	prompt(output,input);
      }
      else if (matches(resp,"week",1)) {
	int week;

	resp = skip_first(resp);
	(void) sscanf(resp,"%d",&week);
	if (1 <= week && week <= num_games) {
	  display_week(output,week);
	  prompt(output,input);
	  valid_input = YES;
	}
      }
      else if (matches(resp,"team-name/abbr",2)) {
	valid_input = YES;
	display_team(output);
	prompt(output,input);
      }

      if (!valid_input)
	(void) fprintf(output,"Illegal display command.\n");
    }
    else
    if (matches(resp,"enter",1)) {

      resp = skip_first(resp);
      if (matches(resp,"week",1)) {
	int week;

	resp = skip_first(resp);
	(void) sscanf(resp,"%d",&week);
	if (1 <= week && week <= num_games) {
	  resp = skip_first(resp);
	  if (end_of_line(resp)) {
	    enter_week(output,input,week);
	    compute_team_info ();
	    sort_all();
	    prompt(output,input);
	    valid_input = YES;
	  } else if (matches(resp,"with-prompts",1)) {
	    enter_prompt_week(output,input,week);
	    compute_team_info ();
	    sort_all();
	    prompt(output,input);
	    valid_input = YES;
	  }
	} 
     } 
      if (!valid_input)
	(void) fprintf(output,"Illegal enter command.\n");
    }
    else
      if (matches(resp,"save",1)) {
	valid_input = YES;
	save_scores(output,input_score); 
      }

    
  }
}
