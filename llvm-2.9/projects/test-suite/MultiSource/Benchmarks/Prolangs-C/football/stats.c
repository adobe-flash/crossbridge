/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "io.h"
#include "sort.h"

#define MAX_FILE_NAME_LEN 100

BOOL preseason = NO;
int num_games;

BOOL terminate = NO;

FILE *schedule;
#define SCHED_0 "data/sched.19"
char inp_sched[MAX_FILE_NAME_LEN+1];

FILE *the_scores;
#define SCORE_0 "data/scores.19"
char input_score[MAX_FILE_NAME_LEN+1];

SCHED_TYPE sched;
SCORES_TYPE scores;
TEAMS_STATS team_info,team_info_wi_conf,team_info_wi_div;
BOOL team_plays[NUM_TEAMS+1][NUM_TEAMS+1];
H_TO_H_STATS h_to_h_stats;

BOOL current_with_disk;

TEAM_TYPE team= {{"error","   "},
                       {"min","Minnesota"},
		       {"atl","Atlanta"},
		       {"chi","Chicago"},
		       {"dal","Dallas"},
		       {"det","Detroit"},
		       {"gb","Green Bay"},
		       {"ram","L.A. Rams"},
		       {"no","New Orleans"},
		       {"gia","N.Y. Giants"},
		       {"phi","Philadelphia"},
		       {"pho","Phoenix"},
		       {"sf","San Francisco"},
		       {"tb","Tampa Bay"},
		       {"was","Washington"},
		       {"buf","Buffalo"},
		       {"cin","Cincinnati"},
		       {"cle","Cleveland"},
		       {"den","Denver"},
		       {"hou","Houston"},
		       {"ind","Indianapolis"},
		       {"kc","Kansas City"},
		       {"rai","L.A. Raiders"},
		       {"mia","Miami"},
		       {"ne","New England"},
		       {"jet","N.Y. Jets"},
		       {"pit","Pittsburgh"},
		       {"sd","San Diego"},
		       {"sea","Seattle"},
		       {"none","None"}};

DIVISION_TYPE divisions = {{0,0}, {NFC_0,CENTRAL_0}, {NFC_0,WEST_0},
		 {NFC_0,CENTRAL_0}, {NFC_0,EAST_0}, {NFC_0,CENTRAL_0},
		 {NFC_0,CENTRAL_0}, {NFC_0,WEST_0}, {NFC_0,WEST_0},
 		 {NFC_0,EAST_0}, {NFC_0,EAST_0}, {NFC_0,EAST_0},
		 {NFC_0,WEST_0}, {NFC_0,CENTRAL_0}, {NFC_0,EAST_0},
		 {AFC_0,EAST_0}, {AFC_0,CENTRAL_0}, {AFC_0,CENTRAL_0},
		 {AFC_0,WEST_0}, {AFC_0,CENTRAL_0}, {AFC_0,EAST_0},
		 {AFC_0,WEST_0}, {AFC_0,WEST_0}, {AFC_0,EAST_0},
  	         {AFC_0,EAST_0}, {AFC_0,EAST_0}, {AFC_0,CENTRAL_0},
		 {AFC_0,WEST_0}, {AFC_0,WEST_0}};

ORDERING standings,conf_standings,abs_standings;
ORDERING defence_ranks,defence_ranks_nfl;
ORDERING offence_ranks,offence_ranks_nfl;
ORDERING net_ranks,net_ranks_nfl;

TEAM nfc_east_champ, nfc_central_champ, nfc_west_champ;
TEAM afc_east_champ, afc_central_champ, afc_west_champ;
TEAM nfc_wild_card1, nfc_wild_card2,nfc_wild_card3;
TEAM afc_wild_card1, afc_wild_card2,afc_wild_card3;

int iyear;

BOOL a_wild_card(TEAM team)
{
  return (team == nfc_wild_card1 || team == nfc_wild_card2 ||
	  team == nfc_wild_card3 || team == afc_wild_card1 ||
	  team == afc_wild_card2 || team == afc_wild_card3);
}

