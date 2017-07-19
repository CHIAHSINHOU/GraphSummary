#include "funcs.h"
int indexOf(int v, vector<int> &V) {
	for ( int i = 0 ; i < V.size() ; i++ )
		if ( V[i] == v )
			return i;
	return -1;
}

int indexOf(int v, vector<Edge> &V) {
	for ( int i = 0 ; i < V.size() ; i++ )
		if ( V[i].B == v )
			return i;
	return -1;
}

void fprintIntVector(FILE * fout, vector<int> &V) {
	fprintf(fout, "%d\n", V.size());
	for ( int i = 0 ; i < V.size() ; i++ ) {
		fprintf(fout, "%d ", V[i]);
	}
	fprintf(fout, "\n\n");
}
void fgetIntVector(FILE * fin, vector<int> &V) {
	int N, A;
	fscanf(fin, "%d", &N);
	for ( int i = 0 ; i < N ; i++ ) {
		fscanf(fin, "%d", &A);
		V.push_back(A);
	}
}
void fprintEdgeVector(FILE * fout, vector<LinkInEdge> &V) {
	fprintf(fout, "%d\n", V.size());
	for ( int i = 0 ; i < V.size() ; i++ ) {
		V[i].fprint(fout);
	}
	fprintf(fout, "\n");
}
void fprintEdgeVector(FILE * fout, vector<LinkOutEdge> &V) {
	fprintf(fout, "%d\n", V.size());
	for ( int i = 0 ; i < V.size() ; i++ ) {
		V[i].fprint(fout);
	}
	fprintf(fout, "\n");
}
void fgetEdgeVector(FILE * fin, vector<LinkInEdge> &V) {
	int N, GA, A, B;
	double c;
	fscanf(fin, "%d", &N);
	for ( int i = 0 ; i < N ; i++ ) {
		fscanf(fin, "%d%d%d%lf", &GA, &A, &B, &c);
		V.push_back(LinkInEdge(ID(GA,A), B, c));
	}
}
void fgetEdgeVector(FILE * fin, vector<LinkOutEdge> &V) {
	int N, A, GB, B;
	double c;
	fscanf(fin, "%d", &N);
	for ( int i = 0 ; i < N ; i++ ) {
		fscanf(fin, "%d%d%d%lf", &A, &GB, &B, &c);
		V.push_back(LinkOutEdge(A, ID(GB,B), c));
	}
}

void setToVector(set<int> &S, vector<int> &V) {
	for ( set<int>::iterator it = S.begin() ; it != S.end() ; it++ ){
		V.push_back(*it);
		printf("%d\n", *it);
	}
}

void stop() {
	while(1);
}

// void pause() {
// 	system("pause");
// }
