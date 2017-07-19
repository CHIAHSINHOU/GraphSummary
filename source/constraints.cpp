#include "constraints.h"
#include "Group.h"
#include "Graph.h"
#include "mpi.h"
#include "job_type.h"
#include "summary.h"
#include <queue>
#include <map>
#include <pthread.h>
#include "writeLog.h"
#include "procInfo.h"

#define THEDA 0.2

using namespace std;


pthread_mutex_t mutex, ans_mutex;
pthread_barrier_t barr;
int PROCID;
int groupNum;
int threadNum;
bool *updateGraphBool;
Graph *graphs;
Group *groups;
queue<JobType> job_queue;
queue<Constraint_Data> constraint_data_queue;
queue<Source_Data> source_data_queue;
queue<Target_Data> target_data_queue;
queue<Update_Data> update_data_queue;
queue<Path_Data> path_data_queue;
queue<ST_Data> st_data_queue;
queue<int> ans_type_queue;
queue<int> ans_source_queue;
queue<int*> ans_edgeNumPrefix_queue;
queue<int> ans_edgeNum_queue;
queue<int*> ans_edges_queue;
queue<double*> ans_values_queue;
queue<int*> ans_paths_queue;
queue<int*> ans_pathPrefix_queue;
queue< vector<int> > ans_st_path;
queue<double> ans_st_value;


map<int, int> groupIdIndex;


int genEdges(vector<int> &edges, vector<double> &values, Graph &graph) {

	int edgeNum = 0;
	for ( int i = 0; i < graph.innerEdge.size() ; i++ ) {
		for ( int j = 0 ; j < graph.innerEdge[i].size() ; j++ ) {
			edges.push_back(i);
			edges.push_back(graph.innerEdge[i][j].B);
			values.push_back((double)graph.innerEdge[i][j].w);
			edgeNum++;
		}
	}
	return edgeNum;
}

int genEdges(vector<int> &edges, vector<double> &values, Group &group) {

	int SN = group.S.size();
	int TN = group.T.size();
	int edgeNum = 0;
	for (int i = 0 ; i < SN ; i++ ) {
		for ( int j = 0 ; j < TN ; j++ ) {
			if (group.distance[i][j] != inf) {
				edges.push_back(group.S[i]);
				edges.push_back(group.T[j]);
				values.push_back((double)group.distance[i][j]);
				edgeNum++;
			}
		}
	}
	return edgeNum;
}

void addAnsQueue(int *paths, int *pathPrefix, int *edges, double *values, int edgeNum, int source, int type) {
//******* for source or target
	pthread_mutex_lock(&ans_mutex);
	ans_type_queue.push(type);
	ans_source_queue.push(source);
	ans_edgeNum_queue.push(edgeNum);
	ans_edges_queue.push(edges);
	ans_values_queue.push(values);
	ans_paths_queue.push(paths);
	ans_pathPrefix_queue.push(pathPrefix);

	// for(int i=0;i<edgeNum;i++){
	// 	fprintf(stderr,"(%d %d) ", edges[i*2], edges[i*2+1]);
	// 	for(int j=pathPrefix[i];j<pathPrefix[i+1];j++)
	// 		fprintf(stderr, "->%d", paths[j]);
	// 	fprintf(stderr, "\n");
	// }
	pthread_mutex_unlock(&ans_mutex);

}

void addAnsQueue(int *edges, double *values, int* edgeNumPrefix, int source, int type) {
//******* for summary result
	fprintf(stderr, "!!! add ans queue\n");
	pthread_mutex_lock(&ans_mutex);
	ans_type_queue.push(type);
	ans_source_queue.push(source);
	ans_edgeNumPrefix_queue.push(edgeNumPrefix);
	ans_edges_queue.push(edges);
	ans_values_queue.push(values);
	pthread_mutex_unlock(&ans_mutex);

}

