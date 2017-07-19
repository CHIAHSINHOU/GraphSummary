#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "writeLog.h"

enum ParseResult {
	GENQUERY	= 0x1,
	SENDQUERY	= 0x2,
	FINISHQUERY	= 0x4,
	GENUPDATE	= 0x8,
	SENDUPDATE	= 0x10,
	END			= 0x20
};

struct LogInfo {
	ParseResult job;
	ID A;
	ID B;
	double w;
	int No;
	timeval t;
};


ID parseId(FILE *f);
timeval parseTime(FILE *f);
ParseResult parseLog(FILE *f, LogInfo *log);
ParseResult parseLog(FILE *f, LogInfo *log, ParseResult flag);
