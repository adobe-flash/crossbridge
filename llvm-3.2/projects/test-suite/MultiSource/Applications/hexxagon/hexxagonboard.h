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

#ifndef _HEXXAGONBOARD_H
#define _HEXXAGONBOARD_H

#include "bitboard64.h"

class HexxagonMove;
class HexxagonMoveList;

enum 
{
	SCR_WIN = 20000,
	SCR_INFINITY = 32000
};

/* Hexxagon type */
enum 
{
	pl1 = 1,
	pl2,
	empty,
	removed
};

int getHexxagonIndex(int x, int y);
void initCloneLookups();
void initJumpLookups();

class HexxagonBoard
{
 public:

	HexxagonBoard(void) {};
	HexxagonBoard(const HexxagonBoard &copy);

	void init(void);
	int evaluate(void);
	int countBricks(int player);
	int getHexxagon(int i);
	int applyMove(HexxagonMove &move);

	HexxagonMoveList *generateMoveList();

	int computerMove(int depth, void (*callback)(), int maxtime);
	int endOfGame();
	int isMoveValid(HexxagonMove &move);

	BitBoard64 getBBBoard() {return board;};
	BitBoard64 getBBColor() {return color;};

	HexxagonBoard operator=(const HexxagonBoard &right);

	int readFromFile(FILE *file);
	int writeToFile(FILE *file);

	void displayBoardText(int turn);

 protected:
	BitBoard64 board;
	BitBoard64 color;
};

#endif
