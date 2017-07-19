#include "Graph.h"
#include "Group.h"
#include "funcs.h"
#include "structs.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SimpleGraph.h"
#include "omp.h"
using namespace std;

void initBigGraph(int caseNum, SimpleGraph &SG, int G) {
	char input[100];
	Graph *originG = new Graph[G + 1];
	for (int i = 1; i <= G; i++) {
		sprintf(input, "../data/case%d/graph/graph%d.out", caseNum, i);
		originG[i].initWithFile(input);
		// printf("graph[%d].edgeSize : %d\n", i, originG[i].innerEdgeSize());
	}
	for (int id = 1; id <= G; id++) {
		for ( int i = 0 ; i < originG[id].innerEdge.size() ; i++ ) {
			for ( int j = 0 ; j < originG[id].innerEdge[i].size() ; j++ ) {
				SG.insertEdge(ID(id, i), ID(id, originG[id].innerEdge[i][j].B), originG[id].innerEdge[i][j].w);
			}
		}

		int inEdgeNum = originG[id].inEdge.size();
		for (int i = 0; i < inEdgeNum; i++) {
			// printf("3(%d %d) -> (%d %d) = %d\n", originP[id].inEdge[i].A.groupId,
			// originP[id].inEdge[i].A.nodeId,
			// id, originP[id].inEdge[i].B, originP[id].inEdge[i].w);
			SG.insertEdge(originG[id].inEdge[i].A, ID(id, originG[id].inEdge[i].B),
					originG[id].inEdge[i].w);
		}
	}

	SG.mergeMultipleEdges();
	delete[] originG;
}
/*
void updateGenerator(int caseNum) {
	SimpleGraph G;
	initBigGraph(caseNum, G);
	int prefixSum[G.size()+1];
	prefixSum[0] = 0;
	for (int i = 1; i <= G.size(); i++ ) {
		prefixSum = prefixSum[i-1] + G.edgeSize;
	}
	int edgeTotal = prefixSum[G];

	int updateTime = 10;
	int waitSec = 30;
	while (updateTime--) {
		Edge E;
		int edge = rand() % (edgeTotal)+1;
		for (int i = 1; i <= G.size(); i++) {
			if (prefixSum[i] >= edge) {
				E = G.innerEdge[i][edge-prefixSum[i-1]];
			}
		}
		E.update(rand(100000));
		for (int i = 0; i < summaryProcID.size(); i++) {
			
		}
		MPI_Send()
	}
}*/
