#ifndef funcs_h
#define funcs_h
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<set>
#include "structs.h"
using namespace std;

int indexOf(int v, vector<int> &V);
int indexOf(int v, vector<Edge> &V);

void fprintIntVector(FILE * fout, vector<int> &V);
void fgetIntVector(FILE * fin, vector<int> &V);
void fprintEdgeVector(FILE * fout, vector<LinkInEdge> &V);
void fgetEdgeVector(FILE * fin, vector<LinkInEdge> &V);
void fprintEdgeVector(FILE * fout, vector<LinkOutEdge> &V);
void fgetEdgeVector(FILE * fin, vector<LinkOutEdge> &V);

void setToVector(set<int> &S, vector<int> &V);

void stop(); 
// void pause();

#endif
