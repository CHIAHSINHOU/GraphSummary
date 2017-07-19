#include "Group.h"
#include "time.h"

Group::Group() {}
Group::Group(Graph &G) {
	type = 0;
	initWithGraph(G);
}


// Group::Group( const Group &obj) {
// 	groupId = obj.groupId;
// 	k = obj.k;
	

// }
inline int Group::size() {
	return newId.size();
}
int Group::relabel(ID A) {
	return newId.relabel(A);
}
ID Group::recover(int a) {
	return newId.recover(a);
}
int Group::initWithFile(char * _path) {
	FILE * fin = fopen(_path, "r");
	if ( !fin ) {
		fprintf(stderr, "Fail to open file %s\n", _path);
		return 1;
	}
	int N;
	fscanf(fin, "%d\n", &groupId);
	fgetIntVector(fin, S);
	fgetIntVector(fin, T);
	K = S.size()+T.size();
	fgetEdgeVector(fin, inEdge);
	fgetEdgeVector(fin, outEdge);
	int SN = S.size();
	int TN = T.size();
	distance = (double**) malloc(SN*sizeof(double*));
	for (int i=0 ; i < SN; i++){
		distance[i] = (double*) malloc(TN*sizeof(double));
		fill(distance[i], distance[i]+TN, inf);
	}
	shortcutNum = 0;
	fscanf(fin, "%d", &N);
	path.resize(SN);
	for(int i=0; i<SN ;i++)
		path[i].resize(TN);
	for ( int i = 0 ; i < N ; i++ ) {
		int A, B ,x, y;
		double W;
		fscanf(fin, "%d -> %d = %lf\n", &A, &B, &W);
		if(W != inf){
			for (int j = 0; j < SN; j++)
				if(S[j] == A)
					x = j;
			for (int j = 0; j < TN; j++)
				if(T[j] == B)
					y = j;
			distance[x][y] = W;
			shortcutNum++;
		}
		// printf("%d -> %d = %d\n", A, B, W);
	}
	fclose(fin);
	time(&updateTime);
	return 0;
}
void Group::initWithGraph(Graph &G) {
	groupId = G.groupId;
	setToVector(G.S, S);
	setToVector(G.T, T);
	inEdge = G.inEdge;
	outEdge = G.outEdge;
	K = S.size()+T.size();
	int SN = S.size();
	int TN = T.size();
	distance = (double**) malloc(SN*sizeof(double*));
	for (int i=0 ; i < SN; i++)
		distance[i] = (double*) malloc(TN*sizeof(double));
	path.resize(SN);
	for(int i=0; i<SN ;i++)
		path[i].resize(TN);
	
	printf("%d %d\n", groupId, K);
}
void Group::fprint(int caseNum, Graph &graph) {
	char input[100];
	sprintf(input, "../data/case%d/graph/group%d.out", caseNum, groupId);
	FILE * fout = fopen(input, "w+");
	type = 1;
	fprint(fout, graph);
	fclose(fout);
}

void Group::fprint(FILE * fout, Graph &graph) {
	fprintf(fout, "%d\n", groupId);
	fprintf(fout, "%d\n", graph.size());
	for(int i=1;i<=graph.size();i++){
		fprintf(fout, "%d ", i);
	}
	fprintf(fout, "\n");
	fprintf(fout, "%d\n", graph.size());
	for(int i=1;i<=graph.size();i++){
		fprintf(fout, "%d ", i);
	}
	fprintf(fout, "\n");
	fprintEdgeVector(fout, inEdge);
	fprintEdgeVector(fout, outEdge);
	
	graph.fprint(fout);
}

void Group::fprint(int caseNum) {
	char input[100];
	sprintf(input, "../data/case%d/graph/group%d.out", caseNum, groupId);
	FILE * fout = fopen(input, "w+");
	fprint(fout);
	fclose(fout);
}
void Group::fprint(FILE * fout) {
	fprintf(fout, "%d\n", groupId);
	fprintIntVector(fout, S);
	fprintIntVector(fout, T);
	fprintEdgeVector(fout, inEdge);
	fprintEdgeVector(fout, outEdge);
	int SN = S.size();
	int TN = T.size();
	
	fprintf(fout, "%d\n", SN*TN);
	for (int i = 0 ; i < SN ; i++ ) {
		for (int j = 0 ; j < TN ; j++){
			fprintf(fout, "%d -> %d = %lf\n", S[i], T[j], distance[i][j]);
		}
	}
}



Group::~Group() {
	newId.clear();
	int SN = S.size();
	// if(distance){
	// 	for(int i = 0 ; i < SN ; i++){
	// 		free(distance[i]);
	// 	}
	// 	free(distance);
	// }
	S.clear();
	T.clear();
	inEdge.clear();
	outEdge.clear();
}
