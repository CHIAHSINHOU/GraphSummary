#include"SimpleGraph.h"
#include <math.h>


int SimpleGraph::size() {
	return newId.size();
}

int SimpleGraph::relabel(ID A) {
	int tmp = newId.relabel(A);
	groupSize = newId.size();
	return tmp;
}

int SimpleGraph::groupIdOf(int A) {
	return newId.groupIdOf(A);
}

int SimpleGraph::nodeIdOf(int A) {
	return newId.nodeIdOf(A);
}

// always insert inner edge
void SimpleGraph::insertEdge(ID A, ID B, double w) {
	int a = relabel(A);
	int b = relabel(B);

	
		//while(1);
	//}
	
	resize(a, b);
	innerEdge[a].push_back(Edge(b,w));
	edgeSize++;
	
	// printf("%d\n", edgeSize);
}

void SimpleGraph::removeEdge(ID A, ID B) {
	int a = relabel(A);
	int b = relabel(B);
	edgeSize--;
	removeEdge(a,b);
}

void SimpleGraph::updateEdge(ID A, ID B, double w) {
	int a = relabel(A);
	int b = relabel(B);
	for (int i = 0 ; i < innerEdge[a].size() ; i++) {
		if (innerEdge[a][i].B == b) {
			innerEdge[a][i].w = w;
			time(&updateTime);
			return;
		}
	}
	insertEdge(A,B,w);
	time(&updateTime);
}

void SimpleGraph::updateEdgeRatio(ID A, ID B, double w) {
	int a = relabel(A);
	int b = relabel(B);
	for (int i = 0 ; i < innerEdge[a].size() ; i++) {
		if (innerEdge[a][i].B == b) {
			innerEdge[a][i].w *= w/100;
			time(&updateTime);
			return;
		}
	}
	insertEdge(A,B,w);
	time(&updateTime);
}

void SimpleGraph::insertS(ID A) {
	S.insert(relabel(A));
}

void SimpleGraph::insertT(ID A) {
	T.insert(relabel(A));
}

void SimpleGraph::insertLinkInEdge(ID A, ID B, double c) {
	insertLinkInEdge(LinkInEdge(A, relabel(B), c));
}

void SimpleGraph::insertLinkOutEdge(ID A, ID B, double c) {
	insertLinkOutEdge(LinkOutEdge(relabel(A), B, c));
}

void SimpleGraph::mergeMultipleEdges() {
	// merge multiple edges
	for ( int i = 0 ; i < innerEdge.size() ; i++ ) {
		sort(innerEdge[i].begin(), innerEdge[i].end());
		int j, k;
		for ( j = 0 , k = 1 ; k < innerEdge[i].size() ; k++ ) {
			if ( innerEdge[i][j].B == innerEdge[i][k].B ) {
					innerEdge[i][j].w = min(innerEdge[i][j].w, innerEdge[i][k].w);
					edgeSize--;
			}
			else {
				innerEdge[i][++j] = innerEdge[i][k];
			}
		}
		if ( j )
			innerEdge[i].resize(j+1);
	}
}

void SimpleGraph::print() {
	Graph::print();
	newId.fprint(stdout);
}

void SimpleGraph::clear() {
	for (int i = 0 ; i < innerEdge.size() ; i++) {
		innerEdge[i].clear();
	}
	innerEdge.clear();
	newId.clear();
}
