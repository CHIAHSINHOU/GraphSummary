#include"Graph.h"

using namespace std;

Graph::Graph() {
	edgeSize = 0;
}
int Graph::initWithFile(char * s) {
	FILE * fin = fopen(s, "r");
	if ( !fin ) {
		fprintf(stderr, "Fail to open file %s\n", s);
		return 1;
	}
	fscanf(fin, "%d", &groupId);
	int N, M, I, X;
	fscanf(fin, "%d%d%d%d", &N, &M, &I, &X);
	
	groupSize = N;
	edgeSize = 0;
	
	int A, B, GA, GB;
	double W;
	for ( int i = 0 ; i < M ; i++ ) {
		fscanf(fin, "%d%d%lf", &A, &B, &W);
		insertEdge(A,B,W);
		edgeSize++;
	}
	for ( int i = 0 ; i < I ; i++ ) {
		fscanf(fin, "%d%d%d%lf", &GA, &A, &B, &W);
		insertLinkInEdge(LinkInEdge(ID(GA,A),B,W));
	}
	for ( int i = 0 ; i < X ; i++ ) {
		fscanf(fin, "%d%d%d%lf", &A, &GB, &B, &W);
		insertLinkOutEdge(LinkOutEdge(A,ID(GB,B),W));
	}
	fclose(fin);
	time(&updateTime);
}

int Graph::size() {
	return groupSize;
}
int Graph::innerEdgeSize() {
	return edgeSize;
}
void Graph::resize(int A, int B) {
	innerEdge.resize(max(max(A+1,B+1),(int)innerEdge.size()));
}
void Graph::insertEdge(int A, int B, double w) {
	resize(A,B);
	innerEdge[A].push_back(Edge(B,w));
}
void Graph::insertLinkInEdge(LinkInEdge edge) {
	S.insert(edge.B);
	inEdge.push_back(edge);
}
void Graph::insertLinkOutEdge(LinkOutEdge edge) {
	T.insert(edge.A);
	outEdge.push_back(edge);
}
void Graph::updateEdge(int A, int B, double w) {
	for ( int i = 0 ; i < innerEdge[A].size() ; i++ ) {
		if ( innerEdge[A][i].B == B ) {
			innerEdge[A][i].w = w;
			time(&updateTime);
			return;
		}
	}
	insertEdge(A,B,w);
	time(&updateTime);
}
void Graph::removeEdge(int A, int B) {
	// remove all
	for ( int i = 0 ; i < innerEdge[A].size() ; i++ ) {
		if ( innerEdge[A][i].B == B ) {
			innerEdge[A][i] = innerEdge[A].back();
			innerEdge[A].pop_back();
			i--;
			break;
		}
	}
}

void Graph::print() {
	printf("Graph %d ====\n", groupId);
	for ( int i = 0 ; i < innerEdge.size() ; i++ ) {
		printf("%d ->", i);
		for ( int j = 0 ; j < innerEdge[i].size() ; j++ ) {
			printf(" (%d, %lf)", innerEdge[i][j].B, innerEdge[i][j].w);
		}
		puts("");
	}
}

void Graph::fprint(FILE * fout) {
	fprintf(fout, "%d\n", edgeSize);
	for ( int i = 0 ; i < innerEdge.size() ; i++ ) {
		for ( int j = 0 ; j < innerEdge[i].size() ; j++ ) {
			fprintf(fout,"%d -> %d = %lf\n", i, innerEdge[i][j].B, innerEdge[i][j].w);
		}
	}
	puts("");
}


Graph::~Graph() {
	S.clear();
	T.clear();
	innerEdge.clear();
	inEdge.clear();
	outEdge.clear();
}
