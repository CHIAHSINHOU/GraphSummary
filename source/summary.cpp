#include "summary.h"
#include "Graph.h"
#include "Group.h"
#include "sssp.h"
#include <time.h>


double calculateSummaryST(vector<int> &path, Graph &graph, int source, int target){
	path.clear();
	return sssp(graph ,source, target, path);
}
void calculateSummary(Graph &graph, Group &group){

	bool changed = false;
	int *from = NULL;
	fprintf(stderr, "groupid: %d\n", group.groupId);
	for(int i=0;i<group.S.size();i++){

		double dis[graph.groupSize];
		from = sssp(dis, graph, group.S[i]);
		//graph.print();
		for(int j=0;j<group.T.size();j++){
			fprintf(stderr, "calculateSummary %d %d %lf\n", group.S[i], group.T[j], dis[group.T[j]]);
			if(group.distance[i][j] != dis[group.T[j]])
				changed = true;
			group.distance[i][j] = dis[group.T[j]];
			group.path[i][j].clear();
			addPath(group.path[i][j], group.T[j], from);
			// for(int k=0;k<group.path[i][j].size();k++)
			// 	fprintf(stderr, "%d->", group.path[i][j][k]);
			//fprintf(stderr, "\n");
		}
	}
	if(from!=NULL)
		free(from);
	if(changed){
		fprintf(stderr, "group %d changed\n", group.groupId);
		time(&group.updateTime);
	}
}

void calculateSummarySource(vector<int> &paths, int *pathPrefix, int *edges, double *values, int &edgeNum, Graph &graph, int source){

	double dis[graph.groupSize];
	int *from;
	from = sssp(dis, graph, source);


	edgeNum = 0;
	int count = 1;
	pathPrefix[0] = 0;
	for(std::set<int>::iterator it=graph.T.begin(); it!=graph.T.end(); ++it){
		vector<int> path;
		edges[edgeNum*2] = source;
		edges[edgeNum*2+1] = *it;
		values[edgeNum++] = dis[*it];
		addPath(path, *it, from);
		//fprintf(stderr, "(%d->%d): ",source, *it);
		for(int i=1;i<path.size();i++){
			//fprintf(stderr, "%d->", path[i]);
			paths.push_back(path[i]);
		}
		//fprintf(stderr, "\n");
		pathPrefix[count++] = paths.size();

	}

	free(from);
}

void calculateSummaryTarget(vector<int> &paths, int *pathPrefix, int *edges, double *values, int &edgeNum, Graph &graph, int target){


	edgeNum = 0;
	int count = 1;
	pathPrefix[0] = 0;
	//graph.print();
	for(std::set<int>::iterator it=graph.S.begin(); it!=graph.S.end(); ++it){
		vector<int> path;
		edges[edgeNum*2] = *it;
		edges[edgeNum*2+1] = target;
		values[edgeNum++] = sssp(graph, *it, target, path);
		//fprintf(stderr, "(%d %d) size %d\n", *it, target, path.size());
		for(int i=1;i<path.size();i++)
			paths.push_back(path[i]);
		pathPrefix[count++] = paths.size();
	}
	//fprintf(stderr, "pathPrefixSize:%d TSize:%d\n", count, graph.T.size());
}