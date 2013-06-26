extern BOOL terminate;
extern int num_games;
extern int iyear;
extern BOOL a_wild_card(TEAM team);
extern BOOL a_champ(TEAM team);
extern int find_name(FILE *output,char name[]);
extern BOOL current_with_disk;
extern void save_scores(FILE *msg_stream,char *file_name);
extern char input_score[];
extern SCHED_TYPE sched;
extern SCORES_TYPE scores;
extern TEAM_TYPE team;
extern void compute_team_info(void);
extern TEAMS_STATS team_info,team_info_wi_conf,team_info_wi_div;
extern ORDERING standings,conf_standings,abs_standings;
extern ORDERING defence_ranks,defence_ranks_nfl;
extern ORDERING offence_ranks,offence_ranks_nfl;
extern ORDERING net_ranks,net_ranks_nfl;
extern DIVISION_TYPE divisions;

extern TEAM nfc_east_champ, nfc_central_champ, nfc_west_champ;
extern TEAM afc_east_champ, afc_central_champ, afc_west_champ;
extern TEAM nfc_wild_card1, nfc_wild_card2,nfc_wild_card3;
extern TEAM afc_wild_card1, afc_wild_card2,afc_wild_card3;

extern BOOL team_plays[NUM_TEAMS+1][NUM_TEAMS+1];
extern H_TO_H_STATS h_to_h_stats;
