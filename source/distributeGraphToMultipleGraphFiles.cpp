#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<vector>
#include<map>
#define CASE_NUM 10000
using namespace std;


/*
	Given an initial graph file, this program participates 
	the graph into multiple subgraphs by group id.
	
	input file format
	First line : Two integers, number of nodes N and number of groups G.
	Second line : N integers denote the group id of each node.
		(numbered from 1 to G)
	Several lines : Each line contains 3 integers A B C, indicating
		there is a directed edge from A to B, with capacity C. The
		input ends with 3 zeros.
		
	output file format - subgraph
	First line : Number of graph id
	Second line : Four integers, number of nodes N, number of inner 
		edges E, number of linked-in edges I, and number of linked-out
		edges X.
	E lines follow : Three integers A, B, C, indicating	there is a 
		directed edge from A to B, with capacity C.
	I lines follow : Four integers GA, A, B and C indicating there 
		is a directed edge from A in group GA to B, with capacity C.
	X lines follow : Four integers A, GB, B and C indicating there 
		is a directed edge from A to B in group GB, with capacity C.
	
*/

struct Relabel {
	int count;
	map<int, int> newId;
	vector<int> originId;
	Relabel() {
		count = 1;
		originId.push_back(0);
	}
	int relabel(int a) {
		if ( newId.find(a) == newId.end() ) {
			originId.push_back(a);
			newId[a] = count++;
		}
		return newId[a];
	}
	int recover(int A) {
		return originId[A];
	}
	int size() {
		return newId.size();
	}
} ;

struct Edge {
	int A, B, C;
	Edge() {}
	Edge(int _a, int _b, int _c) {
		A = _a;
		B = _b;
		C = _c;
	}
	void fprint(FILE * fout) {
		fprintf(fout, "%d %d %d\n", A, B, C);
	}
} ;

struct LinkInEdge {
	int A, B, C;
	int groupOfA;
	LinkInEdge() {}
	LinkInEdge(int _a, int _b, int _c, int d) {
		groupOfA = _a;
		A = _b;
		B = _c;
		C = d;
	}
	void fprint(FILE * fout) {
		fprintf(fout, "%d %d %d %d\n", groupOfA, A, B, C);
	}
} ;

struct LinkOutEdge {
	int A, B, C;
	int groupOfB;
	LinkOutEdge() {}
	LinkOutEdge(int _a, int _b, int _c, int d) {
		A = _a;
		groupOfB = _b;
		B = _c;
		C = d;
	}
	void fprint(FILE * fout) {
		fprintf(fout, "%d %d %d %d\n", A, groupOfB, B, C);
	}
} ;

struct Graph {
	int id;
	Relabel R;
	vector<Edge> E;
	vector<LinkInEdge> inE;
	vector<LinkOutEdge> outE;
	void setId(int _id) {
		id = _id;
	}
	int relabel(int a) {
		return R.relabel(a);
	}
	int recover(int A) {
		return R.recover(A);
	}
	int size() {
		return R.size();
	}
	void insertEdge(int A, int B, int C) {
		relabel(A);
		E.push_back(Edge(relabel(A), relabel(B), C));
	}
	void insertLinkInEdge(int A, int B, int C, Graph & groupOfA) {
		inE.push_back(LinkInEdge(groupOfA.id, groupOfA.relabel(A), relabel(B), C));
	}
	void insertLinkOutEdge(int A, int B, int C, Graph & groupOfB) {
		outE.push_back(LinkOutEdge(relabel(A), groupOfB.id, groupOfB.relabel(B), C));
	}
	void fprint(FILE * fout) {
		fprintf(fout, "%d\n", id);
		fprintf(fout, "%d %d %d %d\n\n", size(), E.size(), inE.size(), outE.size());
		for ( int i = 0 ; i < E.size() ; i++ ) {
			E[i].fprint(fout);
		}
		fprintf(fout, "\n");
		for ( int i = 0 ; i < inE.size() ; i++ ) {
			inE[i].fprint(fout);
		}
		fprintf(fout, "\n");
		for ( int i = 0 ; i < outE.size() ; i++ ) {
			outE[i].fprint(fout);
		}
		fprintf(fout, "\n");
	}
} ;

