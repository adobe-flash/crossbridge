/*
 * Hexxagon board game.
 * Copyright (C) 2001 Erik Jonsson.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Email erik@nesqi.homeip.net
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <libhexx.h>

void stripFromDblSpace(char *cpfrom)
{
	char *cpto = cpfrom;
	int space = 1;
	
	while(*cpfrom)
	{
		if(!(space && (*cpfrom == ' ' || *cpfrom == '\t')))
		{
			*cpto = *cpfrom;
			cpto++;
		}
		
		if(*cpfrom == ' ' || *cpfrom == '\t')
			space = 1;
		else 
			space = 0;
		
		cpfrom++;
	}
	
	*cpto = 0;
}

void printCopy()
{
	printf("\n");
	printf("Hexxagon board game.\n");
	printf("Copyright (C) 2001 Erik Jonsson.\n\n");
	
	printf("The pieces was drawn by Stefan Påhlson.\n\n");

	printf("This program is free software; you can redistribute it and/or\n");
	printf("modify it under the terms of the GNU General Public License\n");
	printf("as published by the Free Software Foundation; either version 2\n");
	printf("of the License, or (at your option) any later version.\n\n");
	
	printf("This program is distributed in the hope that it will be useful,\n");
	printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	printf("GNU General Public License for more details.\n\n");
	
	printf("You should have received a copy of the GNU General Public License\n");
	printf("along with this program; if not, write to the Free Software\n");
	printf("Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n\n");
	
	printf("Email erik@nesqi.homeip.net\n\n");
};

void printHelp()
{
	printf("        __                              \n");
	printf("     __/  \\__                          \n");
	printf("  __/  \\__/  \\__      Hexxagon v0.3.1 \n");
	printf(" /  \\__/  \\__/  \\     Copyright 2001 \n");
	printf(" \\__/  \\__/  \\__/     Erik Jonsson   \n");
	printf(" /  \\__/  \\__/  \\                    \n");
	printf(" \\__/  \\__/  \\__/                    \n");
	printf("    \\__/  \\__/                        \n");
	printf("       \\__/                            \n\n");
	printf(" erik@nesqi.homeip.net\n"); 
	printf(" http://nesqi.homeip.net/hexxagon\n");
	printf("\n");
	printf(" Common commands:\n");
	printf("   quit                  : Quit the program.\n");
	printf("   help                  : Print this help.\n");
	printf("   copyright             : Prints copyright information.\n\n");
	printf(" Play related commands:\n");
	printf("   jump  [FROM][TO]      : Jump from eg. A4 to B4.\n");
	printf("   j     [FROM][TO]      : Same as jump.\n");
	printf("   clone [TO]            : Clone to a position.\n");
	printf("   c     [TO]            : Same as clone.\n");
	printf("   hint                  : Let the computer move for you.\n");
	printf("   board                 : Display the current playboard.\n");
	printf("   undo                  : Go gack one half move. (Undo)\n");
	printf("   redo                  : Goto the next half move. (If you have used \"undo\".)\n\n");
	printf(" Game related commands:\n");
	printf("   time [seconds]        : Max time for the computer to think.\n");
	printf("   level [level]         : Max search depth for the computer the use.\n");
	printf("   load [filename]       : Load a game from a save-file.\n");
	printf("   save [filename]       : Save a game (with history) to a file.\n");
	printf("   newgame [CC/HC/CH/HH] : Start a new game:\n");
	printf("                           HC for human against computer.\n");
	printf("                           CH for computer against human.\n"); 
	printf("                              Same as HC but computer starts.\n");
	printf("                           CC for computer against computer.\n");
	printf("                           HH for human against human.\n\n");
	printf(" If you just hit enter your last command will be repeated.\n");
	printf("\n");
}

enum
{
	 GAME_HC = 1,
	 GAME_HH
};

int parseCords(char *str)
{
	 if(((*str >= 'a' && *str <= 'i') || 
		 (*str >= 'A' && *str <= 'I')) && (str[1] >= '1' && str[1] <= '9'))
	 {
		  int x, y;

		  if(*str >= 'a' && *str <= 'i')
			   x = *str - 'a';
		  else if(*str >= 'A' && *str <= 'I')
			   x = *str - 'A';
		  else
			   return -1;

		  y = str[1] - '1';

		  return getHexxagonIndex(x + 1, y + 1);
	 }
	 
	 return -1;
}

int main(int argc, char *argv[])
{
	int quit = 0;
	
	int mode = 1;
#ifdef SMALL_PROBLEM_SIZE
        int level = 3;
#else
        int level = 4;
#endif
	int time = 12;
	
  int llvm_index;

	printf("Hexxagon board game.\n");
	printf("Copyright 2001.\n");
	printf("Erik Jonsson.\n");
	printf("Type \"copyright\" to see the copyright notice.\n\n");
	
	HexxagonGame *game = new HexxagonGame();
	game->displayText();
	
	char *input = 0;
	
	for (llvm_index = 0; llvm_index < 1; llvm_index++)
	{
          char tmp[] = "newgame CC";

		int len = strlen(tmp);
		if(len && tmp[len - 1] == ' ')
			tmp[len - 1] = 0;
		
		if(*tmp == 0)
		{
			if(input)
				printf("Repeating last command.\n");
		}
		else
		{
			input = tmp;
		}
		
		if(!input)
			continue;
		
		if(!strcasecmp("quit", input) || !strcasecmp("exit", input))
		{
			quit = 1;
		}
		else if(!strcasecmp("help", input))
			printHelp();
		else if(!strcasecmp("copyright", input))
			printCopy();
		else if(!strncasecmp("level ", input, 6))
		{
			if(!(isdigit(input[6]) && input[7] == 0))
				printf("Invalid input to command \"level\".\nValid values are 0-9.\n");
			else
				level = input[6] - '0';
		}
		else if(!strcasecmp("level", input))
			printf("The search level is set to: %i.\n", level);
		else if(!strncasecmp("time ", input, 5))
		{
			char *cp = input + 5;
			
			while(isdigit(*cp))
				cp++;
			
			if(cp == input + 5 || (*cp != 0 && !((*cp == ' ') && *(cp + 1) == 0)))
				printf("Syntax: time [number]\n");
			else
				time = atoi(input + 5);
		}
		else if(!strcasecmp("time", input))
			printf("The search time is set to %i sec.\n", time);
		else if(!strcasecmp("board", input))
			game->displayText();
		else if(!strcasecmp("undo", input))
		{
			if(game->prev())
				printf("No more moves left to undo!\n");
			else
				game->displayText();
		}
		else if(!strcasecmp("redo", input))
		{
			if(game->next())
				printf("There is no move to redo!\n");
			else
				game->displayText();
		}
		else if(!strcasecmp("hint", input))
		{
			game->computerMove(level, 0, time * 1000);
			game->displayText();
			
			if(mode != GAME_HH)
			{
				 game->computerMove(level, 0, time * 1000);
				 game->displayText();							
			} 
		}
		else if(!strncasecmp("load ", input, 5))
		{
			int ret = game->loadGame(input + 5);
			if(ret == 0)
			{
				printf("Game loaded.\n");
				game->displayText();
			}
			else if(ret == -1)
				printf("Error opening load-file!\n");
			else if(ret == -3)
				printf("Unknown file format.\n");
			else
				printf("Error reading from load-file!\n");
		}
		else if(!strcasecmp("load", input))
			printf("The load command needs a filename as argument.\n");
		else if(!strncasecmp("save ", input, 5))
	   {
		   int ret = game->saveGame(input + 5);
		   if(ret == 0)
			   printf("Game saved.\n");
		   else if(ret == -1)
			   printf("Error opening save-file!\n");
		   else
				 printf("Error writing to save-file!\n");
	   }
		else if(!strcasecmp("save", input))
			printf("The save command needs a filename as argument.\n");
		else if(!strncasecmp("newgame ", input, 8))
		{
			if(((input[8] == 'c' || input[8] == 'C') || (input[8] == 'h' || input[8] == 'H')) &&
			   ((input[9] == 'c' || input[9] == 'C') || (input[9] == 'h' || input[8] == '9')) &&
			   (input[10] == 0 || (input[10] == ' ' && input[11] == 0))) 
			{
				delete game;
				game = new HexxagonGame();
				game->displayText();
				
				if((input[8] == 'h' || input[8] == 'H') &&
					(input[9] == 'c' || input[9] == 'C'))
					mode = GAME_HC;
				else if((input[8] == 'c' || input[8] == 'C') &&
						(input[9] == 'h' || input[9] == 'H'))
				{
					game->computerMove(level, 0, time * 1000);
					game->displayText();
					mode = GAME_HC;
				}
				else if((input[8] == 'h' || input[8] == 'H') &&
						(input[9] == 'h' || input[9] == 'H'))
					mode = GAME_HH;
				else if((input[8] == 'c' || input[8] == 'C') &&
						(input[9] == 'c' || input[9] == 'C'))
				{
					while(!game->endOfGame())
					{
						game->computerMove(level, 0, time * 1000);
						game->displayText();
					}
					
					mode = GAME_HC;
				}
				else
					printf("You have found a bug in Hexxagon! This code should never be called!!\n");
			}
			else
				printf("Syntax: newgame HC/CH/CC/HH\n");
		}
		else if(!strcasecmp("newgame", input))
		{
			 printf("Syntax: newgame HC/CH/CC/HH\n");
		}
		else if(!strncasecmp("jump ", input, 5) ||
				!strncasecmp("j ", input, 2))
		{
			 char *cp = input;
			 
			 if(!strncasecmp("j ", input, 2))
				  cp += 2;
			 else
				  cp += 5;

			 HexxagonMove move;

			 int len = strlen(cp);
			 if(len == 5)
			 {
				  move.from = parseCords(cp);
				  move.to = parseCords(cp + 3);
			 } else if(len == 4)
			 {
				  move.from = parseCords(cp);
				  move.to = parseCords(cp + 2);
			 }
			 else
				  move.from = -1;

			 if(move.from != -1 && move.to != -1)
			 {
				  if(game->isMoveValid(move))
				  {
					   game->applyMove(move);
					   game->displayText();							
					   
					   if(mode != GAME_HH)
					   {
							game->computerMove(level, 0, time * 1000);
							game->displayText();							
					   } 
				  }
				  else
					   printf("Illegal move!\n");
			 } 
			 else
			 {
				  if(len == 5)
				  {
					   if(cp[2] == ' ')
							printf("Bad cordinate.\n");
					   else
							printf("Syntax: jump [FROM][TO]\nEg. \"jump a4b4\"\n");
				  }
				  else if(len == 4)
					   printf("Bad cordinate.\n");
				  else
					   printf("Syntax: jump [FROM][TO]\nEg. \"jump a4b4\"\n");
			 }
		}
		else if(!strcasecmp("jump", input) ||
				!strcasecmp("j", input))
		{
			 printf("Syntax: jump [FROM][TO]\nEg. \"jump a4b4\"\n");
		}
		else if(!strncasecmp("clone ", input, 6) ||
				!strncasecmp("c ", input, 2))
		{
			 char *cp = input;
			 
			 if(!strncasecmp("c ", input, 2))
				  cp += 2;
			 else
				  cp += 6;

			 HexxagonMove move;
			 
			 if(strlen(cp) == 2)
			 {
				  move.to = parseCords(cp);
				  move.from = move.to;

				  if(move.to != -1)
				  {
					   if(game->isMoveValid(move))
					   {
							game->applyMove(move);
							game->displayText();							
							
							if(mode != GAME_HH)
							{
								 game->computerMove(level, 0, time * 1000);
								 game->displayText();							
							} 
					   }
					   else
							printf("Illegal move!\n");
				  }
				  else
					   printf("Invalid cordinate.\n");
			 }
			 else
				  printf("Syntax: clone [TO]\nEg. \"clone b4\"\n");
			 
		}		
		else if(!strcasecmp("clone", input) ||
				!strcasecmp("c", input))
		{
			 printf("Syntax: clone [TO]\nEg. \"clone b4\"\n");
		}
		else
			printf("Unknown command, try \"help\" for a list of commands.\n");
	}
}
