#include "Graph.h"
#include "Group.h"
#include "funcs.h"
#include "structs.h"
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include "job_type.h"
#include "SimpleGraph.h"
#include "writeLog.h"
#include "mpi.h"
#include "baseline.h"
using namespace std;

int terminateProc;
double queryGenInterval;
double runtime;
int answer;

FILE *fLog;
int *busy;
int PROCN;

struct Msg {
	ID A, B;
	double V;
	int type;
	int N;
	Msg() {}
	Msg(ID a, ID b, int num) {
		type = 0;
		A = a;
		B = b;
		N = num;
	}
	Msg(ID a, ID b, double value) {
		type = 1;
		A = a;
		B = b;
		V = value;
	}
};
queue<Msg> Q;
queue<Msg> buf;
Msg *msgs;
vector<int> edges;
vector<double> values;

// void insertQ(ID A, ID B) {
// 	char s[100];
// 	getTime(s);
// 	fprintf(fLog, "GENQUERY %d (%d,%d) (%d,%d) %s\n", msg.N, A.groupId, A.nodeId, B.groupId, B.nodeId, s);
// 	Q.push(Msg(A,B));
// }

int getFreeProc() {
	for (int i = 0; i < PROCN-1; i++) {
		if (!busy[i]) {
			return i;
		}
	}
	return -1;
}

void setFreeProc(int id) {
	busy[id] = 0;
}

void recvQueryResult(int id, double result) {
	MPI_Status status;
	answer++;
	char s[100];
	Msg msg = msgs[id];
	getTime(s);
	fprintf(fLog, "FINISHQUERY %d (%d,%d) (%d,%d) %lf %s\n", msg.N, msg.A.groupId, msg.A.nodeId, msg.B.groupId, msg.B.nodeId, result, s);
	setFreeProc(id);
}

void genQuery() {
	Msg msg = buf.front();
	while(msg.type==1 && !buf.empty()){
		Q.push(msg);
		buf.pop();
		msg = buf.front();
	}
	if(buf.empty())
		return;
	buf.pop();
	char s[100];
	getTime(s);
	fprintf(fLog, "GENQUERY %d (%d,%d) (%d,%d) %s\n", msg.N, msg.A.groupId, msg.A.nodeId, msg.B.groupId, msg.B.nodeId, s);
	Q.push(msg);


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


		if(msg.type==0){

			busy[id]=1;
			jobType = ANSWER_QUERY;

			MPI_Send(&jobType, 1, MPI_INT, 1+id, 1, MPI_COMM_WORLD);
			int arr[2];
			arr[0] = msg.A.groupId;
			arr[1] = msg.A.nodeId;
			MPI_Send(arr, 2, MPI_INT, 1+id, 4, MPI_COMM_WORLD);
			arr[0] = msg.B.groupId;
			arr[1] = msg.B.nodeId;
			MPI_Send(arr, 2, MPI_INT, 1+id, 5, MPI_COMM_WORLD);
			getTime(s);
			fprintf(fLog, "SENDQUERY %d (%d,%d) (%d,%d) %s\n", msg.N, msg.A.groupId, msg.A.nodeId, msg.B.groupId, msg.B.nodeId, s);
			msgs[id] = msg;
			Q.pop();
		}
		else if(msg.type==1){
			jobType = UPDATE_GRAPH;
			Q.pop();

			for(int i=0;i<PROCN-1;i++){
				id = 1 + i;
				MPI_Send(&jobType, 1, MPI_INT, id, 1, MPI_COMM_WORLD);
				fprintf(stderr, "send update to %d\n", i);
				int edgeNum = 1;
				int edges[4];
				edges[0] = msg.A.groupId;
				edges[1] = msg.A.nodeId;
				edges[2] = msg.B.groupId;
				edges[3] = msg.B.nodeId;
				MPI_Send(&edgeNum, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
				fprintf(stderr, "send edgeNum to %d\n", i, edgeNum);
				MPI_Send(edges, 4, MPI_INT, id, 2, MPI_COMM_WORLD);
				fprintf(stderr, "send edges to %d (%d, %d)->(%d,%d)\n", i, edges[0], edges[1], edges[2], edges[3]);
				MPI_Send(&msg.V, 1, MPI_DOUBLE, id, 3, MPI_COMM_WORLD);
				fprintf(stderr, "send values to %d value=%lf\n", i, msg.V);
			}
		}
	}
}

