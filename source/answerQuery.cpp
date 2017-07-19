#include "answerQuery.h"
#include "SimpleGraph.h"
#include "mpi.h"
#include "Group.h"
#include "job_type.h"
#include "sssp.h"
#include "procInfo.h"
#include "writeLog.h"
#include <time.h>
#include <math.h>

#define THEDA 0.2


int qPROCID;
void getPathRequest(int *from, SimpleGraph &oriG, int target, vector<int> *pathRequest){

	while(from[target] != target){

		if(oriG.groupIdOf(target) == oriG.groupIdOf(from[target])){
			if(oriG.path.find(make_pair(from[target], target)) == oriG.path.end()){
				pathRequest[graphInfo[oriG.groupIdOf(target)] - updateProc.start].push_back(oriG.groupIdOf(target));
				pathRequest[graphInfo[oriG.groupIdOf(target)] - updateProc.start].push_back(oriG.nodeIdOf(from[target]));
				pathRequest[graphInfo[oriG.groupIdOf(target)] - updateProc.start].push_back(oriG.nodeIdOf(target));
			}
		}
		target = from[target];
	}

}

void insertPath(int *queryTmp, int *perfixTmp, int *path, int size, SimpleGraph &oriG){


	for(int i=0;i<size;i+=3){

		int groupId = queryTmp[i];
		int fromId = queryTmp[i+1];
		int targetId = queryTmp[i+2];
		std::vector<int> v;
		fprintf(stderr, "insert ((%d, %d))", oriG.relabel(ID(groupId, fromId)), oriG.relabel(ID(groupId, targetId)));

		for(int j=perfixTmp[i/3];j<perfixTmp[i/3+1];j++){
			fprintf(stderr, "%d->", path[j]);
			v.push_back(path[j]);
		}
		fprintf(stderr, "\n");
		oriG.path[make_pair(oriG.relabel(ID(groupId, fromId)), oriG.relabel(ID(groupId, targetId)))] = v;

	}
}

