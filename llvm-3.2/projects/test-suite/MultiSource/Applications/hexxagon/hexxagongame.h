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

#ifndef _HEXXAGONGAME_H
#define _HEXXAGONGAME_H

#include "hexxagonboard.h"

class GameList : public HexxagonBoard
{
 public:
	GameList(GameList *p) {prev = p; next = 0;};
	
	GameList *next;
    GameList *prev;
};

class HexxagonGame
{
 public:
	HexxagonGame(); 
	~HexxagonGame() {destroyRest(); delete first;};

	int getTurn() {return turn;};

	int next();
	int prev();

	void reset();
	void destroyRest();

	int applyMove(HexxagonMove &move);
	int computerMove(int depth, void (*callback)(), int maxtime);

	int noBoards();
	
	HexxagonBoard *getBoard() {return (HexxagonBoard*) curr; };

	int endOfGame() {return curr->endOfGame();};
	int isMoveValid(HexxagonMove &move){ return curr->isMoveValid(move);};

	void displayText() {curr->displayBoardText(turn);};
	
	int saveGame(char *filename);
	int loadGame(char *filename);

 protected:
	int turn;

	GameList *first;
	GameList *curr;
};

#endif // _HEXXAGONGAME_H

