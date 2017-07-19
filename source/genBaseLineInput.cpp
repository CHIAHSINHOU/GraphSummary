#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "writeLog.h"
#include "parseLog.h"

vector<double> value;
vector<int> sentQuery;
int main(int args, char *argv[])
{
	if (args != 2) {
		printf("need testcase #\n");
		return 0;
	}
	int t;
	sscanf(argv[1], "%d", &t);
	char s[100];
	sprintf(s, "../Log/%d/updateLog", t);
	FILE *update = fopen(s, "r");
	if (!update) {
		printf("Fail to open %s, no update generator ?\n", s);
	}
	sprintf(s, "../Log/%d/queryLog", t);
	FILE *query = fopen(s, "r");
	if (!query) {
		printf("Fail to open %s\n", s);
		return 0;
	}
	sprintf(s, "../Log/%d/baseline.in", t);
	FILE *result = fopen(s, "w");
	if (!result) {
		printf("Fail to open %s\n", s);
		return 0;
	}

	LogInfo U, Q;
	while (parseLog(update, &U, SENDUPDATE) != END) {
		if (value.size() <= U.No)
			value.resize(U.No+1, 0.0);
		value[U.No] = U.w;
	}
	if (update)
		rewind(update);

	while (parseLog(query, &Q, FINISHQUERY) != END) {
		if (sentQuery.size() <= Q.No)
			sentQuery.resize(Q.No+1, 0);
		sentQuery[Q.No] = 1;
	}
	if (query)
		rewind(query);

	parseLog(update, &U, GENUPDATE);
	parseLog(query, &Q, GENQUERY);
	while (U.job != END && Q.job != END) {
		if (timediff(Q.t, U.t) > 0) {
			if (sentQuery.size() > Q.No && sentQuery[Q.No]) {
				fprintf(result, "0 %d %d %d %d %d\n", Q.No, Q.A.groupId, Q.A.nodeId, Q.B.groupId, Q.B.nodeId);
			}
			parseLog(query, &Q, GENQUERY);
		} else {
			if (value.size() > U.No && value[U.No] != 0.0) {
				fprintf(result, "1 %d %d %d %d %lf\n", U.A.groupId, U.A.nodeId, U.B.groupId, U.B.nodeId, value[U.No]);
			}
			parseLog(update, &U, GENUPDATE);
		}
	}
	while (Q.job != END) {
		if (sentQuery.size() > Q.No && sentQuery[Q.No]) {
			fprintf(result, "0 %d %d %d %d %d\n", Q.No, Q.A.groupId, Q.A.nodeId, Q.B.groupId, Q.B.nodeId);
		}
		parseLog(query, &Q, GENQUERY);
	}
	while (U.job != END) {
		if (value.size() > U.No && value[U.No] != 0.0) {
			fprintf(result, "1 %d %d %d %d %lf\n", U.A.groupId, U.A.nodeId, U.B.groupId, U.B.nodeId, value[U.No]);
		}
		parseLog(update, &U, GENUPDATE);
	}
	if (update)
		fclose(update);
	fclose(query);
	fclose(result);
	return 0;
}