void getAnsPath(int *from, SimpleGraph &oriG, int target, vector<int> &ansPath){
	if(from[target] != target){
		getAnsPath(from, oriG, from[target],ansPath);
		if(oriG.groupIdOf(target) == oriG.groupIdOf(from[target])){
			std::vector<int> v = oriG.path[make_pair(from[target], target)];
			fprintf(stderr, " %d->%d size: %d", from[target], target, v.size());
			for(int i=0;i<v.size();i++){
				ansPath.push_back(oriG.groupIdOf(target));
				ansPath.push_back(v[i]);
				fprintf(stderr, "(%d, %d)->", oriG.groupIdOf(target),v[i]);
			}
		}
		else{
			ansPath.push_back(oriG.groupIdOf(target));
			ansPath.push_back(oriG.nodeIdOf(target));
		}
	}
	else{
		ansPath.push_back(oriG.groupIdOf(target));
		ansPath.push_back(oriG.nodeIdOf(target));
	}
	fprintf(stderr, "((%d, %d))->", oriG.groupIdOf(target), oriG.nodeIdOf(target));
	return;
}
double answer_query(SimpleGraph &oriG, int *S, int *T, vector<int> &ansPath){

	MPI_Status status;
	fprintf(stderr, "%d %d\n", S[0], T[0]);
	int S_procId = graphInfo[S[0]];
	int T_procId = graphInfo[T[0]];
	JobType jobsource = ASK_CONSTRAINTS_WITH_SOURCE;
	JobType jobtarget = ASK_CONSTRAINTS_WITH_TARGET;
	fprintf(stderr, "[q%3d] send S to [u%3d]\n", qPROCID, S_procId);
	MPI_Send(&jobsource, 1, MPI_INT, S_procId, 1, MPI_COMM_WORLD);
	MPI_Send(S, 2, MPI_INT, S_procId, 2, MPI_COMM_WORLD);

	fprintf(stderr, "[q%3d] send T to [u%3d]\n", qPROCID, T_procId);
	MPI_Send(&jobtarget, 1, MPI_INT, T_procId, 1, MPI_COMM_WORLD);
	MPI_Send(T, 2, MPI_INT, T_procId, 3, MPI_COMM_WORLD);

	int edgeNum, edgeNum1;

	fprintf(stderr, "answerQuery waiting for source constraits %d\n", S_procId);
	MPI_Recv(&edgeNum, 1, MPI_INT, S_procId, 3, MPI_COMM_WORLD, &status);
	fprintf(stderr, "answerQuery recv source edgeNum\n");
	int *edges = new int[edgeNum*2];
	double *values = new double[edgeNum];
	int *pathPrefix = new int[edgeNum+1];
	MPI_Recv(edges, edgeNum*2, MPI_INT, S_procId, 3, MPI_COMM_WORLD, &status);
	fprintf(stderr, "answerQuery recv source edges\n");
	MPI_Recv(values, edgeNum, MPI_DOUBLE, S_procId, 3, MPI_COMM_WORLD, &status);
	MPI_Recv(pathPrefix, edgeNum+1, MPI_INT, S_procId, 3, MPI_COMM_WORLD, &status);
	int *paths = new int[pathPrefix[edgeNum]];
	MPI_Recv(paths, pathPrefix[edgeNum], MPI_INT, S_procId, 3, MPI_COMM_WORLD, &status);
	usleep(30000);
	fprintf(stderr, "answerQuery recv source values\n");
	fprintf(stderr, "answerQuery recv source constraits\n");
	for(int i=0;i<edgeNum;i++){
		//fprintf(stderr, "insert source constraits %d %d %lf\n", edges[i*2], edges[i*2+1], values[i]);
		oriG.insertEdge(ID(S[0], edges[i*2]), ID(S[0], edges[i*2+1]), (int)values[i]);
		vector<int> path;
		fprintf(stderr, "%d %d %d", S[0], edges[i*2], edges[i*2+1]);
		fprintf(stderr, "source ((%d-> %d))", oriG.relabel(ID(S[0], edges[i*2])), oriG.relabel(ID(S[0], edges[i*2+1])));
		fprintf(stderr, " _%d %d_ ", pathPrefix[i], pathPrefix[i+1]);
		for(int j=pathPrefix[i];j<pathPrefix[i+1];j++){
			fprintf(stderr, "%d->", paths[j]);
			path.push_back(paths[j]);
		}
		fprintf(stderr, "\n");
		oriG.path[make_pair(oriG.relabel(ID(S[0], edges[i*2])),
					oriG.relabel(ID(S[0], edges[i*2+1])))]= path;
	}
	fprintf(stderr, "answerQuery waiting for target constraits %d\n", T_procId);
	MPI_Recv(&edgeNum1, 1, MPI_INT, T_procId, 4, MPI_COMM_WORLD, &status);
	fprintf(stderr, "answerQuery recv target edgeNum\n");
	int *edges1 = new int[edgeNum1*2];
	double *values1 = new double[edgeNum1];
	int *pathPrefix1 = new int[edgeNum1+1];

	MPI_Recv(edges1, edgeNum1*2, MPI_INT, T_procId, 4, MPI_COMM_WORLD, &status);
	fprintf(stderr, "answerQuery recv target edges\n");
	MPI_Recv(values1, edgeNum1, MPI_DOUBLE, T_procId, 4, MPI_COMM_WORLD, &status);
	fprintf(stderr, "answerQuery recv target values\n");
	MPI_Recv(pathPrefix1, edgeNum1+1, MPI_INT, T_procId, 4, MPI_COMM_WORLD, &status);
	fprintf(stderr, "answerquery recv pathPrefix1 \n");
	int *paths1 = new int[pathPrefix1[edgeNum1]];
	MPI_Recv(paths1, pathPrefix1[edgeNum1], MPI_INT, T_procId, 4, MPI_COMM_WORLD, &status);
	usleep(30000);
	fprintf(stderr, "answerQuery recv target constraits\n");
	for(int i=0;i<edgeNum1;i++){
				//fprintf(stderr, "insert target constraits %d %d %lf\n", edges[i*2], edges[i*2+1], values[i]);
		oriG.insertEdge(ID(T[0], edges1[i*2]), ID(T[0], edges1[i*2+1]), values1[i]);
		vector<int> path;
		for(int j=pathPrefix1[i];j<pathPrefix1[i+1];j++){
			path.push_back(paths1[j]);
		}
		oriG.path.insert(
			make_pair(
				make_pair(oriG.relabel(ID(T[0], edges1[i*2])),
					oriG.relabel(ID(T[0], edges1[i*2+1]))), path));
	}

	if(S[0] == T[0]){
		JobType jobType = ASK_CONSTRAINTS_WITH_S_T;
		MPI_Send(&jobType, 1, MPI_INT, graphInfo[S[0]], 1, MPI_COMM_WORLD);
		int tmp[3];
		tmp[0] = S[0];
		tmp[1] = S[1];
		tmp[2] = T[1];
		double w;
		MPI_Send(tmp, 3, MPI_INT, graphInfo[S[0]], 5, MPI_COMM_WORLD);
		MPI_Recv(&w, 1, MPI_DOUBLE, graphInfo[S[0]], 6, MPI_COMM_WORLD, &status);
		if(w != inf){
			oriG.insertEdge(ID(S[0], S[1]), ID(T[0], T[1]), w);
			int size;
			MPI_Recv(&size, 1, MPI_INT, graphInfo[S[0]], 6, MPI_COMM_WORLD, &status);
			int pathTmp[size];
			MPI_Recv(pathTmp, size, MPI_INT, graphInfo[S[0]], 6, MPI_COMM_WORLD, &status);
			vector<int> path;
			for(int i=0;i<size;i++)
				path.push_back(pathTmp[i]);
			oriG.path[make_pair(oriG.relabel(ID(S[0], S[1])), oriG.relabel(ID(T[0], T[1])))] = path;
		}
	}
	//oriG.print();
	int *from = new int[oriG.size()+1];
	double ans = ssspSimple(oriG, oriG.relabel(ID(S[0], S[1])), oriG.relabel(ID(T[0], T[1])), from);
	vector<int> pathRequest[summaryProcId.size()];
	getPathRequest(from, oriG, oriG.relabel(ID(T[0], T[1])), pathRequest);
	for(int i=0;i<summaryProcId.size();i++){
		if(pathRequest[i].size()!=0){
			JobType jobType = ASK_PATH;
			MPI_Send(&jobType, 1, MPI_INT, summaryProcId[i], 1, MPI_COMM_WORLD);
			int size = pathRequest[i].size();
			MPI_Send(&size, 1, MPI_INT, summaryProcId[i], 4, MPI_COMM_WORLD);
			MPI_Send(pathRequest[i].data(), size, MPI_INT, summaryProcId[i], 4, MPI_COMM_WORLD);
		}
	}

	for(int i=0;i<summaryProcId.size();i++){
		if(pathRequest[i].size()!=0){
			int queryTmp[pathRequest[i].size()];
			int perfixTmp[pathRequest[i].size()/3+1];
			int *path, pathSize;
			MPI_Recv(queryTmp, pathRequest[i].size(), MPI_INT, summaryProcId[i], 5, MPI_COMM_WORLD, &status);
			MPI_Recv(perfixTmp, pathRequest[i].size()/3+1, MPI_INT, summaryProcId[i], 5, MPI_COMM_WORLD, &status);
			pathSize = perfixTmp[pathRequest[i].size()/3];
			path = (int*)malloc(sizeof(int)*pathSize);
			MPI_Recv(path, pathSize, MPI_INT, summaryProcId[i], 5, MPI_COMM_WORLD, &status);
			insertPath(queryTmp, perfixTmp, path, pathRequest[i].size(), oriG);
		}
	}

	getAnsPath(from, oriG, oriG.relabel(ID(T[0], T[1])), ansPath);

	for(int i=0;i<edgeNum;i++){
		//fprintf(stderr, "insert source constraits %d %d %lf\n", edges[i*2], edges[i*2+1], values[i]);
		oriG.removeEdge(ID(S[0], edges[i*2]), ID(S[0], edges[i*2+1]));

	}
		for(int i=0;i<edgeNum1;i++){
				//fprintf(stderr, "insert target constraits %d %d %lf\n", edges[i*2], edges[i*2+1], values[i]);
		oriG.removeEdge(ID(T[0], edges1[i*2]), ID(T[0], edges1[i*2+1]));
	}

	oriG.path.clear();
	delete[] edges;
	delete[] values;
	delete[] edges1;
	delete[] values1;
	return ans;

}

