#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <mpi.h>
#include <getopt.h>
#include "procInfo.h"
#include "writeLog.h"
#include "constraints.h"
#include "answerQuery.h"
#include "receiveQuery.h"
using namespace std;

FILE *flog;
int N, G, K;

int Rand(int a, int b) { return rand() % (b - a + 1) + a; }

void getNG(char *casePath) {
	char cmd[1000];
	sprintf(cmd, "%s/input.in", casePath);
	FILE *fin = fopen(cmd, "r");
	fscanf(fin, "%d%d", &N, &G);
	fclose(fin);
}

int main(int args, char *argv[]) {
	srand(time(NULL));

	int numTask, rank;
	int provided;
	flog = stderr;
	int opt;
	int port = 1234;
	char testcasePath[200];
	testcasePath[0] = '\0';
	enum opt_val {
		opt_cachetime,
		opt_updateSendInterval,
	};
	struct option longopts[] = {
		{"testcase", required_argument, NULL, 't'},
		{"updateProc", required_argument, NULL, 'u'},
		{"queryProc", required_argument, NULL, 'q'},
		{"port", required_argument, NULL, 'p'},
		{"cachetime", required_argument, NULL, opt_cachetime},
		{"updateSendInterval", required_argument, NULL, opt_updateSendInterval},
		{0, 0, 0, 0},
	};
	receiver.num = 1;
	updateProc.num = 1;
	queryProc.num = 1;
	while ((opt = getopt_long(args, argv, "t:u:q:p:h", longopts, NULL)) != -1) {
		switch (opt) {
		case 't':
			sscanf(optarg, "%s", testcasePath);
			break;
		case 'u':
			updateProc.num = atoi(optarg);
			break;
		case 'q':
			queryProc.num = atoi(optarg);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case opt_cachetime:
			sscanf(optarg, "%lf", &cachetime);
			break;
		case opt_updateSendInterval:
			sscanf(optarg, "%lf", &updateSendInterval);
			break;
		case 'h':
			fprintf(stderr, "\t-t\ttestcase path\n");
			fprintf(stderr, "\t-u\tupdate processor\n");
			fprintf(stderr, "\t-q\tquery processor\n");
			fprintf(stderr, "--cachetime\n");
			fprintf(stderr, "--updateSendInterval\n");
			return 0;
		}
	}

	if (testcasePath[0] == '\0') {
		fprintf(stderr, "These args are necessary\n");
		fprintf(stderr, "--testcase / -t\n");
		return 0;
	}

	MPI_Init_thread(&args, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &numTask);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	getNG(testcasePath);
	initProcInfo(G);
	ProcType jobType = getProcType(rank);
	fprintf(flog, "----- Case %s start -------------------\n", testcasePath);
	switch (jobType) {
	case UPDATE_PROC:
		fprintf(flog, "Update processor %d start.\n", rank - updateProc.start);
		fprintf(stderr, "graphId[%d] size=%d\n", rank - updateProc.start, graphId[rank - updateProc.start].size());
		calculateConstraints(testcasePath, graphId[rank - updateProc.start].data(), graphId[rank - updateProc.start].size(), rank);
		break;
	case QUERY_PROC:
		fprintf(stderr, "Query processor %d start.\n", rank - queryProc.start);
		answerQuery(testcasePath, rank);
		break;
	case RECEIVER:
		fprintf(stderr, "Receiver start.\n", testcasePath);
		receiveQuery(rank, port, testcasePath);
		break;
	}

Exit:
	MPI_Finalize();

	return 0;
}
