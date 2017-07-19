#ifndef PROCINFO_h
#define PROCINFO_h
#include "omp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <mpi.h>
using namespace std;

extern double updateSendInterval;
extern double cachetime;

struct procInfo {
	int start;
	int num;
};

extern procInfo receiver, updateProc, queryProc;

extern vector<int> summaryProcId;
extern vector<int> graphInfo;
extern vector< vector<int> > graphId;

typedef enum {
	RECEIVER,
	UPDATE_PROC,
	QUERY_PROC,
} ProcType;

void initProcInfo(int G);
ProcType getProcType(int rank);
#endif
