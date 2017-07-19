#include "parseLog.h"
#include <vector>
using namespace std;

vector<timeval> start;
vector<timeval> end;
vector<double> value;

int main(int args, char *argv[])
{
	if (args != 2) {
		printf("need testcase #\n");
		return 0;
	}
	int t;
	sscanf(argv[1], "%d", &t);
	char s[100];
	sprintf(s, "../Log/%d/queryLog", t);
	FILE *queryLog = fopen(s, "r");
	if (!queryLog) {
		printf("Fail to open %s\n", s);
		return 0;
	}
	sprintf(s, "../Log/%d/resultFile", t);
	FILE *result = fopen(s, "w");
	if (!result) {
		printf("Fail to open %s\n", s);
		return 0;
	}
	LogInfo log;
	timeval now;
	gettimeofday(&now, NULL);
	while (parseLog(queryLog, &log) != END) {
		if (log.job == GENQUERY) {
			if (log.No >= start.size()) {
				start.resize(log.No + 100, now);
				end.resize(log.No + 100, now);
				value.resize(log.No + 100, 0.0);
			}
			start[log.No] = log.t;
		} else if (log.job == FINISHQUERY) {
			end[log.No] = log.t;
			value[log.No] = log.w;
		}
	}
	for (int i = 0; i < start.size(); i++) {
		if (timediff(end[i], now) != 0) {
			FLOG(result, "%d %lf %lf\n", i, value[i], timediff(start[i], end[i]));
		}
	}
	fclose(queryLog);
	return 0;
}

