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

#include "hexxagonboard.h"
#include "hexxagonmove.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

BitBoard64 *clone_lookups = 0; 
BitBoard64 *jump_lookups = 0; 

int getHexxagonIndex(int x, int y)
{
	int no;

	if(x > 0 && x < 10 && y > 0 && y < 10)
	{
		if(y < 5)
			if(x > (9 - (5 - y)))
				return -1;

		if(y > 5)
			if(x <= (y - 5))
				return -1;
	  
		no = x+y*9 - 10;
	  
		if(y > 1) no -= 4;
		if(y > 2) no -= 3;
		if(y > 3) no -= 2;
		if(y > 4) no -= 1;
		if(y > 5) no -= 1;
		if(y > 6) no -= 2;
		if(y > 7) no -= 3;
		if(y > 8) no -= 4;
	  
		return no;
	}

	return -1;
}

void initCloneLookups()
{
	clone_lookups = new BitBoard64[61];
	
	int no = 0;

	for(int i = 0; i < 61; i++)
		clone_lookups[i] = BitBoard64(0,0);

	// Clone. 
	for(int y = 1; y <= 9; y++)
	{
		for(int x = 1; x <= 9; x++)
		{
			int out = 0;
			
			if(x > 0 && x < 10 && y > 0 && y < 10)
			{
				if(y < 5)
					if(x > (9 - (5 - y)))
						out = 1;
				
				if(y > 5)
					if(x <= (y - 5))
						out = 1;
			}
			
			if(!out)
			{
				clone_lookups[no].setBit(getHexxagonIndex(x-1, y-1));
				clone_lookups[no].setBit(getHexxagonIndex(x  , y-1));
				clone_lookups[no].setBit(getHexxagonIndex(x-1, y  ));
				clone_lookups[no].setBit(getHexxagonIndex(x+1, y  ));
				clone_lookups[no].setBit(getHexxagonIndex(x  , y+1));
				clone_lookups[no].setBit(getHexxagonIndex(x+1, y+1));
				no++;
			}
		} 
	}
}

void initJumpLookups()
{
	jump_lookups = new BitBoard64[61];
	
	int no = 0;

	for(int i = 0; i < 61; i++)
		jump_lookups[i] = BitBoard64(0,0);

	// Jump.
	for(int y = 1; y <= 9; y++)
	{
		for(int x = 1; x <= 9; x++)
		{
			int out = 0;
			
			if(x > 0 && x < 10 && y > 0 && y < 10)
			{
				if(y < 5)
					if(x > (9 - (5 - y)))
						out = 1;
				
				if(y > 5)
					if(x <= (y - 5))
						out = 1;
			}
			
			if(!out)
			{
				jump_lookups[no].setBit(getHexxagonIndex(x-2, y-2));
				jump_lookups[no].setBit(getHexxagonIndex(x-1, y-2));
				jump_lookups[no].setBit(getHexxagonIndex(  x, y-2));
				jump_lookups[no].setBit(getHexxagonIndex(x-2, y-1));
				jump_lookups[no].setBit(getHexxagonIndex(x+1, y-1));
				jump_lookups[no].setBit(getHexxagonIndex(x-2, y  ));
				jump_lookups[no].setBit(getHexxagonIndex(x+2, y  ));
				jump_lookups[no].setBit(getHexxagonIndex(x-1, y+1));
				jump_lookups[no].setBit(getHexxagonIndex(x+2, y+1));
				jump_lookups[no].setBit(getHexxagonIndex(x  , y+2));
				jump_lookups[no].setBit(getHexxagonIndex(x+1, y+2));
				jump_lookups[no].setBit(getHexxagonIndex(x+2, y+2));
				no++;
			}
		}
	}
}

HexxagonBoard::HexxagonBoard(const HexxagonBoard &copy)
{
	board = copy.board;
	color = copy.color;
}

HexxagonBoard HexxagonBoard::operator=(const HexxagonBoard &right)
{
	color = right.color;
	board = right.board;

	return *this;
}

void HexxagonBoard::init(void)
{
	board = BitBoard64(0,0);
	color = BitBoard64(0,0);

	board.setBit( 0); color.setBit(0);
    board.setBit(34); color.setBit(34);
	board.setBit(56); color.setBit(56);
	board.setBit( 4);
	board.setBit(26);
	board.setBit(60);
}

int HexxagonBoard::countBricks(int player)
{
	int good = 0, bad = 0;

	for(int i = 0; i < 61; i++)
	{
		if(board.getBit(i))
		{
			if(color.getBit(i))
				good++;
			else
				bad++;
		}
	}

	if(player == 0)
		return good - bad;
	else if(player == 1)
		return good;
	else if(player == 2)
		return bad;
	
	return 0;
}

