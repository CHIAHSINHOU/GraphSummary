#include "parseLog.h"

ID parseId(FILE *f) {
	int A;
	int B;
	fscanf(f, " (%d,%d)", &A, &B);
	return ID(A,B);
}

timeval parseTime(FILE *f) {
	char buf[30];
	timeval now;
	struct tm Tm;
	double usec;
	fscanf(f, "%s %lf", buf, &usec);
	strptime(buf, "%F-%T", &Tm);
	now.tv_sec = mktime(&Tm);
	now.tv_usec = usec;
	return now;
}

ParseResult parseLog(FILE *f, LogInfo *log, ParseResult flag) {
	ParseResult job; 
	while ((job = parseLog(f,log)) != END) {
		if (job == flag) {
			return job;
		}
	}
	return job;
}
ParseResult parseLog(FILE *f, LogInfo *log) {
	char job[20];
	char s[100];

	if (!f)
		return log->job = END;

	ParseResult result;
	while (1) {
		if (fscanf(f, "%s", job) == EOF) {
			return log->job = END;
		}
		if (!strcmp(job, "GENUPDATE")) {
			result = GENUPDATE;
			fscanf(f, "%d", &(log->No));
			log->A = parseId(f);
			log->B = parseId(f);
			log->t = parseTime(f);
			return log->job = GENUPDATE;
		} else if (!strcmp(job, "SENDUPDATE")) {
			result = SENDUPDATE;
			fscanf(f, "%d", &(log->No));
			log->A = parseId(f);
			log->B = parseId(f);
			fscanf(f, "%lf", &(log->w));
			log->t = parseTime(f);
			return log->job = SENDUPDATE;
		} else if (!strcmp(job, "GENQUERY")) {
			result = GENQUERY;
			fscanf(f, "%d", &(log->No));
			log->A = parseId(f);
			log->B = parseId(f);
			log->t = parseTime(f);
			return log->job = GENQUERY;
		} else if (!strcmp(job, "SENDQUERY")) {
			result = SENDQUERY;
			fscanf(f, "%d", &(log->No));
			log->A = parseId(f);
			log->B = parseId(f);
			log->t = parseTime(f);
			return log->job = SENDQUERY;
		} else if (!strcmp(job, "FINISHQUERY")) {
			result = FINISHQUERY;
			fscanf(f, "%d", &(log->No));
			log->A = parseId(f);
			log->B = parseId(f);
			fscanf(f, "%lf", &(log->w));
			log->t = parseTime(f);
			return log->job = FINISHQUERY;
		} else {
			printf("%s ", job);
			fgets(s, 100, f);
			printf("%s\n", s);
		}
	}
}


