#include<stdio.h>
#include<string.h>
#include<map>
using namespace std;

#define NumOfMachine 10

struct node {
	int N, G, K;
	double pushRelabelTime;
	double pushRelabelNoModified;
	int crossEdgeCount;
	int crossEdgeCountNoModified;
	int queryCount;
	double averageTime;
	double longestTime;
	double getFlowTime;
	double parallelTime;
	node() {
		N = G = K = crossEdgeCount = crossEdgeCountNoModified = queryCount = 0;
		pushRelabelTime = pushRelabelNoModified = averageTime = longestTime = getFlowTime = parallelTime = 0;
	}
	int fulfilled() {
		return N && G && K && crossEdgeCount && pushRelabelTime && averageTime && longestTime && getFlowTime && parallelTime;
	}
	void print() {
		printf("%d, %d, %d, %d, %lf, %d, %lf, %d, %lf, %lf, %lf, %lf", N, G, K, queryCount,
			pushRelabelTime, crossEdgeCount, pushRelabelNoModified, crossEdgeCountNoModified,
			averageTime, longestTime, getFlowTime, parallelTime);
		if ( pushRelabelTime > parallelTime )
			printf(", win\n");
		else
			printf(", lose\n");
	}
};

map<int, node> S;

double getTime() {
	double t;
	scanf("%*s %lf", &t);
	return t;
}

double maxConstraintTime() {
	char s[100];
	double T[NumOfMachine];
	int id;
	double t;
	for ( int i = 0 ; i < NumOfMachine ; i++ )
		T[i] = 0;
	while ( scanf("%s", s) != EOF ) {
		if ( !strcmp("constraint", s) ) {
			scanf("%d%*s%lf", &id, &t);
			T[id%NumOfMachine] += t;
		}
		else {
			break;
		}
	}
	t = 0;
	for ( int i = 0 ; i < NumOfMachine ; i++ )
		t = max(t, T[i]);
	
	return t;
}

inline int scanCompare(const char * B) {
	char s[100];
	scanf("%s", s);
	return !strcmp(s, B);
}

int main()
{
	freopen("testLog0622.log", "r", stdin);
	freopen("data.csv", "w", stdout);
	char s[100];
	node now;
	double count = 0;
	double totalTime = 0;
	double parallelConstraint = 0;
	double parallelGetFlow = 0;
	int id = 0;
	while ( 1 ) {	
		if ( scanf("%s", s) == EOF ) {
			break;
		}
		if ( !strcmp(s, "Case") ) {
			scanf("%d", &id);
			count = totalTime 
				= parallelConstraint = parallelGetFlow = 0;
			now = node();
		}
		else if ( !strcmp(s, "N") ) {
			scanf("%*s%d", &now.N);
		}
		else if ( !strcmp(s, "G") ) {
			scanf("%*s%d", &now.G);
		}
		else if ( !strcmp(s, "K") ) {
			scanf("%*s%d", &now.K);
		}
		else if ( !strcmp(s, "runPushRelabel") ) {
			if ( id < 600 ) {
				now.pushRelabelTime = getTime();
				if ( now.pushRelabelTime != -1 )
					scanf("%*s%*s%*s%*s%d", &now.crossEdgeCount);
				now.queryCount = 1;
			}
			else {
				now.pushRelabelTime = getTime();
				if ( now.pushRelabelTime != -1 ) {
					while ( scanCompare("Cross") ) {
						int A;
						scanf("%*s%*s%*s%d", &A);
						now.crossEdgeCount += A;
					}
					if ( scanCompare("Count") ) {
						scanf("%*s%d", &now.queryCount);
					}
				}
			}
		}
		else if ( !strcmp(s, "calculateConstraints") ) {
			scanf("%*d");
			double t = getTime();
			if ( t != -1 ) {
				totalTime += t;
				count++;
				t = maxConstraintTime();
				if ( t != -1 ) {
					parallelConstraint = max(t, parallelConstraint);
				}
			}
		}
		else if ( !strcmp(s, "Longest") ) {
			now.longestTime = getTime();
		}
		else if ( !strcmp(s, "getFlow") ) {
			now.getFlowTime = getTime();
			if ( id < 600 ) {
				double t;
				while ( scanCompare("repreprocess") ) {
					t = maxConstraintTime();
					if ( t != 0 )
						parallelGetFlow = max(t, parallelGetFlow);
					scanf("%*s");
				}
				// get sequential time
				now.parallelTime = getTime() + parallelGetFlow;
			}
			else {
				double totalQueryTime = 0;
				double t;
				scanf("%s", s);
				while ( !strcmp(s, "getFlow") ) {
					scanf("%*s%*s%s", s);
					while ( !strcmp(s, "repreprocess") ) {
						t = maxConstraintTime();
						if ( t != 0 )
							parallelGetFlow = max(t, parallelGetFlow);
						scanf("%*s%s", s);
					}
					totalQueryTime += parallelGetFlow;
				}
				// get sequential time
				t = getTime();
				if ( t != -1 )
					now.parallelTime = t + totalQueryTime;
			}
		}
		else if ( !strcmp(s, "Yes") ) {
			now.averageTime = totalTime/count;
			if ( now.fulfilled() ) {
				S[id] = now;
			}
		}
		else if ( !strcmp(s, "No") ) {
			fprintf(stderr, "Error case : %d\n", id);
		}
		else if ( !strcmp(s, "runPushRelabelNoModified") ) {
			if ( S.find(id) != S.end() ) {
				S[id].pushRelabelNoModified = getTime();
				scanf("%*s%*s%*s%*s%d", &S[id].crossEdgeCountNoModified);
			}
		}
	}
	printf("id, N, G, K, queryCount, pushRelabelTime, crossEdgeCount, pushRelabelNoModified, crossEdgeCountNoModified, averageTime, longestTime, sequentialGetFlowTime, parallelTime\n");
	for ( map<int, node>::iterator it = S.begin() ; it != S.end() ; it++ ) {
		if ( it->first < 100 )
			continue;
		printf("%d, ", it->first-100+1);
		it->second.print();
	}
	puts("");
	return 0;
}