int HexxagonBoard::evaluate(void)
{
	int good = 0; 
	int bad = 0;
	int count = 0;

	for(int i = 0; i < 61; i++)
	{
		if(board.getBit(i))
		{
			count++;
			if(color.getBit(i))
				good++;
			else
				bad++;
		}
	}
    
	int score = good - bad;
	
	if(good == 0 || bad == 0 || count == 61) /* Game is over... */
	{
		if(good == 0)
			score -= SCR_WIN;

		if(bad == 0)
			score += SCR_WIN;

		if(count == 61)
		{
			if(good > bad)
				score += SCR_WIN;
			
			if(good <= bad)
				score -= SCR_WIN;
		}
	}

	return score;
}

int HexxagonBoard::getHexxagon(int i)
{
	if(board.getBit(i))
	{
		if(color.getBit(i))
			return pl1;
		else
			return pl2;
	} else
		return empty;
}

int HexxagonBoard::applyMove(HexxagonMove &move)
{
	board.setBit(move.to);
	color.setBit(move.to);
		
	color = color | clone_lookups[move.to];

	if(move.from != move.to) // Jump.
		board.unSetBit(move.from);
	
	color = ~color;

	return 0;
}

int HexxagonBoard::isMoveValid(HexxagonMove &move)
{
	 BitBoard64 mine = color & board;

	 if(!board.getBit(move.to))
	 {
		  if(move.from == move.to) // clone
		  {
			   if(mine & clone_lookups[move.to])
					return 1;
		  }
		  else // Jump
		  {
			   BitBoard64 tmp(0,0);
			   tmp.setBit(move.from);

			   if(tmp & jump_lookups[move.to] & mine)
					return 1;
		  }
	 }
		  
	 return 0;
}

HexxagonMoveList *HexxagonBoard::generateMoveList()
{
	HexxagonMoveList *list = new HexxagonMoveList();

	if(!list)
	{
		printf("Out of memory !\n");
		exit(-1);
	}

	for(int i = 0; i < 61; i++)
	{
		if(!board.getBit(i)) // Found place to clone / jump.
		{
			if(color.operator&((board.operator&(clone_lookups[i])))) // Clone.
			{
				HexxagonMove move = HexxagonMove(i);
				list->addMove(move);
			}
			
			BitBoard64 moves;
			if((moves = ((board & jump_lookups[i]) & color))) // Jump.
			{
				for(int j = 0; j < 61; j++)
				{
					if(moves.getBit(j))
					{
						HexxagonMove move = HexxagonMove(j, i);
						list->addMove(move);
					}
				}
			}
		}
	}

	if(list->getNrMoves())
		return list;

	delete list;
	return 0;
}

int HexxagonBoard::endOfGame()
{
	HexxagonMoveList *movelist;
	
	movelist = generateMoveList();

	if(!movelist)
		return 1;

	delete movelist;
	
	return 0;
}

int HexxagonBoard::computerMove(int depth, void (*callback)(), int maxtime)
{
	HexxagonMoveList *movelist;

	if(!(movelist = generateMoveList()))
	{
		printf("No more moves.\n");
		return -1;
	} 

	movelist->scoreAllMoves(*this, depth, callback, maxtime);
	applyMove(*(movelist->getBestMove()));
	
	delete movelist;
	
	return 0;
}

int HexxagonBoard::readFromFile(FILE *file)
{
	if(color.readFromFile(file) || board.readFromFile(file))
		return -1;
	else
		return 0;
}

int HexxagonBoard::writeToFile(FILE *file)
{
	if(color.writeToFile(file) || board.writeToFile(file))
		return -1;
	else
		return 0;
}

void HexxagonBoard::displayBoardText(int turn)
{
	BitBoard64 c(color);

	if(!turn)
		c = ~c;

	cout << "         A B C D E F G H I\n";
	cout << "        / / / / / / / / /\n";

	for(int y = 1; y < 10; y++)
	{
		cout << y << "- ";

		if(y < 5)
		{
			for(int off = 0; off < (5 - y); off++)
				cout << " ";
		}

		for(int x = 1; x < 10; x++)
		{
			int no;

			if((no = getHexxagonIndex(x, y)) == -1)
				cout << " ";
			else 
			{
				if(board.getBit(no))
				{
					if(c.getBit(getHexxagonIndex(x, y)))
						cout << "x ";
					else
						cout << "o ";
				} else
					cout << ". ";
			}
		}
		cout << "\n";
	}
	
	int empty = 61 - ((turn ? countBricks(1) : countBricks(2)) + 
					  (turn ? countBricks(2) : countBricks(1)));
	
	cout << "\nBricks: x " << (turn ? countBricks(1) : countBricks(2));
	cout << ", o " << (turn ? countBricks(2) : countBricks(1));
	cout <<	". Empty " << empty << ".\n";
	cout << "Next to move: " << (turn ? "x" : "o");
	cout << (endOfGame() ? ", Game over." : "") << "\n";
}
