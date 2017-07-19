#include "Graph.h"
#include "Group.h"
#include "funcs.h"
#include "structs.h"
#include <iostream>
#include <queue>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include "calculate.h"
#include "SimpleGraph.h"
#include "omp.h"
#include "writeLog.h"
#define threadNum 20
// #define GROUP_VISIT_LIMIT 50
#define THEDA 0.2
using namespace std;
int caseNum;
int NumOfGroup;
int queryId;
int *GROUP_VISIT_LIMIT;
void genSimpleGraph(SimpleGraph &G, int a_groupId, int a_nodeId, int b_groupId,
                    int b_nodeId, Graph *originG, Group *originP);
int shortestPath(SimpleGraph &G, int source, int target);
int shortestPath_again(SimpleGraph &G, int source, int target);
void iniSimpleGraph(SimpleGraph &G, Group *originP, Graph *originG);
void shortestPath(Graph &G, int source, int *dis);
void reomveSimpleGraph(SimpleGraph &G, int a_groupId, int a_nodeId,
                       int b_groupId, int b_nodeId, Graph *originG,
                       Group *originP);
void genSimpleGraph(SimpleGraph &G, Graph &a_graph, Graph &b_graph);
void reomveSimpleGraph(SimpleGraph &G, Graph &a_graph, Graph &b_graph);
ID querys[10005][2];
double sssp = 0;
double insert = 0;
double qq = 0;
int main(int args, char *argv[]) {
  if (args < 3) {
    fputs("Too few args ./a.out caseNum NumOfGroup [queryNum]\n", stderr);
    return 0;
  }

  double type1Theda=1, type2Theda=1;
  sscanf(argv[1], "%d", &caseNum);
  sscanf(argv[2], "%d", &NumOfGroup);
  if(args > 3){
    sscanf(argv[3], "%lf", &type1Theda);
    sscanf(argv[3], "%lf", &type2Theda);
  }
  char input[100];
  sprintf(input, "../data/case%d/getDistance.log", caseNum);
  FILE *flog = setLog(input);
  startTime();
  double sequentialTime = 0;
  // fprintf(stderr, "ini originG originP\n");
  // Timer totalTime;
  // totalTime.startTime();
  Timer tm;
  tm.startTime();
  Graph *originG = new Graph[NumOfGroup + 1];
  GROUP_VISIT_LIMIT = (int*) malloc(sizeof(int)*(NumOfGroup+1));
  for (int i = 1; i <= NumOfGroup; i++) {
    sprintf(input, "../data/case%d/graph/graph%d.out", caseNum, i);
    originG[i].initWithFile(input);
    // printf("graph[%d].edgeSize : %d\n", i, originG[i].innerEdgeSize());
  }
  Group *originP = new Group[NumOfGroup + 1];
  for (int i = 1; i <= NumOfGroup; i++) {
    sprintf(input, "../data/case%d/graph/group%d.out", caseNum, i);
    originP[i].initWithFile(input);
    if((double)originG[i].edgeSize/originP[i].shortcutNum > THEDA)
      if(type1Theda == 1)
        GROUP_VISIT_LIMIT[i] = inf;
      else
        GROUP_VISIT_LIMIT[i] = (int)ceil((double)originP[i].S.size()*type1Theda);
    else
      if(type2Theda == 1)
        GROUP_VISIT_LIMIT[i] = inf;
      else
        GROUP_VISIT_LIMIT[i] = (int)ceil((double)originG[i].S.size()*type2Theda);
    // printf("group[%d].shortcutNum : %d\n", i, originP[i].shortcutNum);

  }
  
  

  sprintf(input, "../data/case%d/graph/query.in", caseNum);
  FILE *fin = fopen(input, "r");

  int a_groupId, a_id;
  int b_groupId, b_id;
  double io[threadNum+1] = {0};
  double compute[threadNum+1] = {0};

  fprintf(stderr, "getDistance query start\n");
  int tn;
  fscanf(fin, "%d", &tn);
  while (fscanf(fin, "%d%d%d%d%d", &queryId, &a_groupId, &a_id, &b_groupId,
                &b_id) != EOF) {
    querys[queryId][0] = ID(a_groupId, a_id);
    querys[queryId][1] = ID(b_groupId, b_id);
  }
  // if (args > 3) {
  //   sscanf(argv[3], "%d", &tn);
  // }
  fclose(fin);
  SimpleGraph iniSimple[30];
  
  
  omp_set_num_threads(threadNum);
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < threadNum+1; i++)
    iniSimpleGraph(iniSimple[i], originP, originG);
  // io[0] = t.endTime();
  // fprintf(stderr, "finish iniSimple\n");
  fprintf(stderr, "prepare: %lf ", tm.endTime());    
  char path[100];
  sprintf(path, "../data/case%d/output.out", caseNum);
  FILE *fout = fopen(path, "w+");
  Timer t;
  t.startTime();
  int cannot_go[threadNum] = {0};
