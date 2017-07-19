#include "sssp.h"
#include <queue>
#include <math.h>

using namespace std;

struct Node {
  int id;
  double dis;
};
bool operator<(const Node &n1, const Node &n2) { return n1.dis > n2.dis; }

void addPath(vector<int> &path, int now, int *from){

  if(from[now] != now)
    addPath(path, from[now], from);
  path.push_back(now);
}

double ssspSimple(SimpleGraph &G, int source, int target, int *from) {

 // fprintf(stderr, "s:%d t:%d G.size %d\n", source, target, G.size());
  int n = G.size();
  double *dis = new double[n + 1];
  bool *visit = new bool[n + 1];

  for (int i = 1; i <= n; i++) {
    visit[i] = false;
    dis[i] = inf;
    from[i] = i;
  }
  priority_queue<Node> PQ;

  visit[source] = true;
  dis[source] = 0;
  Node s;
  s.id = source;
  s.dis = 0;
  PQ.push(s);
  //fprintf(stderr, "G.innerEdge.size = %d\n", G.innerEdge.size());

  for (int k = 0; k < n; k++) {

    int a = -1;
    while (!PQ.empty()) {
      // if(PQ.top().id > G.size())
      //     fprintf(stderr, "!!!!!!!!!!!!!!!!! %d %d %d\n", G.groupId, G.size(), PQ.top().id);
      if(visit[a = PQ.top().id])
        PQ.pop();
      else
        break;
    }
    if (a == -1)
      break;
   

    visit[a] = true;
    if (a == target){
      //fprintf(stderr, "s:%d t:%d ans:%d\n", source, target, dis[a]);
      return dis[a];
    }

    int out = 0;

    if(G.innerEdge.size()>a){
      for (int i = 0; i < G.innerEdge[a].size(); i++) {

        int B = G.innerEdge[a][i].B;
        double w = G.innerEdge[a][i].w;
        // if(B > G.size())
        //   fprintf(stderr, "!!!!!!!! B %d %d %d->%d\n", G.groupId, G.size(), a, B);
        if (!visit[B] && dis[a] + w < dis[B]){
          dis[B] = dis[a] + w;
          Node node;
          node.id = B;
          node.dis = dis[B];
          from[B] = a;
          PQ.push(node);
        }
      }
    }

  }

  while (!PQ.empty())
    PQ.pop();
  //fprintf(stderr, "s:%d t:%d ans: inf\n", source, target);
  delete[] visit;
  delete[] dis;
  return inf;
}

double sssp(Graph &G, int source, int target, vector<int> &path) {

  fprintf(stderr, "s:%d t:%d G.size %d\n", source, target, G.size());
  int n = G.size();
  double *dis = new double[n + 1];
  bool *visit = new bool[n + 1];
  int *from = new int[ n + 1]; 

  for (int i = 1; i <= n; i++) {
    visit[i] = false;
    dis[i] = inf;
    from[i] = i;
  }
  priority_queue<Node> PQ;

  visit[source] = true;
  dis[source] = 0;
  Node s;
  s.id = source;
  s.dis = 0;
  PQ.push(s);
  //fprintf(stderr, "G.innerEdge.size = %d\n", G.innerEdge.size());

  for (int k = 0; k < n; k++) {

    int a = -1;
    while (!PQ.empty()) {
      // if(PQ.top().id > G.size())
      //     fprintf(stderr, "!!!!!!!!!!!!!!!!! %d %d %d\n", G.groupId, G.size(), PQ.top().id);
      if(visit[a = PQ.top().id])
        PQ.pop();
      else
        break;
    }
    if (a == -1)
      break;
   

    visit[a] = true;
    if (a == target){
      //fprintf(stderr, "s:%d t:%d ans:%d\n", source, target, dis[a]);
      addPath(path, target, from);
      return dis[a];
    }

    int out = 0;

    if(G.innerEdge.size()>a){
      for (int i = 0; i < G.innerEdge[a].size(); i++) {

        int B = G.innerEdge[a][i].B;
        double w = G.innerEdge[a][i].w;
        // if(B > G.size())
        //   fprintf(stderr, "!!!!!!!! B %d %d %d->%d\n", G.groupId, G.size(), a, B);
        if (!visit[B] && dis[a] + w < dis[B]){
          dis[B] = dis[a] + w;
          Node node;
          node.id = B;
          node.dis = dis[B];
          from[B] = a;
          PQ.push(node);
        }
      }
    }

  }

  while (!PQ.empty())
    PQ.pop();
  //fprintf(stderr, "s:%d t:%d ans: inf\n", source, target);
  addPath(path, target, from);
  delete[] visit;
  delete[] dis;
  delete[] from;
  return inf;
}

int* sssp(double *dis, Graph &G, int source){

	int n = G.size();
  
	bool *visit = new bool[n + 1];
  int *from = new int[n+1];
	for (int i = 1; i <= n; i++) {
		visit[i] = false;
	    dis[i] = inf;
      from[i] = i;
	}
	priority_queue<Node> PQ;


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

		if(G.innerEdge.size()>a){
		  for (int i = 0; i < G.innerEdge[a].size(); i++) {
		    int B = G.innerEdge[a][i].B;
		    double w = G.innerEdge[a][i].w;
		    if (!visit[B] && dis[a] + w < dis[B]) {
		      dis[B] = dis[a] + w;
		      Node node;
		      node.id = B;
		      node.dis = dis[B];
          from[B] = a;
		      PQ.push(node);
		    }
		  }
		}

	}

	while (!PQ.empty())
	PQ.pop();

  delete[] visit;

	return from;

}