#include "omp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <mpi.h>
#include "procInfo.h"
using namespace std;

double updateSendInterval = 1;
double cachetime = 1;

procInfo receiver, queryProc, updateProc;

vector<int> summaryProcId;
vector<int> graphInfo;
vector< vector<int> > graphId;

void initProcInfo(int G) {
	receiver.start = 0;
	queryProc.start = receiver.start + receiver.num;
	updateProc.start = queryProc.start + queryProc.num;

	int avgG = (G+updateProc.num-1)/updateProc.num;
	graphInfo.resize(G+1);
	graphId.resize(updateProc.num+1);
	int k = 1;

	for(int i=0; i<updateProc.num; i++)
		summaryProcId.push_back(i+updateProc.start);

	for(int i=0; i<avgG; i++){
		for(int j=0; k<=G && j < updateProc.num; j++){
			int pid = j+updateProc.start;
			graphInfo[k] = pid;
			graphId[j].push_back(k);
			k++;
		}
	}

}

ProcType getProcType(int rank) {
	if (rank == 0)
		return RECEIVER;
	if (rank < updateProc.start)
		return QUERY_PROC;
	if (rank < updateProc.start+updateProc.num)
		return UPDATE_PROC;
}