#pragma omp parallel for schedule(dynamic)
  for (int i = 1; i <= tn; i++) {

    ID a = querys[i][0];
    ID b = querys[i][1];

    int id = omp_get_thread_num();
    // Timer t1;
    int ans;
    // t1.startTime();
    // printf("graph[27] :%d\n", originG[27].size());
    // genSimpleGraph(iniSimple[id], originG[ a.groupId ], originG[ b.groupId ]);
    // iniSimple[id].print();
    genSimpleGraph(iniSimple[id], a.groupId, a.nodeId, b.groupId, b.nodeId, originG, originP);
    // io[id] += t1.endTime();
    // Timer comt;

    // iniSimple[id].print();
    int source = iniSimple[id].relabel(a);
    int target = iniSimple[id].relabel(b);
    // comt.startTime();
    // printf("query id %d\n", i);
    // printf("%d %d %d %d\n", a.groupId, a.nodeId, b.groupId, b.nodeId);
    // iniSimple[id].print();  
    // printf("%d %d\n", source, target);
    ans = shortestPath(iniSimple[id], source, target);
    // compute[id] += comt.endTime();
    if (ans == inf){
      // ans = shortestPath_again(iniSimple[id], source, target); 
      cannot_go[id]++;
    }
    
    fprintf(fout, "%d %d\n", i, ans);
    // fprintf(stderr, "ans: %d\n", ans);
    // t1.startTime();
    // reomveSimpleGraph(iniSimple[id], originG[ a.groupId ], originG[ b.groupId ]);
    reomveSimpleGraph(iniSimple[id], a.groupId, a.nodeId, b.groupId, b.nodeId, originG, originP);
    // io[id] += t1.endTime();
  }
  
  fprintf(stderr, "node: %d edge: %d\n", iniSimple[0].size(),
          iniSimple[0].innerEdgeSize());
  double tmp = t.endTime();
  fprintf(stderr, "Time: %lf\n", tmp);
  fprintf(stdout, " %lf", tmp);
  // fprintf(stdout, "io: %lf, compute: %lf\n", io[0], compute[1]);
  // fprintf(stdout, "sssp: %lf, insert: %lf, qq: %lf\n", sssp, insert, qq);
  
  // sequentialTime += endTime(0);
  // fprintf(flog, "Sequential ");
  // printTime(sequentialTime);
  // fprintf(flog, "Total Time: %lf\n", totalTime.endTime());
  // for(int i=1;i<=100;i++){
  //   fprintf(stderr, "%d\t%d\n", i, GROUP_VISIT_LIMIT[i]);
  // }
  for(int i=0, tmp=0;i<threadNum;i++){
    tmp+=cannot_go[i];
    if(i==threadNum-1)
      fprintf(stderr, "cannot_go: %d\n", tmp);
  }
  closeLog();

  return 0;
}

struct Node {
  int id;
  int dis;
};
bool operator<(const Node &n1, const Node &n2) { return n1.dis > n2.dis; }