void addAnsQueue(int *from, int size, int *perfixPath, int *query, int source, int type) {
//****** for path
	pthread_mutex_lock(&ans_mutex);
	ans_type_queue.push(type);
	ans_source_queue.push(source);
	ans_paths_queue.push(from);
	ans_pathPrefix_queue.push(perfixPath);
	ans_edgeNum_queue.push(size);
	ans_edges_queue.push(query);
	pthread_mutex_unlock(&ans_mutex);

}
void addAnsQueue(vector<int> &paths, double w, int source, int type){
//****** for source and target result
	pthread_mutex_lock(&ans_mutex);
	ans_type_queue.push(type);
	ans_source_queue.push(source);
	ans_st_value.push(w);
	ans_st_path.push(paths);
	pthread_mutex_unlock(&ans_mutex);

}

void ask_constraints(Constraint_Data &data) {


	fprintf(stderr, "ask constraints\n");
	MPI_Status status;
	int ask;
	vector<int> edges;
	vector<double> values;
	int edgeNum;
	int *edgePrefix = new int[groupNum];
	for (int i = 0, current = 0; i < groupNum; i++) {
		int tmpEdgeNum = 0;
		if ((double)graphs[i].edgeSize / groups[i].shortcutNum < THEDA) {
			if (difftime(graphs[i].updateTime, (time_t)data.updateTime) > 0.0)
				tmpEdgeNum = genEdges(edges, values, graphs[i]);
		}
		else {
			if (difftime(groups[i].updateTime, (time_t)data.updateTime) > 0.0)
				tmpEdgeNum = genEdges(edges, values, groups[i]);
		}

		edgePrefix[i] = current + tmpEdgeNum;
		current += tmpEdgeNum;

	}

	int *edgesArray = new int[edges.size()];
	double *valuesArray = new double[values.size()];
	//fprintf(stderr, "%d %d\n", edges.size(), values.size());
	for (int i = 0; i < edges.size(); i++)
		edgesArray[i] = edges[i];
	for (int i = 0; i < values.size(); i++)
		valuesArray[i] = values[i];

	addAnsQueue(edgesArray, valuesArray, edgePrefix, data.source, 2);


}

void ask_constraints_source(Source_Data &data) {

	Graph &graph = graphs[data.groupId];
	fprintf(stderr, "source group id %d \n", graph.groupId);
	int edgeNum;
	int *edges = new int[graph.T.size() * 2];
	double *values = new double[graph.T.size()];
	int *pathPrefix = new int[graph.T.size() + 1];
	vector<int> paths;
	calculateSummarySource(paths, pathPrefix, edges, values, edgeNum, graph, data.S[1]);
	int *path_tmp = new int[paths.size()];
	for(int i=0;i<paths.size();i++)
		path_tmp[i] = paths[i];
	addAnsQueue(path_tmp, pathPrefix, edges, values, edgeNum, data.source, 3);
	fprintf(stderr, "finish\n" );

}

void ask_constraints_target(Target_Data &data) {

	Graph &graph = graphs[data.groupId];
	fprintf(stderr, "target group id %d \n", graphs[data.groupId].groupId);
	int edgeNum;
	int *edges = new int[graph.S.size() * 2];
	double *values = new double[graph.S.size()];
	int *pathPrefix = new int[graph.S.size() + 1];
	vector<int> paths;
	calculateSummaryTarget(paths, pathPrefix, edges, values, edgeNum, graph, data.T[1]);
	int *path_tmp = new int[paths.size()];
	for(int i=0;i<paths.size();i++)
		path_tmp[i] = paths[i];
	addAnsQueue(path_tmp, pathPrefix, edges, values, edgeNum, data.source, 4);
	fprintf(stderr, "finish\n" );

}

void ask_constraints_st(ST_Data &data){

	Graph &graph = graphs[data.groupId];
	vector<int> paths;
	double w;
	w = calculateSummaryST(paths, graph, data.S, data.T);
	fprintf(stderr, "ask_constraints_st ");
	for(int i=0;i<paths.size();i++)
		fprintf(stderr, "%d->", paths[i]);
	fprintf(stderr, "\n");
	addAnsQueue(paths, w, data.source, 6);

}

