#ifndef structs_h
#define structs_h
#include<stdio.h>
#include<vector>
#include<map>

using namespace std;

const int inf = 1 << 30;

class ID {
public:
	int groupId;
	int nodeId;
	ID();
	ID(int _groupId, int _nodeId);
	bool operator < (const ID B) const {
		if ( groupId != B.groupId )
			return groupId < B.groupId;
		return nodeId < B.nodeId;
	}
	bool operator==(const ID B) const {
		if (groupId == B.groupId && nodeId == B.nodeId)
			return true;
		return false;
	}
} ;

class Edge {
public:
	int B;
	double w;
	Edge();
	Edge(int _B, double _w);
	void update(double _w);
	bool operator < (const Edge &t) const;
} ;

class LinkInEdge {
public:
	ID A;
	int B;
	double w;
	LinkInEdge();
	LinkInEdge(ID _A, int _B, double _w);
	void fprint(FILE * fout);
} ;

class LinkOutEdge {
public:
	ID B;
	int A;
	double w;
	LinkOutEdge();
	LinkOutEdge(int _A, ID _B, double _w);
	void fprint(FILE * fout);
} ;

class Relabel {
public:
	map<ID, int> newId;
	vector<ID> originId;
	Relabel();
	int size();
	int relabel(ID a);
	ID recover(int a);
	int groupIdOf(int a);
	int nodeIdOf(int a);
	void clear();
	void fprint(FILE * fout);
	~Relabel();
	Relabel& operator = (Relabel B) {
		newId = B.newId;
		originId = B.originId;
		return *this;
	}
} ;
#endif