void readQuery(int caseNum){

	char fileName[100];
	sprintf(fileName,"../Log/%d/baseline.in", caseNum);
	FILE *fp;
	fp = fopen(fileName, "r");
	int type;
	Msg msg;
	int a, b, c, d;
	while(fscanf(fp, "%d", &msg.type) != EOF){
		if(msg.type == 0)
			fscanf(fp, "%d%d%d%d%d", &msg.N, &a, &b, &c, &d);
		else
			fscanf(fp, "%d%d%d%d%lf", &a, &b, &c, &d, &msg.V);
		msg.A = ID(a,b);
		msg.B = ID(c,d);
		buf.push(msg);
	}


}

void sendQuitQueryMsg() {
	int jobType = QUIT;
	for (int i = 0; i < PROCN-1; i++) {
		int id = 1 + i;
		MPI_Send(&jobType, 1, MPI_INT, id, 1, MPI_COMM_WORLD);
		fprintf(stderr, "send quit to %d\n", id);
	}
}

void queryGenerator(int caseNum, int G) {


	busy = new int[PROCN-1];
	msgs = new Msg[PROCN-1];
	fill(busy, busy+PROCN-1, 0);

	char output[50];
	MPI_Request recvReq;
	MPI_Status status;
	int flag = 0;
	int recving = 0;
	double result;
	sprintf(output, "../Log/%d/queryLog", caseNum);
	fLog = fopen(output, "w");
	if (!fLog) {
		fprintf(stderr, "Fail to open %s\n", output);
		return;
	}

	readQuery(caseNum);
	int bufSize = buf.size();
	// while (!Q.empty()) {
	// 	sendQuery();
	// 	if (!recving) {
	// 		MPI_Irecv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvReq);
	// 	}
	// 	MPI_Test(&recvReq, &flag, &status);
	// 	if (!flag) {
	// 		recving = 1;
	// 	} else {
	// 		recvQueryResult(status.MPI_SOURCE - 1);
	// 		recving = 0;
	// 	}

	// 	sleep(0.1);
	// }
	fprintf(stderr, "finish read query file\n");
	timeval now;
	gettimeofday(&now, NULL);
	timeval start = now;
	timeval lastGen = now;
	while (1) {
		gettimeofday(&now, NULL);
		if (buf.empty() && Q.empty()) {
			sendQuitQueryMsg();
			break;
		}
		// if (timediff(start, now) >= runtime && answer > bufSize/2){
		// 	sendQuitQueryMsg();
		// 	break;
		// }
		if (!buf.empty()
			&& timediff(lastGen, now) >= queryGenInterval && Q.size() < (PROCN-1)*100) {
			lastGen = now;
			genQuery();
		}

		sendQuery();

		if (!recving) {
			MPI_Irecv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvReq);
		}
		MPI_Test(&recvReq, &flag, &status);
		if (!flag) {
			recving = 1;
		} else {
			recvQueryResult(status.MPI_SOURCE - 1, result);
			recving = 0;
		}

		usleep(0.1);
	}

	terminateProc = 1;
	delete[] busy;
	delete[] msgs;
}

int getNG(int caseNum) {
	char cmd[1000];
	int N, G;
	sprintf(cmd, "../data/case%d/input.in", caseNum);
	FILE *fin = fopen(cmd, "r");
	fscanf(fin, "%d%d", &N, &G);
	fclose(fin);
	return G;
}

int main(int args, char *argv[]){


	int rank;
	answer = 0;
	MPI_Init(&args, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &PROCN);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int caseNum = atoi(argv[1]);
	queryGenInterval = atof(argv[2]);
	runtime = atof(argv[3]);
	int G = getNG(caseNum);
	if(rank==0){
		sleep(20);
		queryGenerator(caseNum, G);
	}
	else{
		answerQuery(caseNum, rank, G);
	}

	fprintf(stderr, "[ %3d] finish\n", rank);
	MPI_Finalize();

	return 0;

}
