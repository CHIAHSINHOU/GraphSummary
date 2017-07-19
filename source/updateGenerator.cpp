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
#include "writeLog.h"
using namespace std;

FILE *updateLog;
FILE *updateFile;

struct Msg {
	ID A, B;
	int No;
	double w;
	Msg() {}
	Msg(int n, ID a, ID b, double _w) {
		No = n;
		A = a;
		B = b;
		w = _w;
	}
};
queue<Msg> updateQ;
vector< vector<int> > edges;
vector< vector<double> > values;
vector<int> toQueryEdge;
vector<double> toQueryValue;

void insertQ(Msg msg) {
	char s[100];
	ID A = msg.A;
	ID B = msg.B;
	double w = msg.w;
	getTime(s);
	FLOG(updateLog, "GENUPDATE %d (%d,%d) (%d,%d) %s\n", msg.No, A.groupId, A.nodeId, B.groupId, B.nodeId, s);
	updateQ.push(msg);
}

void cleanBuffer() {
	for (int i = 0; i < updateProc.num; i++) {
		edges[i].clear();
		values[i].clear();
	}
	toQueryEdge.clear();
	toQueryValue.clear();
}

void sendUpdate() {
	char s[100];

	Msg msg;
	if (updateQ.empty()) {
		return;
	}
	while (!updateQ.empty()) {
		msg = updateQ.front();
		updateQ.pop();
		int PA = graphInfo[msg.A.groupId] - updateProc.start;
		int PB = graphInfo[msg.B.groupId] - updateProc.start;
		double w = rand()%100;
		while(w == 100 || w == 0){
			w = rand()%100;
		}
		if (msg.A.groupId == msg.B.groupId) {
			edges[PA].push_back(msg.A.groupId);
			edges[PA].push_back(msg.A.nodeId);
			edges[PA].push_back(msg.B.groupId);
			edges[PA].push_back(msg.B.nodeId);
			values[PA].push_back(w);
		} else {
			toQueryEdge.push_back(msg.A.groupId);
			toQueryEdge.push_back(msg.A.nodeId);
			toQueryEdge.push_back(msg.B.groupId);
			toQueryEdge.push_back(msg.B.nodeId);
			toQueryValue.push_back(w);
		}
		getTime(s);
		FLOG(updateLog, "SENDUPDATE %d (%d,%d) (%d,%d) %lf %s\n", msg.No, msg.A.groupId, msg.A.nodeId, msg.B.groupId, msg.B.nodeId, w, s);
	}

	for (int i = 0 ; i < updateProc.num; i++) {
		if (edges[i].size() == 0) {
			continue;
		}
		int target = updateProc.start + i;
		int jobType = UPDATE_GRAPH;
		MPI_Send(&jobType, 1, MPI_INT, target, 1, MPI_COMM_WORLD);
		int edgeNum = edges[i].size()/4;
		MPI_Send(&edgeNum, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
		MPI_Send(edges[i].data(), edgeNum*4, MPI_INT, target, 0, MPI_COMM_WORLD);
		MPI_Send(values[i].data(), edgeNum, MPI_DOUBLE, target, 0, MPI_COMM_WORLD);
	}
	for (int i = 0 ; i < queryProc.num; i++) {
		if (toQueryEdge.size() == 0) {
			continue;
		}
		int target = queryProc.start + i;
		int jobType = UPDATE_GRAPH;
		MPI_Send(&jobType, 1, MPI_INT, target, 1, MPI_COMM_WORLD);
		int edgeNum = toQueryEdge.size()/4;
		MPI_Send(&edgeNum, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
		MPI_Send(toQueryEdge.data(), edgeNum*4, MPI_INT, target, 0, MPI_COMM_WORLD);
		MPI_Send(toQueryValue.data(), edgeNum, MPI_DOUBLE, target, 0, MPI_COMM_WORLD);
	}
	cleanBuffer();
}
int updateCount;
void genUpdate(){
	int a, b, c, d;
	if(fscanf(updateFile, "%d%d%d%d", &a, &b, &c, &d) == EOF)
		return;
	double w = rand()%100;
	while(w == 100 || w == 0){
		w = rand()%100;
	}
	Msg msg(updateCount++, ID(a,b), ID(c,d), w);
	insertQ(msg);
}

void updateGenerator(char *t, int G) {

	sleep(20);
	edges.resize(updateProc.num, vector<int>());
	values.resize(updateProc.num, vector<double>());

	char output[50];
	sprintf(output, "mkdir -p ../Log/%s", t);
	system(output);
	sprintf(output, "../Log/%s/updateLog", t);
	updateLog = fopen(output, "w");
	if (!updateLog) {
		fprintf(stderr, "Fail to open %s\n", output);
		return;
	}
	sprintf(output, "%s/update.in", t);
	updateFile = fopen(output, "r");

	timeval now;
	gettimeofday(&now, NULL);
	timeval start = now;
	timeval lastGen = now;
	timeval lastUpdate = now;
	updateCount = 0;
	while (1) {
		gettimeofday(&now, NULL);
		if (timediff(start,now) >= runtime) {
			break;
		}
		if (timediff(lastGen, now) >= updateGenInterval && updateQ.size() < updateProc.num*1000) {
			lastGen = now;
			genUpdate();
		}
		gettimeofday(&now, NULL);
		if (timediff(lastUpdate, now) >= updateSendInterval) {
			lastUpdate = now;
			sendUpdate();
		}
		usleep(100000);
	}
}