void shortestPath(Graph &G, int source, int *dis) {
  // printf("shortestPath %d %d %d %d\n", S.groupId, S.nodeId, T.groupId,
  // T.nodeId);
  int n = G.size();
  // G.print();
  // printf("n :%d\n", n);
  bool *visit = new bool[n + 1];
  for (int i = 1; i <= n; i++) {
    visit[i] = false;
    dis[i] = inf;
  }
  priority_queue<Node> PQ;

  // printf("source :%d\n", source);
  // printf("graph id: %d\n", G.groupId);
  visit[source] = true;
  dis[source] = 0;
  Node s;
  s.id = source;
  s.dis = 0;
  PQ.push(s);
  for (int k = 0; k < n; k++) {

    int a = -1;
    while (!PQ.empty() && visit[a = PQ.top().id]) {
      PQ.pop();
    }
    if (a == -1)
      break;
    visit[a] = true;
    // printf("head:%d %d\n", a, dis[a]);
    if(G.innerEdge.size()>a){
      for (int i = 0; i < G.innerEdge[a].size(); i++) {
        int B = G.innerEdge[a][i].B;
        int w = G.innerEdge[a][i].w;
        if (!visit[B] && dis[a] + w < dis[B]) {
          dis[B] = dis[a] + w;
          Node node;
          node.id = B;
          node.dis = dis[B];
          // printf("visit: %d %d\n", B, dis[B]);
          PQ.push(node);
        }
      }
    }
    
  }

  while (!PQ.empty())
    PQ.pop();

  return;
}

int shortestPath_again(SimpleGraph &G, int source, int target) {
  // printf("shortestPath %d %d %d %d\n", S.groupId, S.nodeId, T.groupId,
  // T.nodeId);
  int n = G.size();
  // printf("%d\n", n);
  int *dis = new int[n + 1];
  bool *visit = new bool[n + 1];
  int *groupVisit = new int[NumOfGroup + 1];
  for (int i=1;i<=NumOfGroup;i++)
    groupVisit[i] = 0;
  for (int i = 1; i <= n; i++) {
    visit[i] = false;
    dis[i] = inf;
  }
  priority_queue<Node> PQ;

  // printf("%d %d\n", source, target);
  visit[source] = true;
  dis[source] = 0;
  Node s;
  s.id = source;
  s.dis = 0;
  PQ.push(s);

  for (int k = 0; k < n; k++) {

    int a = -1;
    while (!PQ.empty() && visit[a = PQ.top().id]) {
      // printf("%d %d\n", groupVisit[G.groupIdOf(a)], GROUP_VISIT_LIMIT);
      PQ.pop();
    }
    if (a == -1)
      break;
    // if(groupVisit[G.groupIdOf(a)] >= GROUP_VISIT_LIMIT){
    //  k--;
    //  continue;
    // }
    // printf("top: %d %d %d\n", G.groupIdOf(a), G.nodeIdOf(a), dis[a]);
    int a_groupId = G.groupIdOf(a);

    visit[a] = true;
    if (a == target)
      return dis[a];
    // printf("%d\n", G.innerEdge[G.relabel(a)].size());
    int out = 0;
    if(G.innerEdge.size()>a){
      for (int i = 0; i < G.innerEdge[a].size(); i++) {
        int B = G.innerEdge[a][i].B;
        int w = G.innerEdge[a][i].w;
        int b_groupId = G.groupIdOf(B);
        // printf("!!%d %d %d %d\n", G.groupIdOf(B), G.nodeIdOf(B), dis[B], w);
        // if (!visit[B] && dis[a] + w < dis[B] &&
        //     groupVisit[b_groupId] < GROUP_VISIT_LIMIT[b_groupId]) {
        if (!visit[B] && dis[a] + w < dis[B]) {
          dis[B] = dis[a] + w;
          Node node;
          node.id = B;
          node.dis = dis[B];
          // printf("%d %d %d %d\n", G.groupIdOf(B), G.nodeIdOf(B), dis[B], w);
          PQ.push(node);
          if (a_groupId != b_groupId)
            out = 1;
        }
      }
    }
    
    // #pragma omp critical
    // {
    //   if (out == 1)
    //     GROUP_VISIT_LIMIT[a_groupId]++;
    // }
    // if (out == 1)
    //     groupVisit[a_groupId]++;
  }

  while (!PQ.empty())
    PQ.pop();

  return inf;
}

