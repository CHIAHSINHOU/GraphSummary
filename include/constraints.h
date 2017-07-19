#ifndef constraints_h
#define constraints_h

struct Constraint_Data{
	long long updateTime;
	int source;
	Constraint_Data() {}
	Constraint_Data(long long u, int s){
		updateTime = u;
		source = s;
	}

};

struct Update_Data{
	int edgeNum;
	int *edges;
	double *values;
	Update_Data() {}
	Update_Data(int n, int *_edges, double *_values){
		edgeNum = n;
		edges = _edges;
		values = _values;
	}

};

struct Source_Data{
	int S[2];
	int groupId;
	int source;

	Source_Data() {}
	Source_Data(int *s, int g, int _s) {
		S[0] = s[0];
		S[1] = s[1];
		groupId = g;
		source = _s;

	}
};

struct Target_Data
{
	int T[2];
	int groupId;
	int source;
	Target_Data() {}
	Target_Data(int *t, int g, int _s) {
		T[0] = t[0];
		T[1] = t[1];
		groupId = g;
		source = _s;

	}

};

struct Path_Data
{
	int source;
	int *path;
	int size;
	Path_Data() {}
	Path_Data(int * _path, int _s, int _size){
		path = _path;
		source = _s;
		size = _size;
	}
};

struct ST_Data
{
	int source;
	int groupId;
	int S, T;
	ST_Data() {}
	ST_Data(int * _st, int _g, int _s){
		S = _st[1];
		T = _st[2];
		source = _s;
		groupId = _g;
	}

};


void calculateConstraints(char *t, int* groupId, int groupNum, int tn);

#endif