#ifndef Group_h
#define Group_h
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include "structs.h"
#include "Graph.h"
#include "funcs.h"

using namespace std;

class Group {
public:
	int groupId;
	int K;
	int type;
	Relabel newId;
	double** distance;
	int shortcutNum;
	time_t updateTime;
	vector<int> S;
	vector<int> T;
	vector<LinkInEdge> inEdge;
	vector<LinkOutEdge> outEdge;
	vector< vector< vector<int> > > path;

	inline int size();
	int relabel(ID A);
	ID recover(int a);
	void initWithGraph(Graph &G);
	int initWithFile(char * path);

	void fprint(int caseNum);
	void fprint(FILE * fout);
	void fprint(int caseNum, Graph &G);
	void fprint(FILE * fout, Graph &G);
	Group();
	Group(Graph &G);
	~Group();
} ;

#endif
