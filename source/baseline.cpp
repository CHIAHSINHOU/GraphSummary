#include "baseline.h"
#include "SimpleGraph.h"
#include "sssp.h"
#include "Graph.h"
#include "Group.h"
#include "job_type.h"
#include "mpi.h"
#include "writeLog.h"
using namespace std;

double answer_query(SimpleGraph oriG, int *S, int *T){

	double ans = sssp(oriG, oriG.relabel(ID(S[0], S[1])), oriG.relabel(ID(T[0], T[1])));
	return ans;

}

void getOriginalGraph(SimpleGraph &oriG, int caseNum, int GN){

	char input[100];
	for (int id = 1; id <= GN; id++) {
	    Graph g;
	    Group p;
	    sprintf(input, "../data/case%d/graph/group%d.out", caseNum, id);
	    //fprintf(stderr, "answerQuery %s\n", input);
	    p.initWithFile(input);
	    sprintf(input, "../data/case%d/graph/graph%d.out", caseNum, id);
	    //fprintf(stderr, "answerQuery %s\n", input);
	    g.initWithFile(input);
		for ( int i = 0 ; i < g.innerEdge.size() ; i++ ) {
			for ( int j = 0 ; j < g.innerEdge[i].size() ; j++ ) {
			  oriG.insertEdge(ID(id, i), ID(id, g.innerEdge[i][j].B), g.innerEdge[i][j].w);
			}
		}

	    int inEdgeNum = p.inEdge.size();
	    for (int i = 0; i < inEdgeNum; i++) {
	      oriG.insertEdge(p.inEdge[i].A, ID(id, p.inEdge[i].B),
	                   p.inEdge[i].w);
	    }
	}

  oriG.mergeMultipleEdges();
}

void updateGraph(SimpleGraph &oriG, int edgeNum, int *edges, double *values){
	for(int i=0;i<edgeNum;i++){
		oriG.updateEdge(ID(edges[i*4], edges[i*4+1]), ID(edges[i*4+2], edges[i*4+3]), values[i]);
	}
	delete[] edges;
	delete[] values;
}

void answerQuery(int caseNum, int tn, int GN){

	int PROCID = tn;
	SimpleGraph oriG;
	fprintf(stderr, "read graph\n");
	getOriginalGraph(oriG, caseNum, GN);
	//oriG.print();
	time_t updateTime = 0;
	//time(&updateTime);
	int finish=0;
	while(!finish){
		fprintf(stderr, "[baseline%3d] waiting for jobs...\n", PROCID);
		JobType jobType;
		MPI_Status status;
		MPI_Recv(&jobType, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		fprintf(stderr, "[baseline%3d] receive job %d\n", PROCID, jobType);
		int source = status.MPI_SOURCE;
		int edgeNum;
		int *edges;
		double *values;
		double ans;
		int S[2], T[2];
		char s[100];
		switch (jobType) {

			case UPDATE_GRAPH:
				MPI_Recv(&edgeNum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
				//fprintf(stderr, "[baseline%3d] recieve edgeNum %s\n", PROCID, edgeNum, s);
				edges = new int[edgeNum*4];
				values = new double[edgeNum];
				MPI_Recv(edges, edgeNum*4, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
				fprintf(stderr, "[baseline%3d] recieve edges (%d,%d)->(%d,%d)\n", PROCID, edges[0], edges[1], edges[2], edges[3]);
				MPI_Recv(values, edgeNum, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, &status);
				fprintf(stderr, "[baseline%3d] recieve values %lf\n", PROCID, values[0]);
				updateGraph(oriG, edgeNum, edges, values);
				getTime(s);
				fprintf(stderr, "[baseline%3d] update graph %s\n", PROCID, s);
				break;
			case ANSWER_QUERY:
				MPI_Recv(S, 2, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
				MPI_Recv(T, 2, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
				//fprintf(stderr, "answerQuery recv Source: %d %d Target: %d %d\n", S[0], S[1], T[0], T[1]);
				ans = answer_query(oriG, S, T);
				getTime(s);
				fprintf(stderr, "[baseline%3d] send ans (%d, %d) -> (%d, %d) = %lf %s\n", PROCID, S[0], S[1], T[0], T[1], ans, s);
				MPI_Send(&ans, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
				break;
			case QUIT:
				fprintf(stderr, "[baseline%3d] quit!\n",PROCID);
				finish=1;
				break;

		}
	}
}