BOOL a_champ(TEAM team)
{
  return (team == nfc_east_champ || team == nfc_central_champ || 
	  team == nfc_west_champ || team == afc_east_champ ||
	  team == afc_central_champ || team == afc_west_champ);
}

int find_name(FILE *output,char name[])
{
  int i;
  char full_name[MAX_NAME_LENGTH];

  for (i = 1;i<=NUM_TEAMS+1;i++) {
    (void) strcpy(full_name,team[i][FULL_NAME_0]);
    lower_case(full_name);
    if (! strcmp(team[i][ABBR_0],name) || !strcmp(full_name,name)) 
      return i;
  }
 (void) fprintf(output,"No such team %s\n",name);
  return 0;
}

void compute_team_info(void) 
{
  int week,game,team,i;

  for (team=1;team<=NUM_TEAMS; team++)
    for (i=1;i<=NUM_TEAMS;i++)
      team_plays[team][i] = 0;

  for (team=1;team<=NUM_TEAMS; team++) {
    team_info[team].wins = team_info[team].loses = team_info[team].ties = 0;
    team_info[team].points_for = team_info[team].points_against = 0;

    team_info_wi_conf[team].wins = 0;
    team_info_wi_conf[team].loses = 0;
    team_info_wi_conf[team].ties = 0;
    team_info_wi_conf[team].points_for = 0;
    team_info_wi_conf[team].points_against = 0;

    team_info_wi_div[team].wins = 0;
    team_info_wi_div[team].loses = 0;
    team_info_wi_div[team].ties = 0;
    team_info_wi_div[team].points_for = 0;
    team_info_wi_div[team].points_against = 0;

    for (i=1;i<=NUM_TEAMS; i++) {
      h_to_h_stats[team][i].wins = h_to_h_stats[team][i].loses =
	h_to_h_stats[team][i].ties = h_to_h_stats[team][i].points_for =
	  h_to_h_stats[team][i].points_against = 0;
    }
  }

  for (week=1;week<=num_games; week++) 
    for (game=1;game<=NUM_TEAMS/2; game++) {  
      team_plays[sched[week][game][HOST]][sched[week][game][VISITOR]] = YES;
      team_plays[sched[week][game][VISITOR]][sched[week][game][HOST]] = YES;
      if (scores[week][game][HOST] >= 0 && scores[week][game][VISITOR] >=0) {

	h_to_h_stats[sched[week][game][HOST]]
	  [sched[week][game][VISITOR]].points_for += scores[week][game][HOST];
	h_to_h_stats[sched[week][game][HOST]]
	  [sched[week][game][VISITOR]].points_against += 
	    scores[week][game][VISITOR];
	h_to_h_stats[sched[week][game][VISITOR]]
	  [sched[week][game][HOST]].points_for += scores[week][game][VISITOR];
	h_to_h_stats[sched[week][game][VISITOR]]
	  [sched[week][game][HOST]].points_against += 
	    scores[week][game][HOST];

	if (scores[week][game][HOST] < scores[week][game][VISITOR]) {
	  team_info[sched[week][game][HOST]].loses ++;
	  team_info[sched[week][game][VISITOR]].wins ++;

	  h_to_h_stats[sched[week][game][HOST]]
	    [sched[week][game][VISITOR]].loses ++;
	  h_to_h_stats[sched[week][game][VISITOR]]
	    [sched[week][game][HOST]].wins ++;

	  if (divisions[sched[week][game][HOST]][CONFERENCE] ==
	      divisions[sched[week][game][VISITOR]][CONFERENCE]) {
	    team_info_wi_conf[sched[week][game][HOST]].loses ++;
	    team_info_wi_conf[sched[week][game][VISITOR]].wins ++;
	    if (divisions[sched[week][game][HOST]][DIVISION] ==
		divisions[sched[week][game][VISITOR]][DIVISION]) {
	      team_info_wi_div[sched[week][game][HOST]].loses ++;
	      team_info_wi_div[sched[week][game][VISITOR]].wins ++;
	    }
	  }
	}
	else
	if (scores[week][game][HOST] > scores[week][game][VISITOR]) {
	  team_info[sched[week][game][HOST]].wins ++;
	  team_info[sched[week][game][VISITOR]].loses ++;

	  h_to_h_stats[sched[week][game][HOST]]
	    [sched[week][game][VISITOR]].wins ++;
	  h_to_h_stats[sched[week][game][VISITOR]]
	    [sched[week][game][HOST]].loses ++;

	  if (divisions[sched[week][game][HOST]][CONFERENCE] ==
	      divisions[sched[week][game][VISITOR]][CONFERENCE]) {
	    team_info_wi_conf[sched[week][game][HOST]].wins ++;
	    team_info_wi_conf[sched[week][game][VISITOR]].loses ++;

	    if (divisions[sched[week][game][HOST]][DIVISION] ==
		divisions[sched[week][game][VISITOR]][DIVISION]) {
	      team_info_wi_div[sched[week][game][HOST]].wins ++;
	      team_info_wi_div[sched[week][game][VISITOR]].loses ++;
	    }
	  }
	}
	else {
	  team_info[sched[week][game][HOST]].ties ++;
	  team_info[sched[week][game][VISITOR]].ties ++;

	  h_to_h_stats[sched[week][game][HOST]]
	    [sched[week][game][VISITOR]].ties ++;
	  h_to_h_stats[sched[week][game][VISITOR]]
	    [sched[week][game][HOST]].ties ++;

	  if (divisions[sched[week][game][HOST]][CONFERENCE] ==
	      divisions[sched[week][game][VISITOR]][CONFERENCE]) {
	    team_info_wi_conf[sched[week][game][HOST]].ties ++;
	    team_info_wi_conf[sched[week][game][VISITOR]].ties ++;

	    if (divisions[sched[week][game][HOST]][DIVISION] ==
		divisions[sched[week][game][VISITOR]][DIVISION]) {
	      team_info_wi_div[sched[week][game][HOST]].ties ++;
	      team_info_wi_div[sched[week][game][VISITOR]].ties ++;
	    }
	  }
	}
	team_info[sched[week][game][HOST]].points_for +=
	  scores[week][game][HOST];
	team_info[sched[week][game][HOST]].points_against +=
	  scores[week][game][VISITOR];
	team_info[sched[week][game][VISITOR]].points_for +=
	  scores[week][game][VISITOR];
	team_info[sched[week][game][VISITOR]].points_against +=
	  scores[week][game][HOST];

	if (divisions[sched[week][game][HOST]][CONFERENCE] ==
	    divisions[sched[week][game][VISITOR]][CONFERENCE]) {
	  team_info_wi_conf[sched[week][game][HOST]].points_for +=
	    scores[week][game][HOST];
	  team_info_wi_conf[sched[week][game][HOST]].points_against +=
	    scores[week][game][VISITOR];
	  team_info_wi_conf[sched[week][game][VISITOR]].points_for +=
	    scores[week][game][VISITOR];
	  team_info_wi_conf[sched[week][game][VISITOR]].points_against +=
	    scores[week][game][HOST];

	  if (divisions[sched[week][game][HOST]][DIVISION] ==
	      divisions[sched[week][game][VISITOR]][DIVISION]) {
	    team_info_wi_div[sched[week][game][HOST]].points_for +=
	      scores[week][game][HOST];
	    team_info_wi_div[sched[week][game][HOST]].points_against +=
	      scores[week][game][VISITOR];
	    team_info_wi_div[sched[week][game][VISITOR]].points_for +=
	      scores[week][game][VISITOR];
	    team_info_wi_div[sched[week][game][VISITOR]].points_against +=
	      scores[week][game][HOST];
	  }
	}
      }
    }
}

