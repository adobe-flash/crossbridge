/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated
** All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
** terms of the Adobe license agreement accompanying it.  If you have received this file from a
** source other than Adobe, then your use, modification, or distribution of it requires the prior
** written permission of Adobe.
*/
#define A 0
#define B 1

typedef struct HockeyPlayer
{
	int jersey;
	int goals;
} HockeyPlayer;

int plus(int a, int b);
int plusPtr(int* a, int* b);
int stringLength(char* str);
int stringLength2(char str[]);
int sumArray(int numbers[4]);
int sumGrid(int grid[3][3]);
int examineBytes(const unsigned char* buffer, int bufferSize);

void changeStats(HockeyPlayer* p);
void incrementEachCharacter(char* str);
void encode(char** out, int* outsize, char* input);
