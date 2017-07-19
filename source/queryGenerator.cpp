#include "Graph.h"
#include "Group.h"
#include "funcs.h"
#include "structs.h"
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include "procInfo.h"
#include "job_type.h"
#include "SimpleGraph.h"
#include "BigGraph.h"
#include "queryGenerator.h"
#include "writeLog.h"
using namespace std;

FILE *fLog;
int *busy;

struct Msg {
	ID A, B;
	int No;
	Msg() {}
	Msg(ID a, ID b, int no) {
		A = a;
		B = b;
		No = no;
	}
};
queue<Msg> Q;
Msg *msgs;
int queryNo;
FILE *queryFile;

void insertQ(ID A, ID B) {
	char s[100];
	getTime(s);
	FLOG(fLog, "GENQUERY %d (%d,%d) (%d,%d) %s\n", queryNo, A.groupId, A.nodeId, B.groupId, B.nodeId, s);
	Q.push(Msg(A,B,queryNo++));
}

int getFreeProc() {
	for (int i = 0; i < queryProc.num; i++) {
		if (!busy[i]) {
			return i;
		}
	}
	return -1;
}

void setFreeProc(int id) {
	busy[id] = 0;
}

int allFree() {
	for (int i = 0; i < queryProc.num; i++) {
		if (busy[i]) {
			return 0;
		}
	}
	return 1;
}

void recvQueryResult(int id, double result) {
	MPI_Status status;
	char s[100];
	Msg msg = msgs[id];
	getTime(s);
	FLOG(fLog, "FINISHQUERY %d (%d,%d) (%d,%d) %lf %s\n", msg.No, msg.A.groupId, msg.A.nodeId, msg.B.groupId, msg.B.nodeId, result, s);
	setFreeProc(id);
}

void genQuery() {

	int a, b, c, d, n;
	if(fscanf(queryFile, "%d%d%d%d%d", &n, &a, &b, &c, &d) == EOF){
		rewind(queryFile);
		fscanf(queryFile,"%*d");
		fscanf(queryFile, "%d%d%d%d%d", &n, &a, &b, &c, &d);
	}
	ID A(a,b);
	ID B(c,d);
	insertQ(A,B);
}
void sendQuery() {
	char s[100];
	int jobType = ANSWER_QUERY;
	int id;

	while (-1 != (id = getFreeProc())) {
		Msg msg;
		if (Q.empty()) {
			return;
		}
		msg = Q.front();
		Q.pop();

		busy[id] = 1;

		MPI_Send(&jobType, 1, MPI_INT, queryProc.start+id, 1, MPI_COMM_WORLD);
		int arr[2];
		arr[0] = msg.A.groupId;
		arr[1] = msg.A.nodeId;
		MPI_Send(arr, 2, MPI_INT, queryProc.start+id, 0, MPI_COMM_WORLD);
		arr[0] = msg.B.groupId;
		arr[1] = msg.B.nodeId;
		MPI_Send(arr, 2, MPI_INT, queryProc.start+id, 2, MPI_COMM_WORLD);
		getTime(s);
		FLOG(fLog, "SENDQUERY %d (%d,%d) (%d,%d) %s\n", msg.No, msg.A.groupId, msg.A.nodeId, msg.B.groupId, msg.B.nodeId, s);
		msgs[id] = msg;
	}
}

void sendQuitQueryMsg() {
	int jobType = QUIT;
	for (int i = 0; i < queryProc.num; i++) {
		int id = queryProc.start + i;
		MPI_Send(&jobType, 1, MPI_INT, id, 1, MPI_COMM_WORLD);
	}
}
void sendQuitUpdateMsg() {
	int jobType = QUIT;
	for (int i = 0; i < updateProc.num; i++) {
		int id = updateProc.start + i;
		MPI_Send(&jobType, 1, MPI_INT, id, 1, MPI_COMM_WORLD);
	}
}

void queryGenerator(char *t, int G) {
	//SimpleGraph SG;
	//initBigGraph(caseNum, SG, G);
	//int num = SG.size();
	sleep(20);

	busy = new int[queryProc.num];
	msgs = new Msg[queryProc.num];
	fill(busy, busy+queryProc.num, 0);

	char output[50];
	MPI_Request recvReq;
	MPI_Status status;
	int flag = 0;
	int recving = 0;
	double result;
	sprintf(output, "mkdir -p ../Log/%s", t);
	system(output);
	sprintf(output, "../Log/%s/queryLog", t);
	fLog = fopen(output, "w");
	if (!fLog) {
		fprintf(stderr, "Fail to open %s\n", output);
		return;
	}
	sprintf(output, "%s/graph/query.in", t);
	queryFile = fopen(output, "r");
	fscanf(queryFile,"%*d");

	timeval now;
	gettimeofday(&now, NULL);
	timeval start = now;
	timeval lastGen = now;

	int terminateProc = 0;
	queryNo = 1;
	while (1) {
		gettimeofday(&now, NULL);
		if (timediff(start, now) >= runtime) {
			if (!terminateProc) {
				sendQuitQueryMsg();
				terminateProc = 1;
			}
			if (allFree()) {
				sendQuitUpdateMsg();
				break;
			}
		}
		if (!terminateProc
			&& timediff(lastGen, now) >= queryGenInterval && Q.size() < queryProc.num*100) {
			lastGen = now;
			genQuery();
		}
		if (!terminateProc)
			sendQuery();

		if (!recving) {
			MPI_Irecv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvReq);
		}
		MPI_Test(&recvReq, &flag, &status);
		if (!flag) {
			recving = 1;
		} else {
			recvQueryResult(status.MPI_SOURCE - queryProc.start, result);
			recving = 0;
		}

		usleep(10000);
	}
	terminateProc = 1;
	delete[] busy;
	delete[] msgs;
}
