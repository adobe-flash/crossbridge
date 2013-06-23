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
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
#include <sys/time.h>
#else
#include <sys/timeb.h>
#endif
#include <string.h>

using namespace std;

void HexxagonMoveList::addMove(HexxagonMove &move)
{
	if(!(nr_moves & 0xF)) /* Only check every 16:th (Extra speed) */
	{
		for(int i = NO_MOVES_BEGIN; i <= nr_moves; i *= 2)
		{
			if(nr_moves == i)
			{
				HexxagonMove *tmp = new HexxagonMove[i * 2];
				memcpy(tmp, moves, i * sizeof(class HexxagonMove));
				delete moves;
				moves = tmp;
			}
		}
	}

 	moves[nr_moves] = move;
	nr_moves++;
}

int compare(const void *mp1, const void *mp2)
{
	return ((HexxagonMove*)mp2)->score - ((HexxagonMove*)mp1)->score;
}

void HexxagonMoveList::sortListQuick()
{
	qsort(moves, nr_moves, sizeof(moves[0]), &compare);
}

void HexxagonMoveList::sortList()
{
	int move = 1;

	while(move)
	{
		move = 0;
		for(int i = 0; i < (nr_moves - 1); i++)
		{
			if(moves[i].score < moves[i+1].score)
			{
				move = 1;
				HexxagonMove tmp;
				memcpy(&tmp, &moves[i], sizeof(HexxagonMove));
				memcpy(&moves[i], &moves[i+1], sizeof(HexxagonMove));
				memcpy(&moves[i+1], &tmp, sizeof(HexxagonMove));
			} 
		}
	}
}

int getTime()
{
#if defined(__FreeBSD__) || defined(__OpenBSD__ )|| defined(__APPLE__)
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else
	struct timeb tb;
	
	ftime(&tb);
	return (tb.time * 1000) + tb.millitm;
#endif
}

HexxagonMove *HexxagonMoveList::getMove(int i)
{
	return &moves[i];
}

int HexxagonMoveList::getNrMoves()
{
	return nr_moves;
}

char hexx_count;
int alphaBeta(HexxagonBoard &board, int level, int alpha, int beta, void (*callback)())
{
	int best = -SCR_INFINITY;

	if(!level)
		return board.evaluate();

	HexxagonMoveList *moves = board.generateMoveList();

	if(!moves) // The game is over.
	{
		int score = board.countBricks(0); // Add win or loss points to score.

		if(score > 0)
			return score + SCR_WIN;
		else if(score < 0)
			return score - SCR_WIN;
		else
			return board.evaluate();
	}

	hexx_count++;
	if(!hexx_count && callback)
		callback();

	for(int i = 0; (i < moves->getNrMoves()) && (best < beta); i++)
	{
		if(best > alpha)
			alpha = best;

		HexxagonBoard newboard = HexxagonBoard(board);
		newboard.applyMove(*(moves->getMove(i)));
		int value = -alphaBeta(newboard, level - 1, -beta, -alpha, callback);

		if(value > best)
			best = value;
	}     

	delete moves;

	return best;
}

void HexxagonMoveList::scoreAllMoves(HexxagonBoard board, int depth, void (*callback)(), int maxtime)
{
	int t = getTime();

	for(int i = 1; (i < depth)/* && (getTime() - t <= maxtime) */; i++)
	{
		int best  = -SCR_INFINITY;
		int alpha = -SCR_INFINITY;
		int beta  = SCR_INFINITY;

		for(int j = 0; (j < getNrMoves()) && /* (getTime() - t <= maxtime) && */ (best < beta); j++)
		{
			if(best > alpha)
				alpha = best;

			HexxagonBoard newboard = HexxagonBoard(board);
			newboard.applyMove(*getMove(j));
			int value = -alphaBeta(newboard, i, -beta, -alpha, callback);
			
			getMove(j)->score = value;

			if(value > best)
				best = value;
		}
		
		sortList();
	}

#if 0
	cout << "Move computed in " << ((double)getTime() - t) / (double)1000 << " seconds.\n";
#endif
}

HexxagonMove *HexxagonMoveList::getBestMove()
{
	srandom(getTime());

	if(nr_moves)
		return getMove(0);
	
	return NULL;
}




