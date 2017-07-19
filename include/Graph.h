#ifndef Graph_h
#define Graph_h
#include<stdio.h>
#include<vector>
#include<set>
#include<algorithm>
#include <time.h>
#include "structs.h"
using namespace std;
class Graph {
public:
	int groupId;
	int groupSize;
	int edgeSize;
	time_t updateTime;
	set<int> S;
	set<int> T;
	vector< vector<Edge> > innerEdge;
	vector<LinkInEdge> inEdge;
	vector<LinkOutEdge> outEdge;
	Graph();
	int initWithFile(char * path);
	int size();
	int innerEdgeSize();
	void resize(int A, int B);
	void insertEdge(int A, int B, double w);
	void updateEdge(int A, int B, double w);
	void insertLinkInEdge(LinkInEdge edge);
	void insertLinkOutEdge(LinkOutEdge edge);
	void removeEdge(int A, int B);
	void print();
	void fprint(FILE * fout);

	~Graph();
} ;
#endif
