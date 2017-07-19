#ifndef SUMMARY_h
#define SUMMARY_h

#include "Graph.h"
#include "Group.h"

void calculateSummary(Graph &graph, Group &group);
double calculateSummaryST(vector<int> &path, Graph &graph, int source, int target);
void calculateSummarySource(vector<int> &paths, int *pathPrefix, int *edges, double *values, int &edgeNum, Graph &graph, int source);
void calculateSummaryTarget(vector<int> &paths, int *pathPrefix, int *edges, double *values, int &edgeNum, Graph &graph, int target);

#endif