void ask_path(Path_Data &data) {

	std::vector<int> v;
	int *perfixPath = new int[data.size/3+1];
	perfixPath[0] = 0;
	for(int i=0;i<data.size;i+=3){
		Group &group = groups[groupIdIndex[data.path[i]]];
		fprintf(stderr, "group id %d %d %d\n", group.groupId, data.path[i+1], data.path[i+2]);
		int sIndex, tIndex;
		for(int j=0;j<group.S.size();j++){
			if(group.S[j] == data.path[i+1]){
				sIndex = j;
			}
		}
		for(int j=0;j<group.T.size();j++){
			if(group.T[j] == data.path[i+2]){
				tIndex = j;
			}
		}
		perfixPath[i/3+1] = perfixPath[i/3] + group.path[sIndex][tIndex].size()-1;
		for(int j=1;j<group.path[sIndex][tIndex].size();j++)
			v.push_back(group.path[sIndex][tIndex][j]);
		
	}
	int *path_tmp = new int[v.size()];
	for(int i=0;i<v.size();i++)
		path_tmp[i] = v[i];
	addAnsQueue(path_tmp, data.size, perfixPath, data.path, data.source, 5);
}

void updateGraph(int id) {

	int edgeNum;
	int *edgesPtr;
	double *valuesPtr;
	int local_groupId;
	int work = (groupNum+threadNum-1) / threadNum;
	//fprintf(stderr, "work = %d\n", work);
	for (int i = id * work; i < work * (id + 1) && i < groupNum; i++)
		updateGraphBool[i] = false;
	//fprintf(stderr, "%d finish ini updateGraphBool\n", id);
	pthread_barrier_wait(&barr);
	if (id == 0) {
		edgeNum = update_data_queue.front().edgeNum;
		edgesPtr = update_data_queue.front().edges;
		valuesPtr = update_data_queue.front().values;
		for (int i = 0; i < edgeNum; i++) {
			local_groupId = groupIdIndex[edgesPtr[i * 4]];
			fprintf(stderr, "graph %d update\n", local_groupId);
			updateGraphBool[local_groupId] = true;
			graphs[local_groupId].updateEdge(edgesPtr[i * 4 + 1], edgesPtr[i * 4 + 3], valuesPtr[i]);
		}
	}
	//fprintf(stderr, "%d finish update Edges\n", id);
	pthread_barrier_wait(&barr);
	for (int i = id * work; i < work * (id + 1) && i < groupNum; i++) {
		if (updateGraphBool[i]) {
			calculateSummary(graphs[i], groups[i]);
		}
	}
	//fprintf(stderr, "%d finish calculateSummary\n", id);
	if (id == 0) {
		pthread_mutex_lock(&mutex);
		job_queue.pop();
		update_data_queue.pop();
		pthread_mutex_unlock(&mutex);
		delete[] edgesPtr;
		delete[] valuesPtr;
	}
	//fprintf(stderr, "%d finish pop job queue\n", id);
	pthread_barrier_wait(&barr);

}

