#define DEBUG_FLAG 1

#define NUM_PRESEASON_GAMES 5

#define NUM_GAMES_78_89 16
#define NUM_GAMES 17

#define MAX_NAME_LENGTH 15
#define NUM_TEAMS 28
#define MAX_TEAMS_DIV 5
#define NONE_0 NUM_TEAMS+1
#define VISITOR 1
#define HOST 2
#define BOOL int
#define YES 1
#define NO 0
#define ERROR_0 0

#define ABBR_0 0
#define FULL_NAME_0 1

typedef int TEAM;

typedef int 
   SCHED_TYPE[NUM_GAMES+1][(int) NUM_TEAMS/2 +1 ][3];

typedef int 
   SCORES_TYPE[NUM_GAMES+2][(int) NUM_TEAMS/2 +1 ][3];

typedef char
  TEAM_TYPE [NUM_TEAMS+2][2][MAX_NAME_LENGTH];

typedef struct info {
  int wins;
  int loses;
  int ties;
  int points_for;
  int points_against;
  int game[NUM_GAMES+1];
} TEAM_INFO;

typedef TEAM_INFO TEAMS_STATS[NUM_TEAMS+1];
typedef TEAM_INFO H_TO_H_STATS[NUM_TEAMS+1][NUM_TEAMS+1];

#define WILD_CARD 999
#define CONFERENCE 0
#define DIVISION 1
#define NFL_0 2
#define NFC_0 0
#define AFC_0 1
#define EAST_0 0
#define CENTRAL_0 1
#define WEST_0 2

#define CONF_STR(x) (divisions[x][CONFERENCE] == NFC_0 ? "NFC" : "AFC")
#define DIVISION_STR(x) (divisions[x][DIVISION] == EAST_0 ? "East   " : \
			 (divisions[x][DIVISION] == CENTRAL_0 ? \
						 "Central" : "West   "))

typedef int DIVISION_TYPE[NUM_TEAMS+1][2];
typedef int TEAM_ORDERING[NUM_TEAMS+1];
typedef int ORDERING[NUM_TEAMS+1];
#define WIN_PCT(A) ((A.wins+A.loses+A.ties)==0 ? 0.0 : ((A.wins + .5*A.ties)/(A.wins+A.loses+A.ties)))
#define DISP1_PCT(X) (WIN_PCT(X) >= 1.0 ? "1" : " ")
#define DISP2_PCT(X) (WIN_PCT(X) >= 1.0 ? 0 : (int) (WIN_PCT(X)*1000+.5))

#define DISP_REC_HEADER(output)   (void) fprintf(output,"                 W  L  T  Pct.  PF  PA")
#define DISP_RECORD(output,teamcode) (void) fprintf(output, \
	     "%- 14s%1s %2d %2d %2d %s.%03d %3d %3d", \
	    team[teamcode][FULL_NAME_0], \
	    (a_champ(teamcode) ? "*" : \
		   (a_wild_card(teamcode) ? "+" : " ")), \
	    team_info[teamcode].wins,team_info[teamcode].loses, \
	    team_info[teamcode].ties, \
	    DISP1_PCT(team_info[teamcode]), \
	    DISP2_PCT(team_info[teamcode]), \
	    team_info[teamcode].points_for, \
	    team_info[teamcode].points_against)

#define PER_GAME(info,N,T) ((info[T].wins + info[T].loses + info[T].ties) == 0 ? 0.0 : ((double) N / (info[T].wins + info[T].loses + info[T].ties)))

#define NET_POINTS(info,A) (info[A].points_for-info[A].points_against)

#define sort(A,lt,lower,upper,O) {int i,j; BOOL flag; int temp;\
     flag = YES; for (i=lower; i<=upper && flag; i ++) {flag = NO;\
       for (j=upper; j>i; j --) \
	 if (lt(A,O[j],O[j-1])) { \
	     temp = O[j-1]; O[j-1]=O[j]; O[j]=temp; flag = YES;}}}
