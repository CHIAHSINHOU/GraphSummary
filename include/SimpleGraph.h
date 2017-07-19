#ifndef SimpleGraph_h
#define SimpleGraph_h
#include "Graph.h"
class SimpleGraph : public Graph {
	Relabel newId;
	
public:
	map< pair<int,int>, vector<int> > path;
	int size();
	int relabel(ID A);
	int groupIdOf(int A);
	int nodeIdOf(int A);
	void insertS(ID A);
	void insertT(ID A);
	using Graph::insertLinkInEdge;
	using Graph::insertLinkOutEdge;
	using Graph::removeEdge;
	void insertEdge(ID A, ID B, double c);
	void removeEdge(ID A, ID B);
	void updateEdge(ID A, ID B, double c);
	void updateEdgeRatio(ID A, ID B, double c);
	void insertLinkInEdge(ID A, ID B, double c);
	void insertLinkOutEdge(ID A, ID B, double c);
	void mergeMultipleEdges();
	void print();
	void clear();
	// SimpleGraph& operator = (SimpleGraph B) {
	// 	newId = B.newId;
	// 	innerEdge = B.innerEdge;
	// 	return *this;
	// }
};

#endif