int main(int args, char * argv[])
{
	int A, B, C;
	char inputFilePath[100];
	char outputFileDir[100];
	char outputFilePath[100];
	
	char caseNum[100];
	

	if ( args != 2 ) {
		fprintf(stderr, "usage: caseNum\n", args);
		return 0;
	}
	sscanf(argv[1], "%s", caseNum);
	
	sprintf(inputFilePath, "%s/input.in", argv[1]);
	sprintf(outputFileDir, "%s/graph", argv[1]);
	int len = strlen(outputFileDir);
	if ( outputFileDir[len-1] == '\\' || outputFileDir[len-1] == '/' )
		outputFileDir[len-1] = '\0';
	char buf[110];

	// open input file
	FILE * fin = fopen(inputFilePath, "r");
	if ( !fin ) {
		fprintf(stderr, "input file doesn't exist\n");
		return 0;
	}
	
	int N, G;
	fscanf(fin, "%d%d", &N, &G);
	printf("%d %d\n", N, G);
	Graph * graph = new Graph[G+1]();
	int * group = new int[N+1];
	for ( int i = 1 ; i <= G ; i++ )
		graph[i].setId(i);
	for ( int i = 1 ; i <= N ; i++ ) {
		fscanf(fin, "%d", &group[i]);
	}
	while ( fscanf(fin, "%d%d%d", &A, &B, &C) && A ) {
		if ( group[A] == group[B] ) {
			graph[ group[A] ].insertEdge(A, B, C);
		}
		else {
			graph[ group[A] ].insertLinkOutEdge(A, B, C, graph[group[B]]);
			graph[ group[B] ].insertLinkInEdge(A, B, C, graph[group[A]]);
		}
	}
	fclose(fin);
	
	

	// sprintf(inputFilePath, "../data/case%d/query.in", caseNum);
	// fin = fopen(inputFilePath, "r");
	// FILE * fout, *dijFout;
	// sprintf(outputFilePath, "%s/query.in", outputFileDir);
	// fout = fopen(outputFilePath, "w+");
	// sprintf(outputFilePath, "%s/query.in", caseNum);
	// dijFout = fopen(outputFilePath, "w+");
	// fprintf(fout, "%d\n", CASE_NUM);
	// fprintf(dijFout, "%d\n", CASE_NUM);
	// for(int i=1;i<=CASE_NUM;i++){
	
	// 	int a_groupId, a_nodeId, b_groupId, b_nodeId, a , b;
	// 	a = rand()%N + 1;
	// 	b = rand()%N + 1;
	// 	a_groupId = group[a];
	// 	b_groupId = group[b];
	// 	a_nodeId = graph[ group[a] ].relabel(a);
	// 	b_nodeId = graph[ group[b] ].relabel(b);
	// 	if(a_nodeId > graph[ group[a] ].size() || b_nodeId > graph[ group[b] ].size()){
	// 		printf("group[a]: %d\n", group[a]);
	// 		printf("group[b]: %d\n", group[b]);
	// 		printf("a_nodeId:%d %d\n", a_nodeId, graph[ group[a] ].size());
	// 		printf("b_nodeId:%d %d\n", b_nodeId, graph[ group[b] ].size());
	// 	}
	// 	fprintf(fout,"%d %d %d %d %d\n", i, a_groupId, a_nodeId, b_groupId, b_nodeId);
	// 	fprintf(dijFout, "%d %d %d\n", i, a, b);
	// }
	// fclose(fout);
	// fclose(dijFout);

	// open output files
	for ( int i = 1 ; i <= G ; i++ ) {
		sprintf(outputFilePath, "%s/graph%d.out", outputFileDir, i);
		FILE * fout = fopen(outputFilePath, "w");
		if ( !fout ) {
			fprintf(stderr, "output file %s can not open, please check if the directory is correct.\n", outputFilePath);
			return 0;
		}
		graph[i].fprint(fout);
		
		fclose(fout);
	}
	
	char logFilePath[100];
	sprintf(logFilePath, "%s/newIdOfNodes.log", outputFileDir);
	FILE *fout = fopen(logFilePath, "w");
	for ( int i = 1 ; i <= N ; i++ ) {
		fprintf(fout, "origin id: %5d, G: %3d, id: %4d\n", i, group[i], graph[ group[i] ].relabel(i));
	}
	fclose(fout);

	// char logFilePath[100];
	sprintf(logFilePath, "%s/oriId", argv[1]);
	fout = fopen(logFilePath, "w");
	for ( int i = 1 ; i <= N ; i++ ) {
		fprintf(fout, "%5d%3d%4d\n", i, group[i], graph[ group[i] ].relabel(i));
	}
	fclose(fout);
	
	// delete graph;
	// delete group;
	return 0;
}
