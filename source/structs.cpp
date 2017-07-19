#include "structs.h"
using namespace std;

// ID
ID::ID() {
	groupId = nodeId = 0;
}
ID::ID(int _groupId, int _nodeId) {
	groupId = _groupId;
	nodeId = _nodeId;
}
	
// edge
Edge::Edge() {
	B = 0;
	w = 0;
}
Edge::Edge(int _B, double _w) {
	B = _B;
	w = _w;
}
void Edge::update(double _w) {
	w = _w;
}
bool Edge::operator < (const Edge &t) const {
	if ( B != t.B )
		return B < t.B;
	return w < t.w;
}


// LinkInEdge
LinkInEdge::LinkInEdge() {
	B = w = 0;
}
LinkInEdge::LinkInEdge(ID _A, int _B, double _w) {
	A = _A;
	B = _B;
	w = _w;
}
void LinkInEdge::fprint(FILE * fout) {
	fprintf(fout, "%d %d %d %lf\n", A.groupId, A.nodeId, B, w);
}

// LinkOutEdge
LinkOutEdge::LinkOutEdge() {
	A = w = 0;
}
LinkOutEdge::LinkOutEdge(int _A, ID _B, double _w) {
	A = _A;
	B = _B;
	w = _w;
}
void LinkOutEdge::fprint(FILE * fout) {
	fprintf(fout, "%d %d %d %lf\n", A, B.groupId, B.nodeId, w);
}

// Relabel
Relabel::Relabel() {
	originId.push_back(ID(0,0));
}
int Relabel::size() {
	return newId.size();
}
int Relabel::relabel(ID a) {
	if ( newId.find(a) == newId.end() ) {
		newId[a] = newId.size();
		originId.push_back(a);
		// printf("%d ID(%d %d)\n", newId.size()-1, a.groupId, a.nodeId);
	}
	return newId[a];
}
ID Relabel::recover(int a) {
	return originId[a];
}
int Relabel::groupIdOf(int a) {
	return originId[a].groupId;
}
int Relabel::nodeIdOf(int a) {
	return originId[a].nodeId;
}
void Relabel::clear() {
	newId.clear();
	originId.clear();
}
void Relabel::fprint(FILE * fout) {
	fprintf(fout, "%d\n", size());
	for ( int i = 1 ; i <= size() ; i++ ) {
		fprintf(fout, "(%d,%d) ", originId[i].groupId, originId[i].nodeId);
	}
	fprintf(fout, "\n");
}
Relabel::~Relabel() {
	newId.clear();
	originId.clear();
}
