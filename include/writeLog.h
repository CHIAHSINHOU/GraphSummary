#ifndef writeLog_h
#define writeLog_h

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

#define FLOG(fptr, args...) \
	do { \
		fprintf(fptr, args); \
		fprintf(stderr, args); \
	} while (0)


void getTime(char *s);
double timediff(timeval a, timeval b);
struct Timer {
	timeval start, end;
	void startTime() {
		gettimeofday(&start, NULL);
	}
	double endTime() {
		gettimeofday(&end, NULL);
		return timediff(start, end);
	}
};

extern FILE * flogPtr;
extern int openFileFlag;
extern Timer logTimer;

FILE * setLog(const char * path);
FILE * setLog(FILE * fout);
void printTime(double t);
void startTime();
double endTime(int write);
double endTime();
double runTime(char * cmd);
void appendLog(const char * path);
void cleanLog(const char * path);
void closeLog();
#endif
