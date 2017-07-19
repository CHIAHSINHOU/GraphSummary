#include <stdio.h>
#include <vector>
#include <queue>
#include <stdlib.h>
#include "writeLog.h"
#include "omp.h"
using namespace std;
vector< vector< pair<int, int> > > edge;
const int inf = 1 << 28;
struct Node {int id, dis;};

int querys[10005][2];
bool operator<(const Node &n1, const Node &n2) { return n1.dis > n2.dis; }

int shortestPath(int sourceId, int targetId, int n){
	// printf("shortestPath %d %d %d\n", sourceId, targetId, n);
	int *dis= new int[n+1];
	bool *visit = new bool[n+1];
	for(int i=1;i<=n;i++){
		visit[i] = false;
		dis[i] = inf;
	}
	priority_queue<Node> PQ;

	if(sourceId > n){
		printf("!!error: %d\n", sourceId);
		return inf;
	}

	visit[sourceId] = true;
	dis[sourceId] = 0;
	Node s;
	s.id = sourceId;
	s.dis = 0;
	PQ.push(s);
	for (int k = 0; k < n ; k++){
		int a = -1;
		while(!PQ.empty() && visit[a = PQ.top().id]){
			PQ.pop();
		}
		if (a == -1) break;
		visit[a] = true;
		if(a == targetId)
			return dis[a];

		for ( int i = 0 ; i < edge[a].size() ; i++ ){
			int B = edge[a][i].first;
			int w = edge[a][i].second;
			if(B > n){
				printf("!!error: %d\n", B);
				return inf;
			}
			if( !visit[B] && dis[a] + w < dis[B]){

				dis[B] = dis[a] + w;
				Node node;
				node.id = B;
				node.dis = dis[B];
				PQ.push(node);

			}
		}
		

	}

	while(!PQ.empty())
		PQ.pop();

	return inf;
}
int main(int args, char * argv[]){

	int n, g, a, b, w;

	if ( args != 2 ) {
		fprintf(stderr, "usage: caseNum\n", args);
		return 0;
	}
	Timer t;
	t.startTime();
	int caseNum = atoi(argv[1]);
	char inputFilePath[100];
	sprintf(inputFilePath, "../data/case%d/input.in", caseNum);
	FILE * fin = fopen(inputFilePath, "r");
	fscanf(fin, "%d %d\n", &n, &g);
	for(int i=0;i<n;i++)
		fscanf(fin, "%*d");
	edge.resize(n+1);
	int tmp = 0;
	while(fscanf(fin, "%d%d%d", &a, &b, &w) ==3 && a!=0 && b!=0 && w!=0){
		edge[a].push_back(make_pair(b,w));
		tmp++;
	}
	fprintf(stderr, "node: %10d edge: %10d\n", n, tmp);
	fclose(fin);

	sprintf(inputFilePath, "../data/case%d/query.in", caseNum);
	fin = fopen(inputFilePath, "r");
	int tn;
	fscanf(fin, "%d", &tn);
	int queryId;
	while ( fscanf(fin, "%d%d%d", &queryId, &a, &b) != EOF ) {
		querys[queryId][0] = a;
		querys[queryId][1] = b;
	}
	fclose(fin);
	fprintf(stderr, "prepare: %lf\n", t.endTime());
	char outputFilePath[100];
	sprintf(outputFilePath, "../data/case%d/ans.out", caseNum);
	FILE *fout = fopen(outputFilePath, "w+");
	t.startTime();
	omp_set_num_threads(20);
	#pragma omp parallel for schedule(dynamic)
	for(int i=1;i<=tn;i++){
		int sourceId = querys[i][0], targetId = querys[i][1];
		int ans = shortestPath(sourceId, targetId ,n);
		fprintf(fout, "%d %d\n", i, ans);
		// fprintf(stderr, "ans:%d\n", ans);
	}

	fprintf(stderr," compute: %lf\n", t.endTime());
}
