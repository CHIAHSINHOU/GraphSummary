#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<vector>
#include<set>

#define GROUP 15
#define NODE 100000
#define CONNECT_EDGE 10
#define WEIGHT 10000
#define RAND 0

using namespace std;

int G, N, E;

int Rand(int a, int b) {
	return rand()%(b-a+1)+a;
}

inline int Rand() {
	return (rand() << 15) | rand();
}

struct Edge {
	int A, B;
	Edge(int a, int b) {
		A = a;
		B = b;
	}
	bool operator < (const Edge &t) const {
		if ( A != t.A )	return A < t.A;
		return B < t.B;
	}
} ;

int visit[100005];
int group[100005];
int degree[GROUP+1];
vector<int> v[GROUP+1];
set<Edge> edge;

void createDir(int caseNum) {
	char cmd[100];
	sprintf(cmd, "mkdir \"../data/case%d\"", caseNum);
	system(cmd);
	sprintf(cmd, "mkdir \"../data/case%d/graph\"", caseNum);
	system(cmd);
}

void init() {
	for ( int i = 0 ; i <= GROUP ; i++ )
		v[i].clear();
	edge.clear();
	memset(visit,0,sizeof(visit));
	memset(degree,0,sizeof(degree));
}

int main(int args, char * argv[])
{
	if ( args != 5 ) {
		fprintf(stderr, "input args error\n");
		return 0;
	}
	int caseNum;
	int numOfNode;
	int numOfGroup;
	int K;
	sscanf(argv[1], "%d", &caseNum);
	sscanf(argv[2], "%d", &numOfNode);
	sscanf(argv[3], "%d", &numOfGroup);
	sscanf(argv[4], "%d", &K);
	
	srand(time(NULL));
	
	createDir(caseNum);
	init();
	
	char path[100];
	sprintf(path, "../data/case%d/input.in", caseNum);
	FILE * fout = fopen(path, "w");
	N = numOfNode;
	G = min(N, numOfGroup);
	
	// Number of subgraph & number of node
	fprintf(fout, "%d %d\n", N, G);
	for ( int i = 1 ; i <= N ; i++ ) {
		if ( i > G )
			group[i] = Rand(1,G);
		else
			group[i] = i;
		v[group[i]].push_back(i);
		fprintf(fout, "%d ", group[i]);
	}
	fprintf(fout, "\n");
	
	// Edges in subgraph
	
	int W = WEIGHT;
	
	for ( int i = 1 ; i <= G ; i++ ) {
		int size = v[i].size();
		if ( size == 1 )
			continue;
		int E = Rand()%(size*(size/2+1)/4+1)+size;
		if ( !E ) E++;
		while ( E-- ) {
			int A = Rand()%v[i].size();
			int B = A;
			while ( B == A ) {
				B = Rand()%v[i].size();
			}
			if ( edge.find(Edge(v[i][A],v[i][B])) != edge.end() ) {
				continue;
			}
			edge.insert(Edge(v[i][A],v[i][B]));
			fprintf(fout, "%d %d %d\n", v[i][A], v[i][B], rand()%W);
		}
	}
	// Edges which connect subgraphs
	
	for ( int i = 1 ; i < G ; i++ ) {	// The last group can be link by others
		if ( !v[i].size()  ) 
			continue;
		int Rcount = 0;
		while ( degree[i] <= K && degree[i] < v[i].size() && Rcount++ < 1000 ) {
			int A = Rand()%v[i].size();
			int GB = Rand(i+1,G);
			int B = Rand()%v[GB].size();
			if ( degree[GB] >= K || visit[v[i][A]] || visit[v[GB][B]] ) {
				continue;
			}
			visit[v[i][A]] = visit[v[GB][B]] = 1;
			degree[i]++;
			degree[GB]++;
			if ( rand()%2 )
				fprintf(fout, "%d %d %d\n", v[i][A], v[GB][B], rand()%W);
			else
				fprintf(fout, "%d %d %d\n", v[GB][B], v[i][A], rand()%W);
		}
	}
	fprintf(fout, "0 0 0\n");
	
	fclose(fout);
	
	sprintf(path, "../data/case%d/query.in", caseNum);
	fout = fopen(path, "w");
	
	// source and terminal
	int Q;
	if ( caseNum < 600 )
		Q = 1;
	else
		Q = 20;
	while ( Q-- ) {
		int T = rand()%5+1;
		T = 1;
		fprintf(fout, "%d\n", T);
		for ( int i = 0 ; i < T ; i++ ) {
			fprintf(fout, "%d\n", rand()%N+1);
		}
		T = rand()%5+1;
		T = 1;
		fprintf(fout, "\n%d\n", T);
		for ( int i = 0 ; i < T ; i++ ) {
			fprintf(fout, "%d\n", rand()%N+1);
		}
		fprintf(fout, "\n");
	}
	
	fclose(fout);
	
	return 0;
}