void *run(void *_id) {

	int id = *((int*)_id);
	int finish = 0;
	Constraint_Data data1;
	Source_Data data2;
	Target_Data data3;
	Path_Data data4;
	ST_Data data5;
	while (!finish) {
		pthread_mutex_lock(&mutex);
		if (job_queue.empty())
			pthread_mutex_unlock(&mutex);
		else {
			switch (job_queue.front()) {
			case UPDATE_GRAPH :
				//fprintf(stderr, "%d update Graph\n", id);
				pthread_mutex_unlock(&mutex);
				updateGraph(id);
				//fprintf(stderr, "%d finish update Graph\n", id);
				break;
			case ASK_CONSTRAINTS :
				data1 = constraint_data_queue.front();
				constraint_data_queue.pop();
				job_queue.pop();
				pthread_mutex_unlock(&mutex);
				ask_constraints(data1);
				break;
			case ASK_CONSTRAINTS_WITH_SOURCE :
				data2 = source_data_queue.front();
				source_data_queue.pop();
				job_queue.pop();
				pthread_mutex_unlock(&mutex);
				ask_constraints_source(data2);
				break;
			case ASK_CONSTRAINTS_WITH_TARGET :
				data3 = target_data_queue.front();
				target_data_queue.pop();
				job_queue.pop();
				pthread_mutex_unlock(&mutex);
				ask_constraints_target(data3);
				break;
			case ASK_PATH :
				data4 = path_data_queue.front();
				path_data_queue.pop();
				job_queue.pop();
				pthread_mutex_unlock(&mutex);
				ask_path(data4);
				break;
			case ASK_CONSTRAINTS_WITH_S_T :
				data5 = st_data_queue.front();
				st_data_queue.pop();
				job_queue.pop();
				pthread_mutex_unlock(&mutex);
				ask_constraints_st(data5);
				break;
			case QUIT:
				pthread_mutex_unlock(&mutex);
				finish = 1;
				break;
			}
		}

	}

}