void getOriginalGraph(SimpleGraph &oriG, char *t){

	char input[100];
	for (int id = 1; id < graphInfo.size(); id++) {
	    Graph g;
	    //sprintf(input, "%s/graph/group%d.out", t, id);
	    //fprintf(stderr, "answerQuery %s\n", input);
	    //p.initWithFile(input);
	    sprintf(input, "%s/graph/graph%d.out", t, id);
	    //fprintf(stderr, "answerQuery %s\n", input);
	    g.initWithFile(input);
	    // if ( (double)g.edgeSize/p.shortcutNum > THEDA){
	    //   int SN = p.S.size();
	    //   int TN = p.T.size();
	    //   for (int i = 0; i < SN; i++) {
	    //     for (int j = 0; j < TN; j++) {
	    //       if (p.S[i] != p.T[j] && p.distance[i][j] != inf) {
	    //         oriG.insertEdge(ID(id, p.S[i]), ID(id, p.T[j]), p.distance[i][j]);
	    //       }
	    //     }
	    //   }
	    // }
	    // else{
	    //   for ( int i = 0 ; i < g.innerEdge.size() ; i++ ) {
	    //     for ( int j = 0 ; j < g.innerEdge[i].size() ; j++ ) {
	    //       oriG.insertEdge(ID(id, i), ID(id, g.innerEdge[i][j].B), g.innerEdge[i][j].w);
	    //     }
	    //   }
	    // }

	    int inEdgeNum = g.inEdge.size();
	    for (int i = 0; i < inEdgeNum; i++) {
	      oriG.insertEdge(g.inEdge[i].A, ID(id, g.inEdge[i].B),
	                   g.inEdge[i].w);
	    }
	}
}