void read_sched_into(FILE *STREAM,SCHED_TYPE sched)
{
  BOOL error;
  int num_seen[NUM_GAMES+1];
  int team_code,week_num,temp;
  int AWAY,HOME,TEAM_FIELD,opponent,OPPON_FIELD,find_game;
  char CH;
  char str[10];

  error = NO;
  for (week_num=1;week_num<=num_games;week_num++)
    num_seen[week_num] = 1;

  for (team_code=1;team_code<=NUM_TEAMS;team_code++) {
    for (week_num=1;week_num<=num_games;week_num++) {
      CH = (char) getc(STREAM);
      while (white_space(CH)) CH = (char) getc(STREAM);
      temp = 0;
      while (!(white_space(CH))) {
	str[temp] = CH;
	temp ++;
	CH = (char) getc(STREAM);
      }
      str[temp] = 0;
      if (str[0] != '@') {
	HOME = team_code;
	TEAM_FIELD = HOST;
	OPPON_FIELD = VISITOR;
	AWAY = opponent = find_name(stderr,str);
      }
      else {
	AWAY = team_code;
	TEAM_FIELD = VISITOR;
	OPPON_FIELD = HOST;
	HOME = opponent = find_name(stderr,str+1);
      }
      if (opponent < team_code) /* already read in opponents information */
      {
	for (find_game = 1; find_game<=num_seen[week_num]-1;find_game++) {
	  if (sched[week_num][find_game][TEAM_FIELD] == team_code) {
	    if (sched[week_num][find_game][OPPON_FIELD] != opponent) {
	      (void) printf("Error in opponent for %s(%d). Expected %s(%d), Found %s(%d). Week #%d\n"
			    ,team[team_code][FULL_NAME_0], team_code,
			    team[sched[week_num][find_game][OPPON_FIELD]][FULL_NAME_0],
			    sched[week_num][find_game][OPPON_FIELD],
			    team[opponent][FULL_NAME_0],opponent,week_num);
	      error = YES;
	    }
	    find_game = num_seen[week_num] + 2;
	  }
	}
	if (find_game == num_seen[week_num]) {
	  (void) printf("%s(%d) not found yet: team: %s(%d); week# %d; opp. str: %s\n", 
		 team[opponent][FULL_NAME_0],opponent,team[team_code][FULL_NAME_0],
		 team_code,week_num,str);
	  error = YES;
	}
      }
      else if (opponent != NUM_TEAMS + 1) {
	int first_time;

	first_time = 0;
	for (find_game = 1; find_game<=num_seen[week_num]-1;find_game++) {
	  if (sched[week_num][find_game][VISITOR] == team_code)
	    	    first_time = sched[week_num][find_game][HOST];
	  if (sched[week_num][find_game][HOST] == team_code)
	    	    first_time = sched[week_num][find_game][VISITOR];
	}
	if (first_time) {
	  (void) printf("%s(%d) has opponent %s(%d), but wants %s(%d); \n          week# %d\n",
			team[team_code][FULL_NAME_0],team_code,
			team[first_time][FULL_NAME_0],first_time,
			team[opponent][FULL_NAME_0],opponent,week_num);
	  error = YES;
	} 
	else if ( num_seen[week_num] > NUM_TEAMS/2) {
	  (void) printf("ERROR: Week %d, to many games seen.\n",week_num);
	  error = YES;
	} else {
	  sched[week_num][num_seen[week_num]][VISITOR] = AWAY;
	  sched[week_num][num_seen[week_num]][HOST] = HOME;
	  num_seen[week_num] ++;
	}
      }
    }
  }

if (error) exit(1);
}

