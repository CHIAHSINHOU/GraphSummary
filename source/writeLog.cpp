#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include "writeLog.h"

void getTime(char *s) {
	char buf[30];
	timeval now;
	gettimeofday(&now, NULL);
	struct tm Tm;
	localtime_r(&now.tv_sec, &Tm);
	strftime(buf, 30, "%F-%T", &Tm);
	sprintf(s, "%s %.0lf", buf, (double)now.tv_usec);
}

double timediff(timeval a, timeval b) {
    return (double)b.tv_sec-a.tv_sec+(double)(b.tv_usec-a.tv_usec)/1000000;
}

FILE * flogPtr;
int openFileFlag;
Timer logTimer;

FILE * setLog(const char * path) {
	openFileFlag = 1;
	flogPtr = fopen(path, "a");
	return flogPtr;
}
FILE * setLog(FILE * fout) {
	openFileFlag = 0;
	flogPtr = fout;
	return flogPtr;
}
void printTime(double t) {
	if ( !flogPtr )
		flogPtr = stderr;
	fprintf(flogPtr, "Time: %lf\n", t);
}
void startTime() {
	logTimer.startTime();
}
double endTime(int write) {
	double t = logTimer.endTime();
	if ( write )
		printTime(t);
	return t;
}
double endTime() {
	return endTime(1);
}
double runTime(char * cmd) {
	startTime();
	system(cmd);
	return endTime();
}
void appendLog(const char * path) {
	FILE * fin = fopen(path, "r");
	char buf[1000];
	while ( fgets(buf, 1000, fin) )
		fputs(buf, flogPtr);
	fclose(fin);
}
void cleanLog(const char * path) {
	FILE * fout = fopen(path, "w");
	fclose(fout);
}
void closeLog() {
	if ( flogPtr != stderr && flogPtr != stdout && openFileFlag )
		fclose(flogPtr);
}
