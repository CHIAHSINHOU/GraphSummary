#include <stdio.h>
#include <vector>

using namespace std;

int main(){

	vector <int> group;
	vector <int> innerEdge;
	vector <int> connectedNode;
	vector <int> k;
	vector <int> groupNum;
	int n, g, a, b, c;
	scanf("%d%d", &n, &g);
	group.resize(n+1);
	innerEdge.resize(g+1);
	connectedNode.resize(n+1);
	k.resize(g+1);
	groupNum.resize(g+1);
	for(int i=1; i<=n; i++){
		scanf("%d", &group[i]);
	}

	while(scanf("%d%d%d", &a, &b, &c) && a!=0 && b!=0 && c!=0){
		if(group[a] == group[b])
			innerEdge[group[a]]++;
		else{
			connectedNode[a]++;
			connectedNode[b]++;
		}
	}

	for(int i=1;i<=n;i++){
		groupNum[group[i]]++;
		if(connectedNode[i] > 0){
			k[group[i]]++;
		}
	}

	for(int i=1;i<=g;i++){
		fprintf(stderr, "%d\t%d\t%d\n", groupNum[i], innerEdge[i], k[i]);
		fprintf(stdout, "group %d node: %d innerEdge: %d k:%d\n", i, groupNum[i], innerEdge[i], k[i]);
	}


	return 0;
}