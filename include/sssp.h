#ifndef SSSP_h
#define SSSP_h

#include "Graph.h"
#include "SimpleGraph.h"

double ssspSimple(SimpleGraph &G, int source, int target, int *from);
double sssp(Graph &G, int source, int target, vector<int> &path);
int* sssp(double *dis, Graph &G, int source);
void addPath(vector<int> &path, int now, int *from);

#endif