int shortestPath(SimpleGraph &G, int source, int target) {
  // printf("shortestPath %d %d %d %d\n", S.groupId, S.nodeId, T.groupId,
  // T.nodeId);
  int n = G.size();
  // printf("%d\n", n);
  int *dis = new int[n + 1];
  bool *visit = new bool[n + 1];
  int *groupVisit = new int[ NumOfGroup + 1];
  for (int i=0;i<=NumOfGroup; i++)
    groupVisit[i] = 0;
  for (int i = 1; i <= n; i++) {
    visit[i] = false;
    dis[i] = inf;
  }
  priority_queue<Node> PQ;

  // printf("%d %d\n", source, target);
  visit[source] = true;
  dis[source] = 0;
  Node s;
  s.id = source;
  s.dis = 0;
  PQ.push(s);

  for (int k = 0; k < n; k++) {

    int a = -1;
    while (!PQ.empty() && visit[a = PQ.top().id]) {
      // printf("%d %d\n", groupVisit[G.groupIdOf(a)], GROUP_VISIT_LIMIT);
      PQ.pop();
    }
    if (a == -1)
      break;
    // if(groupVisit[G.groupIdOf(a)] >= GROUP_VISIT_LIMIT){
    // 	k--;
    // 	continue;
    // }
    // printf("top: %d %d %d\n", G.groupIdOf(a), G.nodeIdOf(a), dis[a]);
    int a_groupId = G.groupIdOf(a);

    visit[a] = true;
    if (a == target)
      return dis[a];
    // printf("%d\n", G.innerEdge[G.relabel(a)].size());
    int out = 0;
    if(G.innerEdge.size()>a){
      for (int i = 0; i < G.innerEdge[a].size(); i++) {
        int B = G.innerEdge[a][i].B;
        int w = G.innerEdge[a][i].w;
        int b_groupId = G.groupIdOf(B);
        // printf("!!%d %d %d %d\n", G.groupIdOf(B), G.nodeIdOf(B), dis[B], w);
        if (!visit[B] && dis[a] + w < dis[B] &&
            groupVisit[b_groupId] < GROUP_VISIT_LIMIT[b_groupId]) {
        // if (!visit[B] && dis[a] + w < dis[B]) {
          dis[B] = dis[a] + w;
          Node node;
          node.id = B;
          node.dis = dis[B];
          // printf("%d %d %d %d\n", G.groupIdOf(B), G.nodeIdOf(B), dis[B], w);
          PQ.push(node);
          if (a_groupId != b_groupId)
            out = 1;
        }
      }
    }
    
    // #pragma omp critical
    // {
    //   if (out == 1)
    //     GROUP_VISIT_LIMIT[a_groupId]++;
    // }
    if (out == 1)
        groupVisit[a_groupId]++;
  }

  while (!PQ.empty())
    PQ.pop();

  return inf;
}

void iniSimpleGraph(SimpleGraph &G, Group *originP, Graph *originG) {
  char input[100];
  for (int id = 1; id <= NumOfGroup; id++) {
    Group p;
    Graph g;
    p = originP[id];
    g = originG[id];
    if ( (double)g.edgeSize/p.shortcutNum > THEDA){
      int SN = p.S.size();
      int TN = p.T.size();
      for (int i = 0; i < SN; i++) {
        for (int j = 0; j < TN; j++) {
          if (p.S[i] != p.T[j] && p.distance[i][j] != inf) {
            // printf("2(%d %d) -> (%d %d) = %d\n", id, p.S[i], id, p.T[j],
            // p.distance[i][j]);
            G.insertEdge(ID(id, p.S[i]), ID(id, p.T[j]), p.distance[i][j]);
          }
        }
      }
    }
    else{
      for ( int i = 0 ; i < g.innerEdge.size() ; i++ ) {
        for ( int j = 0 ; j < g.innerEdge[i].size() ; j++ ) {
          G.insertEdge(ID(id, i), ID(id, g.innerEdge[i][j].B), g.innerEdge[i][j].w);
        }
      }
    }

    int inEdgeNum = originP[id].inEdge.size();
    for (int i = 0; i < inEdgeNum; i++) {
      // printf("3(%d %d) -> (%d %d) = %d\n", originP[id].inEdge[i].A.groupId,
      // originP[id].inEdge[i].A.nodeId,
      // id, originP[id].inEdge[i].B, originP[id].inEdge[i].w);
      G.insertEdge(originP[id].inEdge[i].A, ID(id, originP[id].inEdge[i].B),
                   originP[id].inEdge[i].w);
    }
  }

  G.mergeMultipleEdges();
}