void delay(time_t &updateTime, SimpleGraph &oriG){


	time_t tmpTime;
	time(&tmpTime);
	MPI_Status status;
	time_t fake = 0;
	if(difftime(tmpTime,updateTime) > cachetime){
		JobType job = ASK_CONSTRAINTS;
		for(int i=0;i<summaryProcId.size();i++){
			//fprintf(stderr, "[q%3d] updateTime %lld\n", qPROCID, updateTime);
			MPI_Send(&job, 1, MPI_INT, summaryProcId[i], 1, MPI_COMM_WORLD);
			MPI_Send(&updateTime, 1, MPI_LONG_LONG, summaryProcId[i], 0, MPI_COMM_WORLD);
		}

		for(int i=0;i<summaryProcId.size();i++){
			int edgePrefix[graphId[i].size()];
			MPI_Recv(edgePrefix, graphId[i].size(), MPI_INT, summaryProcId[i], 2, MPI_COMM_WORLD, &status);
			int edges[edgePrefix[graphId[i].size()-1]*2];
			double values[edgePrefix[graphId[i].size()-1]];
			MPI_Recv(edges, edgePrefix[graphId[i].size()-1]*2, MPI_INT, summaryProcId[i], 2, MPI_COMM_WORLD, &status);
			MPI_Recv(values, edgePrefix[graphId[i].size()-1], MPI_DOUBLE, summaryProcId[i], 2, MPI_COMM_WORLD, &status);
			usleep(30000);
			fprintf(stderr, "[q%3d] constraits edgeNum %d\n", qPROCID, edgePrefix[graphId[i].size()-1]);
			for(int j=0, current=0 ;j<graphId[i].size();j++){
				if(edgePrefix[j] > current){
					for(;current<edgePrefix[j];current++){
						//fprintf(stderr, "%d %d -> %d %d = %lf\n", graphId[i][j], edges[current*2], graphId[i][j], edges[current*2+1], values[current]);
						oriG.updateEdge(ID(graphId[i][j], edges[current*2]), ID(graphId[i][j], edges[current*2+1]), (int)values[current]);
					}
				}
			}
		}

		time(&updateTime);

	}
}

void updateGraph(SimpleGraph &oriG, int edgeNum, int *edges, double *values){
	for(int i=0;i<edgeNum;i++){
		oriG.updateEdge(ID(edges[i*4], edges[i*4+1]), ID(edges[i*4+2], edges[i*4+3]), values[i]);
	}
	delete[] edges;
	delete[] values;
}

void answerQuery(char *t, int tn){

	qPROCID = tn;
	SimpleGraph oriG;

	getOriginalGraph(oriG, t);
	time_t updateTime=0;
	// time(&updateTime);
	delay(updateTime, oriG);
	bool finish = false;
	int ready = 1;
	MPI_Send(&ready, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	fprintf(stderr, "Query processor %d ready.\n", qPROCID-queryProc.start);
	//oriG.print();
	//while(1);
	while(!finish){
		JobType jobType;
		MPI_Status status;
		MPI_Recv(&jobType, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
		fprintf(stderr, "[q%3d] recv job %d\n", qPROCID, jobType);
		int source = status.MPI_SOURCE;
		int edgeNum;
		int *edges;
		double *values;
		double ans;
		int S[2], T[2];
		char s[100];
		vector<int> v;
		int size;
		switch (jobType) {

			case UPDATE_GRAPH:
				MPI_Recv(&edgeNum, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
				edges = new int[edgeNum*4];
				values = new double[edgeNum];
				MPI_Recv(edges, edgeNum*4, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(values, edgeNum, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
				updateGraph(oriG, edgeNum, edges, values);
				getTime(s);
				fprintf(stderr, "[q%3d] update graph %s\n", qPROCID, s);
				break;
			case ANSWER_QUERY:
				MPI_Recv(S, 2, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(T, 2, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
				fprintf(stderr, "[q%3d] answerQuery recv Source: %d %d Target: %d %d\n", qPROCID, S[0], S[1], T[0], T[1]);
				delay(updateTime, oriG);
				v.clear();
				ans = answer_query(oriG, S, T, v);
				getTime(s);
				fprintf(stderr, "[q%3d] send ans (%d, %d) -> (%d, %d) = %lf %s\n", qPROCID, S[0], S[1], T[0], T[1], ans, s);
				MPI_Send(&ans, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD);
				size = v.size();
				MPI_Send(&size, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
				MPI_Send(v.data(), size, MPI_INT, source, 0, MPI_COMM_WORLD);
				break;
			case QUIT:
				fprintf(stderr, "[q%3d] quit!!!!!\n", qPROCID);
				finish=true;
				break;

		}
		//fprintf(stderr, "[%3d] answer query waiting for job...\n", PROCID);
	}
}
