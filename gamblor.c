#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _DEBUG
#define PRINTD printf
#else
#define PRINTD if (0) printf
#endif
int adj[9][9] = { {1, 1, 0, 1, 0, 0, 0, 0, 0},
{1, 1, 1, 0, 1, 0, 0, 0, 0},
{0, 1, 1, 0, 0, 1, 0, 0, 0},
{1, 0, 0, 1, 1, 0, 1, 0, 0},
{0, 1, 0, 1, 1, 1, 0, 1, 0},
{0, 0, 1, 0, 1, 1, 0, 0, 1},
{0, 0, 0, 1, 0, 0, 1, 1, 0},
{0, 0, 0, 0, 1, 0, 1, 1, 1},
{0, 0, 0, 0, 0, 1, 0, 1, 1}
};

const int numCasino = 9;
const int numDays = 30;

/*To Keep track of winnings */
int Winnings[9][30];

/* to keep track of path taken for MaxGain*/
int PathMatrix[9][30];
int MaxWinnings[9][30];

/* to keep track of max gain, day till which we bet, and casino last played */

int gMaxGain = 0;
int gDay = -1;
int gCasino = -1;

/* signifies is user is in any casino */
/* 0 says no, 1 says possible yes */
/* initially, user is prsent in casino 0 */
int CasinoPresent[9] = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
int CasinoNext[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };


/* Gain For the winnings */
int TotalGainPresent[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int TotalGainNext[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*as read from file*/
void printCasinoWinnings()
{
    for (int i = 0; i < 9; i++) {
	printf("wins%d = [ ", i + 1);
	for (int j = 0; j < 30; j++) {
	    printf("%d", Winnings[i][j]);
	    if (j != 29)
		printf(", ");
	}
	printf("]\n");
    }
}

void printPath(int presentCasino, int presentDay, bool zero)
{
    if (MaxWinnings[presentCasino][presentDay] == 0) {
	zero = true;
    }
    if (presentDay != 0) {
	int fromCasino = PathMatrix[presentCasino][presentDay];
	int day = presentDay - 1;
	printPath(fromCasino, day, zero);
    }
    if (!zero) {
	printf("Day=%d CasinoVisited=%d WinningTillNow=%d\n",
	       presentDay + 1, presentCasino + 1,
	       MaxWinnings[presentCasino][presentDay]);
    } else {
	printf("Day=%d Casino=%d :NoPlay\n", presentDay + 1,
	       presentCasino + 1);
    }
}

/* is it possible to be in casino, given the transistions so far */
bool cannotReachState(int presentCasino, int nextCasino)
{
    return ((CasinoPresent[presentCasino] == 0) ||
	    (adj[presentCasino][nextCasino] == 0));
}


/* main logic is here */
/* for each casino, see if user can reach there from last nights casino */
/* if not continue */
/* if yes, calcualte new gain for each possible transistion, and store the max */
/* if the max is more than the global max, then store in global max */

void DaysPath(int day)
{
    PRINTD("DAY : %d \n", day);
    for (int nextCasino = 0; nextCasino < numCasino; nextCasino++) {
	int maxGain = -1;
	int fromCasino = -1;
	bool nextCasinoReached = false;
	for (int presentCasino = 0; presentCasino < numCasino;
	     presentCasino++) {
	    if (cannotReachState(presentCasino, nextCasino)) {
		continue;
	    } else {
		/* calcualte gain for this traversal */
		int tmpGain =
		    TotalGainPresent[presentCasino] +
		    Winnings[nextCasino][day];

		PRINTD
		    ("Casino=%d from Casino=%d OldGain=%d Gain=%d Total=%d\n",
		     nextCasino, presentCasino,
		     TotalGainPresent[presentCasino],
		     Winnings[nextCasino][day], tmpGain);

		if (nextCasinoReached == false) {
		    maxGain = tmpGain;
		    fromCasino = presentCasino;
		} else {
		    if (tmpGain > maxGain) {
			maxGain = tmpGain;
			fromCasino = presentCasino;
		    }
		}
		nextCasinoReached = true;
	    }
	}

	/* check if the casino is reachable */
	if (nextCasinoReached == false) {
	    PRINTD("Casino=%d : NOT REACHABLE\n", nextCasino);
	    continue;
	}
	if (maxGain < 0) {	/* reset counter .i.e do not play till today */
	    maxGain = 0;
	}
	int oldGain = TotalGainNext[nextCasino];
	TotalGainNext[nextCasino] = maxGain;
	MaxWinnings[nextCasino][day] = maxGain;

	/* next casino is also visited */
	CasinoNext[nextCasino] = 1;

	/* store path */
	PathMatrix[nextCasino][day] = fromCasino;

	PRINTD("\tMax: Casino=%d From Casino=%d MaxGain=%d diff=%d\n",
	       nextCasino, fromCasino, TotalGainNext[nextCasino],
	       TotalGainNext[nextCasino] - oldGain);

	/* global maximum */
	if (maxGain > gMaxGain) {
	    gMaxGain = maxGain;
	    gDay = day;
	    gCasino = nextCasino;

	}
    }
    /* update the wininngs  + casino vistied */
    for (int i = 0; i < numCasino; i++) {
	TotalGainPresent[i] = TotalGainNext[i];
	TotalGainNext[i] = 0;
	CasinoPresent[i] = CasinoNext[i];
	CasinoNext[i] = 0;
    }
    PRINTD("\nGlobal Max: MaxGain=%d Day=%d Casino=%d\n\n", gMaxGain, gDay,
	   gCasino);
}

// a very rudimentry parser 
// fills the winning matrix
// assumes all entries of winning casino are single line 
// and start with "[" and end with "]", seperated by ","
int fillWinnings(FILE * fp, int casinoNumber)
{
    char *string = NULL;
    size_t len = 0;
    ssize_t read;
    read = getline(&string, &len, fp);
    if (read == -1) {
	PRINTD("Error in reading  wireTap names\n");
	return -1;
    }
    char *str = strstr(string, "[");
    str++;
    for (int i = 0; i < 30; i++) {
	if (i != 29) {
	    char *win = str;
	    char *newstr = strstr(str, ",");
	    newstr[0] = '\0';
	    str = newstr + 1;
	    Winnings[casinoNumber][i] = atoi(win);
	} else {
	    char *win = str;
	    char *newstr = strstr(str, "]");
	    newstr[0] = '\0';
	    Winnings[casinoNumber][i] = atoi(win);
	}
    }
    free(string);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
	printf("Error: Usage: ./a.out <filename>\n");
	return -1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
	printf("Error: could not open the file : %s\n", argv[1]);
	return -1;
    }
    /* read file to fill up the probablistic winnings from casino's */
    for (int i = 0; i < 9; i++) {
	fillWinnings(fp, i);
	for (int j = 0; j < 30; j++) {
	    PathMatrix[i][j] = 0;
	    MaxWinnings[i][j] = 0;
	}
    }
    // printCasinoWinnings(); // for debugging
    /* plan out way to maximize the eranings */
    for (int day = 0; day < 30; day++) {
	DaysPath(day);
    }
    /* print earnings & path */
    printf("\nMax Gain = %d\n\n", gMaxGain);
    printf("ROUTE:\n\n");
    printPath(gCasino, gDay, false);
    if (gDay < 29) {
	for (int d = gDay+1; d <= 29; d++) {
	    printf("Day=%d NoPlay\n", d+1);
	}
    }
    printf("\nENDROUTE\n\n");
}