void save_scores (FILE *msg_stream,char *file_name)
{
  int game,week;
  FILE *OUTPUT;

  if (!(OUTPUT = fopen(file_name,"w"))) {
    (void) fprintf(msg_stream,"Can not write to %s. Not saved.\n",file_name);
  } else {
    for (week=1;week<=num_games+1; week++) {
      for (game=1;game<=NUM_TEAMS/2; game++) {
	(void) fprintf(OUTPUT," %d %d",scores[week][game][HOST],
		      scores[week][game][VISITOR]); 
      }
      (void) fprintf(OUTPUT,"\n");
    }
    (void) fclose(the_scores);
    (void) fprintf(msg_stream,"Saved in %s.\n",file_name);
    current_with_disk = YES;
  }
}

int in_fileP = 0;

int main(int argc, char *argv[])
{
  BOOL argument_error = NO;
  char *year;
  FILE *in_file;

  {int i;
   for (i=1;i<=NUM_TEAMS;i++) {
     standings[i]=i;
     conf_standings[i]=abs_standings[i]=i;
     defence_ranks[i]=defence_ranks_nfl[i]=i;
     offence_ranks[i]=offence_ranks_nfl[i]=i;
     net_ranks[i]=net_ranks_nfl[i]=i;
   }
 }

  if (argc < 2) argument_error = YES;
  else if (!strcmp(argv[1],"-p")) {
      if (argc > 4) argument_error = YES;
      preseason=YES;
      year = argv[2];
      iyear = str_to_int(year);
      num_games = NUM_PRESEASON_GAMES;
      if (argc == 4) in_fileP = 3;
  }
  else if (argc <= 3) {
      year = argv[1];
      iyear = str_to_int(year);
      num_games = ( ((iyear >=78) && (iyear <= 89)) ? 
		   NUM_GAMES_78_89 : NUM_GAMES);
      if (argc == 3) in_fileP = 2;
  }
  else argument_error = YES;

  if (argument_error) {
    (void) fprintf(stderr,"Usage: stats [-p] <yy> [input-file]\n   <yy> is 2 digit year code.\n");
    exit(1);
  }

  if (iyear <= 87) {
    (void) strcpy(team[11][0],"sl");
    (void) strcpy(team[11][1],"St. Louis");
  }

  (void) strcpy(inp_sched,SCHED_0);
  (void) strcat(inp_sched,year);

if (preseason)
  (void) strcat(inp_sched,".pre");

  if (! (schedule = fopen(inp_sched,"r"))) {
    (void) fprintf(stderr,"ERROR: Can not open %s.\n",inp_sched);
    exit(1);
  }

  read_sched_into(schedule,sched);
  (void) fclose(schedule);

  if (in_fileP) {
      in_file = fopen(argv[in_fileP],"r");
      if (!in_file) {
	  (void) fprintf(stderr,"ERROR: Can not open input file %s.\n",
			 argv[in_fileP]);
	  exit(1);
      }
  }

  (void) strcpy(input_score,SCORE_0);
  (void) strcat(input_score,year);

if (preseason)
  (void) strcat(input_score,".pre");

  if (! (the_scores = fopen(input_score,"r"))) {
    int game,week;

    for (week=1;week<=num_games+1; week++) 
      for (game=1;game<=NUM_TEAMS/2; game++) {
	scores[week][game][HOST] = -1;
	scores[week][game][VISITOR] = -1;
      }
    (void) fprintf(stderr,"No score file. Creating one.\n");
    save_scores (stdout,input_score); 
    current_with_disk = NO;
  } else {
    int game,week;

    for (week=1;week<=num_games+1; week++) 
      for (game=1;game<=NUM_TEAMS/2; game++) {
	if (feof(the_scores)) {
	  (void) fprintf(stderr,"ERROR: %s is not a legally formated score file.\n",input_score);
	  exit(1);
	}
	(void) fscanf(the_scores,"%d %d",&(scores[week][game][HOST]),
		      &(scores[week][game][VISITOR]));
      }
    (void) fclose(the_scores);
    current_with_disk = YES;
  }
  compute_team_info ();
  sort_all ();

  while ( !terminate ) {
      if (in_fileP)
	  display_main_menu(stdout,in_file);
      else display_main_menu(stdout,stdin);
  }
  if (in_fileP) (void) fclose(in_file);
  return 0;
}
