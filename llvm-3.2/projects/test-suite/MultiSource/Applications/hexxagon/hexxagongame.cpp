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

#include "hexxagongame.h"
#include <netinet/in.h>
#include <string.h>

extern BitBoard64 *clone_lookups; 
extern BitBoard64 *jump_lookups; 

HexxagonGame::HexxagonGame()
{
	if(!clone_lookups)
		initCloneLookups();

	if(!jump_lookups)
		initJumpLookups();

	GameList *tmp = new GameList(0);

	tmp->init(); 

	first = tmp; 
	curr = tmp;

	turn = 1;
}

int HexxagonGame::next()
{
	if(curr->next)
	{
		turn = !turn;
		curr = curr->next;
		return 0;
	} 
	
	return -1;
}

int HexxagonGame::prev()
{
	if(curr->prev)
	{
		turn = !turn;
		curr = curr->prev;
		return 0;
	}
	
	return -1;
}

void HexxagonGame::reset()
{
	while(!prev())
		;
	
	destroyRest();
}

void HexxagonGame::destroyRest()
{
	GameList *step = curr->next;
   
	while(step)
	{
		GameList *tmp = step;
		delete step;
		step = tmp->next;
	}

	curr->next = 0;
}

int HexxagonGame::applyMove(HexxagonMove &move)
{
	GameList *tmp = new GameList(curr);
	
	tmp->HexxagonBoard::operator=(*curr);
	
	int ret = tmp->applyMove(move);
		
	if(!ret)
	{
		curr->next = tmp;
		next();
	}
	else
		delete tmp;

	return ret;
}

int HexxagonGame::computerMove(int depth, void (*callback)(), int maxtime)
{
	GameList *tmp = new GameList(curr);

	tmp->HexxagonBoard::operator=(*curr);
	
	int ret = tmp->computerMove(depth, callback, maxtime);
		
	if(!ret)
	{
		curr->next = tmp;
		next();
	}
	else
		delete tmp;

	return ret;
}

//  File format
//----------------
//  
//  Hex2agon 1.0\n
//  32bit networkorder integer : boards
//  datablocks....
//  EOF

int HexxagonGame::noBoards()
{
	GameList *step = first;
	int sum = 0;
	
	while(step)
	{
		sum++;
		step = step->next;
	}

	return sum;
}

#define FILE_HDR "Hex2agon 1.1\n"

int HexxagonGame::loadGame(char *filename)
{
	FILE *loadfile;

	loadfile = fopen(filename, "rb");

	if(!loadfile)
		return -1;
	
	char str[64];
	if(fread(str, strlen(FILE_HDR), 1, loadfile) != 1)
	{
		 fclose(loadfile);
		 return -2;
	}
		
	if(memcmp(str, FILE_HDR, strlen(FILE_HDR)))
	{
		 fclose(loadfile);
		 return -3;
	}

	long int no;
	if(fread(&no, sizeof(long int), 1, loadfile) != 1)
	{
		 fclose(loadfile);
		 return -2;
	}
	
	no = ntohl(no);

	curr = first;
	destroyRest();
	delete first;
	first = 0;
	turn = 1;
	while(no)
	{
		GameList *tmp;

		if(first == 0)
		{
			tmp = new GameList(0);

			first = tmp;
			curr = tmp;
		} 
		else
		{
			tmp = new GameList(curr);
			curr->next = tmp;
			
			next();
		}

		int ret = tmp->readFromFile(loadfile);

		if(ret)
		{
			prev();
			destroyRest();
			return ret;
		}
		
		no--;
	}
	
	fclose(loadfile);

	return 0;
}

int HexxagonGame::saveGame(char *filename)
{
	FILE *savefile;

	savefile = fopen(filename, "wb");

	if(!savefile)
		return -1;

	if(fwrite(FILE_HDR, strlen(FILE_HDR), 1, savefile) != 1)
	{
		 fclose(savefile);
		 return -2;
	}

	long int no;

	no = htonl(noBoards());

	// BUG ?? The size can be different.
	if(fwrite(&no, sizeof(long int), 1, savefile) != 1)
	{
		 fclose(savefile);
		 return -2;
	}

	GameList *step = first;
	while(step)
	{
		if(step->writeToFile(savefile))
		{
			 fclose(savefile);
			 return -2;
		}

		step = step->next;
	}
	
	fclose(savefile);

	return 0;
}