bool addJobQueue(JobType &job, int source) {

	MPI_Status status;
	Constraint_Data data1;
	Source_Data data2;
	Target_Data data3;
	Update_Data data;
	Path_Data data4;
	ST_Data data5;
	int S[2];
	int T[2];
	int *path;
	int size;
	long long updateTime;
	int edgeNum;
	int* edges;
	double* values;
	char s[100];
	int ST[3];
	switch (job) {

	case UPDATE_GRAPH :
		MPI_Recv(&edgeNum, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
		//fprintf(stderr, "[u%3d] update recv %d %s\n", PROCID, edgeNum, s);
		edges = new int[edgeNum * 4];
		values = new double[edgeNum];
		MPI_Recv(edges, edgeNum * 4, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(values, edgeNum, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
		getTime(s);
		for (int i = 0; i < edgeNum; i++) {
			fprintf(stderr, "[u%3d] receive update edge (%d, %d) -> (%d, %d) = %lf %s\n", PROCID, edges[i * 4], edges[i * 4 + 1], edges[i * 4 + 2], edges[i * 4 + 3], values[i], s);
		}
		data = Update_Data(edgeNum, edges, values);
		pthread_mutex_lock(&mutex);
		job_queue.push(job);
		update_data_queue.push(data);
		pthread_mutex_unlock(&mutex);
		fprintf(stderr, "receive update edge finish\n");
		break;

	case ASK_CONSTRAINTS :
		MPI_Recv(&updateTime, 1, MPI_LONG_LONG, source, 0, MPI_COMM_WORLD, &status);
		getTime(s);
		fprintf(stderr, "[u%3d] constraints recv %lld %s\n", PROCID, updateTime, s);
		data1 = Constraint_Data(updateTime, source);
		pthread_mutex_lock(&mutex);
		job_queue.push(job);
		constraint_data_queue.push(data1);
		pthread_mutex_unlock(&mutex);
		break;

	case ASK_CONSTRAINTS_WITH_SOURCE :
		MPI_Recv(S, 2, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
		getTime(s);
		fprintf(stderr, "[u%3d] recv S(%d, %d) %s\n", PROCID, S[0], S[1], s);
		data2 = Source_Data(S, groupIdIndex[S[0]], source);
		pthread_mutex_lock(&mutex);
		job_queue.push(job);
		source_data_queue.push(data2);
		pthread_mutex_unlock(&mutex);
		break;

	case ASK_CONSTRAINTS_WITH_TARGET :
		MPI_Recv(T, 2, MPI_INT, source, 3, MPI_COMM_WORLD, &status);
		getTime(s);
		fprintf(stderr, "[u%3d] recv T(%d, %d) %s\n", PROCID, T[0], T[1], s);
		data3 = Target_Data(T, groupIdIndex[T[0]], source);
		pthread_mutex_lock(&mutex);
		job_queue.push(job);
		target_data_queue.push(data3);
		pthread_mutex_unlock(&mutex);
		break;

	case ASK_PATH :
		MPI_Recv(&size, 1, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
		path = new int[size];
		MPI_Recv(path, size, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
		fprintf(stderr, "[u%3d] recv ask_path size:%d\n", PROCID, size);
		data4 = Path_Data(path, source, size);
		pthread_mutex_lock(&mutex);
		job_queue.push(job);
		path_data_queue.push(data4);
		pthread_mutex_unlock(&mutex);
		break;

	case ASK_CONSTRAINTS_WITH_S_T :
		MPI_Recv(ST, 3, MPI_INT, source, 5, MPI_COMM_WORLD, &status);
		data5 = ST_Data(ST, groupIdIndex[ST[0]], source);
		pthread_mutex_lock(&mutex);
		job_queue.push(job);
		st_data_queue.push(data5);
		pthread_mutex_unlock(&mutex);
		break;

	case QUIT :
		pthread_mutex_lock(&mutex);
		job_queue.push(job);
		pthread_mutex_unlock(&mutex);
		return false;

	}
	return true;
}

void sendAns() {

	int type, source, edgeNum;
	int *edgesPtr, *edgeNumPrefixPtr;
	int *pathPtr, *pathPrefixPtr;

	double *valuesPtr;

	pthread_mutex_lock(&ans_mutex);
	while (!ans_source_queue.empty()) {
		type = ans_type_queue.front();
		source = ans_source_queue.front();
		ans_type_queue.pop();
		ans_source_queue.pop();

		if(type == 2){
			//*** for summary result
			edgeNumPrefixPtr = ans_edgeNumPrefix_queue.front();
			edgesPtr = ans_edges_queue.front();
			valuesPtr = ans_values_queue.front();
			ans_edgeNumPrefix_queue.pop();
			ans_edges_queue.pop();
			ans_values_queue.pop();
		}
		else if(type == 3 || type == 4){
			//** for source or target
			pathPtr = ans_paths_queue.front();
			pathPrefixPtr = ans_pathPrefix_queue.front();
			edgeNum = ans_edgeNum_queue.front();
			edgesPtr = ans_edges_queue.front();
			valuesPtr = ans_values_queue.front();
			ans_edges_queue.pop();
			ans_values_queue.pop();
			ans_edgeNum_queue.pop();
			ans_pathPrefix_queue.pop();
			ans_paths_queue.pop();
		}
		else if(type == 5){
			pathPtr = ans_paths_queue.front();
			pathPrefixPtr = ans_pathPrefix_queue.front();
			edgeNum = ans_edgeNum_queue.front();
			edgesPtr = ans_edges_queue.front();
			ans_edges_queue.pop();
			ans_edgeNum_queue.pop();
			ans_pathPrefix_queue.pop();
			ans_paths_queue.pop();

		}

		else if(type == 6){
			double w = ans_st_value.front();
			ans_st_value.pop();
			vector<int> tmp = ans_st_path.front();
			ans_st_path.pop();
			MPI_Send(&w, 1, MPI_DOUBLE, source, type, MPI_COMM_WORLD);
			if(w != inf){
				int size = tmp.size()-1;
				int tmp1[size];
				for(int i=1;i<tmp.size();i++)
					tmp1[i-1] = tmp[i];
				MPI_Send(&size, 1, MPI_INT, source, type, MPI_COMM_WORLD);
				MPI_Send(tmp1, size, MPI_INT, source, type, MPI_COMM_WORLD);
			}
			
		}


		if(type == 2){
			fprintf(stderr, "groupNum: %d\n%d\n", groupNum, edgeNumPrefixPtr[groupNum - 1]);
			MPI_Send(edgeNumPrefixPtr, groupNum, MPI_INT, source, type, MPI_COMM_WORLD);
			MPI_Send(edgesPtr, edgeNumPrefixPtr[groupNum - 1] * 2, MPI_INT, source, type, MPI_COMM_WORLD);
			MPI_Send(valuesPtr, edgeNumPrefixPtr[groupNum - 1], MPI_DOUBLE, source, type, MPI_COMM_WORLD);
			// free(edgeNumPrefixPtr);
			// free(edgesPtr);
			// free(valuesPtr);
		}
		else if(type == 3 || type == 4){
			MPI_Send(&edgeNum, 1, MPI_INT, source, type, MPI_COMM_WORLD);
			fprintf(stderr, "send edgeNum\n");
			MPI_Send(edgesPtr, edgeNum * 2, MPI_INT, source, type, MPI_COMM_WORLD);
			fprintf(stderr, "send edges\n");
			MPI_Send(valuesPtr, edgeNum, MPI_DOUBLE, source, type, MPI_COMM_WORLD);
			fprintf(stderr, "send values\n");
			MPI_Send(pathPrefixPtr, edgeNum + 1, MPI_INT, source, type, MPI_COMM_WORLD);
			fprintf(stderr, "send pathPrefix\n");
			MPI_Send(pathPtr, pathPrefixPtr[edgeNum], MPI_INT, source, type, MPI_COMM_WORLD);
			fprintf(stderr, "send paths\n");
			// free(edgesPtr);
			// free(valuesPtr);
			// free(pathPrefixPtr);
			// free(pathPtr);
		}
		else if(type == 5){
			MPI_Send(edgesPtr, edgeNum, MPI_INT, source, type, MPI_COMM_WORLD);
			MPI_Send(pathPrefixPtr, edgeNum/3 + 1, MPI_INT, source, type, MPI_COMM_WORLD);
			MPI_Send(pathPtr, pathPrefixPtr[edgeNum/3], MPI_INT, source, type, MPI_COMM_WORLD);
			// free(edgesPtr);
			// free(pathPrefixPtr);
			// free(pathPtr);

		}
	}
	pthread_mutex_unlock(&ans_mutex);
}


void calculateConstraints(char *t, int* groupId, int _groupNum, int tn) {

	groupNum = _groupNum;
	graphs = new Graph[groupNum + 1];
	groups = new Group[groupNum + 1];
	updateGraphBool = new bool[groupNum + 1];
	PROCID = tn;


	threadNum = 8;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&ans_mutex, NULL);
	pthread_barrier_init(&barr, NULL, (unsigned) threadNum);
	pthread_t* threads = (pthread_t*)malloc(threadNum * sizeof(pthread_t));

	for (int i = 0; i < groupNum; i++) {
		char buf[100];
		sprintf(buf, "%s/graph/graph%d.out", t, groupId[i]);
		graphs[i].initWithFile(buf);
		//sprintf(buf, "%s/graph/group%d.out", t, groupId[i]);
		groups[i].initWithGraph(graphs[i]);
		groupIdIndex[groupId[i]] = i;
		calculateSummary(graphs[i], groups[i]);
	}
	int ids[threadNum];

	for (int i = 0; i < threadNum; i++) {
		ids[i] = i;
		pthread_create(&threads[i], NULL, run, (void *)&ids[i]);
	}
	MPI_Status status;
	MPI_Request request;
	bool finish = false;
	int ready = 1;
	MPI_Send(&ready, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	fprintf(stderr, "Update processor %d ready.\n", PROCID - updateProc.start);
	JobType job;

	MPI_Irecv(&job, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &request);
	while (!finish) {
		int flag;
		//fprintf(stderr, "summary waiting for job...\n");
		MPI_Test(&request, &flag, &status);
		if (flag != 0) {
			//fprintf(stderr, "[u%3d] recv job %d\n", PROCID, job);
			if (addJobQueue(job, status.MPI_SOURCE)) {
				MPI_Irecv(&job, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &request);
			}
			else
				finish = true;

		}
		sendAns();
	}

	for (int i = 0; i < threadNum; i++)
		pthread_join(threads[i], NULL);

	sendAns();
	return;
}