void reomveSimpleGraph(SimpleGraph &G, int a_groupId, int a_nodeId,
                       int b_groupId, int b_nodeId, Graph *originG,
                       Group *originP) {

  Timer t;

  if (originG[a_groupId].S.find(a_nodeId) == originG[a_groupId].S.end()) {

    int len = originP[a_groupId].T.size();
    // t.startTime();
    for (int i = 0; i < len; i++)
      G.removeEdge(ID(a_groupId, a_nodeId),
                   ID(a_groupId, originP[a_groupId].T[i]));
    // insert += t.endTime();
    if(a_groupId == b_groupId)
      G.removeEdge(ID(a_groupId, a_nodeId), ID(a_groupId, b_nodeId));
  }

  if (originG[b_groupId].S.find(b_nodeId) == originG[b_groupId].S.end()) {

    int len = originP[b_groupId].S.size();
    // t.startTime();
    for (int i = 0; i < len; i++)
      G.removeEdge(ID(b_groupId, originP[b_groupId].S[i]),
                   ID(b_groupId, b_nodeId));
    // insert += t.endTime();
  }
}

void reomveSimpleGraph(SimpleGraph &G, Graph &a_graph, Graph &b_graph){

  for ( int i = 0 ; i < a_graph.innerEdge.size() ; i++ ) {
    for ( int j = 0 ; j < a_graph.innerEdge[i].size() ; j++ ) {
      G.removeEdge(ID(a_graph.groupId, i), ID(a_graph.groupId, a_graph.innerEdge[i][j].B));
    }
  }

  if(a_graph.groupId != b_graph.groupId){
    for ( int i = 0 ; i < b_graph.innerEdge.size() ; i++ ) {
      for ( int j = 0 ; j < b_graph.innerEdge[i].size() ; j++ ) {
        G.removeEdge(ID(b_graph.groupId, i), ID(b_graph.groupId, b_graph.innerEdge[i][j].B));
      }
    }
  }
}

void genSimpleGraph(SimpleGraph &G, Graph &a_graph, Graph &b_graph){

  for ( int i = 0 ; i < a_graph.innerEdge.size() ; i++ ) {
    for ( int j = 0 ; j < a_graph.innerEdge[i].size() ; j++ ) {
      G.insertEdge(ID(a_graph.groupId, i), ID(a_graph.groupId, a_graph.innerEdge[i][j].B), a_graph.innerEdge[i][j].w);
    }
  }

  if(a_graph.groupId != b_graph.groupId){
    for ( int i = 0 ; i < b_graph.innerEdge.size() ; i++ ) {
      for ( int j = 0 ; j < b_graph.innerEdge[i].size() ; j++ ) {
        G.insertEdge(ID(b_graph.groupId, i), ID(b_graph.groupId, b_graph.innerEdge[i][j].B), b_graph.innerEdge[i][j].w);
      }
    }
  }
}

void genSimpleGraph(SimpleGraph &G, int a_groupId, int a_nodeId, int b_groupId,
                    int b_nodeId, Graph *originG, Group *originP) {

  Timer t;

  // printf("graph[27] :%d\n", originG[27].size());
  if (originG[a_groupId].S.find(a_nodeId) == originG[a_groupId].S.end()) {
    int dis[originG[a_groupId].size() + 1];
    // t.startTime();
    // printf("a_groupId :%d %d\n", a_groupId, a_nodeId);
    shortestPath(originG[a_groupId], a_nodeId, dis);
    // sssp += t.endTime();

    int len = originP[a_groupId].T.size();
    // t.startTime();
    for (int i = 0; i < len; i++)
      G.insertEdge(ID(a_groupId, a_nodeId),
                   ID(a_groupId, originP[a_groupId].T[i]),
                   dis[originP[a_groupId].T[i]]);
    if(a_groupId == b_groupId)
      G.insertEdge(ID(a_groupId, a_nodeId), ID(a_groupId, b_nodeId), dis[b_nodeId]);
    // insert += t.endTime();
  }
  // printf("graph[27] :%d\n", originG[27].size());
  if (originG[b_groupId].S.find(b_nodeId) == originG[b_groupId].S.end()) {
    int dis1[originG[b_groupId].size() + 1];
    // t.startTime();
    // printf("b_groupId :%d %d\n", b_groupId, originG[b_groupId].size());
    shortestPath(originG[b_groupId], b_nodeId, dis1);
    // sssp += t.endTime();

    int len = originP[b_groupId].S.size();
    // t.startTime();
    for (int i = 0; i < len; i++)
      G.insertEdge(ID(b_groupId, originP[b_groupId].S[i]),
                   ID(b_groupId, b_nodeId), dis1[originP[b_groupId].S[i]]);
    // insert += t.endTime();

  }

}
