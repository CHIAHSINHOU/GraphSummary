#include "receiveQuery.h"
#include "procInfo.h"
#include "mpi.h"
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <queue>
#include <map>
#include "job_type.h"
#include <string.h>

using namespace std;


#define SYSCALL_ERROR(ret, call, ...)\
	ret = call(__VA_ARGS__);\
	if(ret < 0){\
		fprintf(stderr, "%s error at %d\n", #call, __LINE__);\
		exit(-1);\
	}

struct QueryJob{
	int a[2];
	int b[2];
	int procId;
};

struct  UpdateJob
{
	vector< vector<int> > updateEdges;
	vector< vector<double> > updateValues;
	vector<int> queryEdges;
	vector<double> queryValues;
};

struct AnsData{
	double ans;
	int *path;
	int size;
	int connfd;
};
struct sockaddr_in serv_addr; 
pthread_mutex_t jobMutex;


queue<QueryJob> queryJobs;
queue<int> freeQueryProc;
map<int, int> connfdMapping;
queue<int> connfdJobs;
queue<int> connfdUpdateJobs;
queue<AnsData> answers;
queue<UpdateJob> updateQueue;
int listenfd;
map< pair<int, int> , int> oriID;
map<int, pair<int, int> > newID;
void readID(char *s){
	int a,b,c;
	FILE *fp;
	char fileName[100];
	sprintf(fileName,"%s/oriId", s);
	fp = fopen(fileName, "r");
	while(fscanf(fp, "%d%d%d", &a, &b, &c)==3){
		printf("%d %d %d\n", a, b, c);
		newID[a]=make_pair(b,c);
		oriID[make_pair(b,c)] = a;
	}
}
void getReady(void){

	MPI_Status status;
	int ready;
	for(int i=0;i<updateProc.num;i++){
		int procId = updateProc.start+i;
		MPI_Recv(&ready, 1, MPI_INT, procId, 0, MPI_COMM_WORLD, &status);
	}

	for(int i=0;i<queryProc.num;i++){
		int procId = queryProc.start+i;
		MPI_Recv(&ready, 1, MPI_INT, procId, 0, MPI_COMM_WORLD, &status);
	}

	fprintf(stderr, "shortest path server ready\n");
}

void initSocket(int portNum){

	struct sockaddr_in serv_addr; 
	int ret;
	SYSCALL_ERROR(listenfd, socket, AF_INET, SOCK_STREAM, IPPROTO_IP);

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portNum); 

	SYSCALL_ERROR(ret, bind, listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	SYSCALL_ERROR(ret, listen, listenfd, 0);

}

void initFreeQueryProc(){
	for(int i=0;i<queryProc.num;i++){
		int pid = queryProc.start+i;
		freeQueryProc.push(pid);
	}
}
int getAvailableQueryProc(){

	int procId=-1;
	while(procId==-1){
		pthread_mutex_lock(&jobMutex);
		if(freeQueryProc.empty()){
			pthread_mutex_unlock(&jobMutex);
		}
		else{
	 		procId = freeQueryProc.front();
			freeQueryProc.pop();
			pthread_mutex_unlock(&jobMutex);
		}
	}
	return procId;
}

int AddFreeQueryProc(int procId){
	pthread_mutex_lock(&jobMutex);
	freeQueryProc.push(procId);
	pthread_mutex_unlock(&jobMutex);
}

int getJobs(int type){
	int connfd = -1;
	while(connfd==-1){
		pthread_mutex_lock(&jobMutex);
		if(type == 0){
			if(connfdJobs.empty())
				pthread_mutex_unlock(&jobMutex);
			else{
				connfd = connfdJobs.front();
				connfdJobs.pop();
				pthread_mutex_unlock(&jobMutex);
			}
		}
		else{
			if(connfdUpdateJobs.empty())
				pthread_mutex_unlock(&jobMutex);
			else{
				connfd = connfdUpdateJobs.front();
				connfdUpdateJobs.pop();
				pthread_mutex_unlock(&jobMutex);
			}

		}
	}
	return connfd;
}

void registerConnfd(int procId, int connfd, QueryJob job){
	pthread_mutex_lock(&jobMutex);
	connfdMapping[procId] = connfd;
	job.procId = procId;
	queryJobs.push(job);
	fprintf(stderr, "(%d, %d)-> (%d, %d)\n", job.a[0], job.a[1],  job.b[0], job.b[1]);
	pthread_mutex_unlock(&jobMutex);

}
AnsData getAns(){

	AnsData ans;
	ans.ans = -1;
	while(ans.ans == -1){
		pthread_mutex_lock(&jobMutex);
		if(answers.empty())
			pthread_mutex_unlock(&jobMutex);
		else{
			ans = answers.front();
			answers.pop();
			pthread_mutex_unlock(&jobMutex);	
		}

	}
	return ans;
}

void *sendUpdate(void *ptr){
	char buf[10000];
	int rval;
	UpdateJob job;
	int jobType = UPDATE_GRAPH;
	int n;
	int a,b,c,d, x, y;
	double f;
	while(1){
		int connfdJob = getJobs(1);
		rval = read(connfdJob,&n,sizeof(int));
		fprintf(stderr, "update %d %d\n", connfdJob, n);
		job.updateEdges.resize(updateProc.num);
		job.updateValues.resize(updateProc.num);
		for(int i=0;i<updateProc.num;i++){
			job.updateEdges[i].clear();
			job.updateValues[i].clear();
		}
		job.queryEdges.clear();
		job.queryValues.clear();
		rval = read(connfdJob,buf,sizeof(buf));
		buf[rval] = '\0';
		int tmp, already=0;
		char * pch;
		pch = strtok (buf," ");
		while(n--){
			sscanf(pch, "%d", &x);
			pch = strtok (NULL, " ");
			sscanf(pch, "%d", &y);
			pch = strtok (NULL, " ");
			sscanf(pch, "%lf", &f);
			pch = strtok (NULL, " ");
			// tmp = sscanf(buf+already, "%d%d%lf", &x, &y, &f);
			printf("%d %d %lf\n", x, y, f);
			// already += tmp;
			a = newID[x].first;
			b = newID[x].second;
			c = newID[y].first;
			d = newID[y].second;
			if(a==c){
				job.updateEdges[graphInfo[a]-updateProc.start].push_back(a);
				job.updateEdges[graphInfo[a]-updateProc.start].push_back(b);
				job.updateEdges[graphInfo[a]-updateProc.start].push_back(c);
				job.updateEdges[graphInfo[a]-updateProc.start].push_back(d);
				job.updateValues[graphInfo[a]-updateProc.start].push_back(f);
			}
			else{
				job.queryEdges.push_back(a);
				job.queryEdges.push_back(b);
				job.queryEdges.push_back(c);
				job.queryEdges.push_back(d);
				job.queryValues.push_back(f);
			}

		}
		pthread_mutex_lock(&jobMutex);
		updateQueue.push(job);
		pthread_mutex_unlock(&jobMutex);
		close(connfdJob);
	}
	

}
void *sendQuery(void *ptr){

	char buf[10000];
	int rval;
	QueryJob job;
	int x, y;
	int jobType = ANSWER_QUERY;
	while(1){
		int procId = getAvailableQueryProc();
		fprintf(stderr, "procId %d\n", procId);
		int connfdJob = getJobs(0);
		fprintf(stderr, "query connfd %d\n", connfdJob);
		memset(buf, sizeof(buf), 0);
		rval = read(connfdJob,buf,sizeof(buf));
		buf[rval]='\0';
		sscanf(buf, "%d%d", &x, &y);
		printf("%d %d\n", x, y);
		if(newID.find(x)==newID.end())
			continue;
		if(newID.find(y)==newID.end())
			continue;
		job.a[0] = newID[x].first;
		job.a[1] = newID[x].second;
		job.b[0] = newID[y].first;
		job.b[1] = newID[y].second;
		// sscanf(buf, "%d%d%d%d", &job.a[0], &job.a[1], &job.b[0], &job.b[1]);
		registerConnfd(procId, connfdJob, job);
	}

}


void *sendAns(void *ptr){
	char buf[10000];
	memset(buf, '\0', sizeof(buf));
	int wrval;
	while(1){
		AnsData ans = getAns();
		int count = 0;
		// int count = sprintf(buf, "%lf\n", ans.ans);
		int tmp = sprintf(buf, "%d ", oriID[make_pair(ans.path[0], ans.path[1])]);
		count += tmp;
		for(int i=2;i<ans.size;i+=2){
			int tmp = sprintf(buf+count, "%d ", oriID[make_pair(ans.path[i], ans.path[i+1])]);
			count += tmp;
		}
		wrval=write(ans.connfd,buf,sizeof(buf));
		close(ans.connfd);
	}
}

void *linstenConnection(void *ptr){
	struct sockaddr_in client;
	int c;
	int client_sock;
	int type;
	while(1){
		client_sock = accept(listenfd, (struct sockaddr *)&client, (socklen_t*)&c);
		read(client_sock,&type,sizeof(int));
		fprintf(stderr, "accept! %d %d\n", client_sock, type);
		pthread_mutex_lock(&jobMutex);
		if(type == 0)
			connfdJobs.push(client_sock);
		else
			connfdUpdateJobs.push(client_sock);
		pthread_mutex_unlock(&jobMutex);
	}
}

void recvQueryResult(int procId, double result){
	MPI_Status status;
	fprintf(stderr, "get ansewer %lf from %d\n", result, procId);
	int size;
	MPI_Recv(&size, 1, MPI_INT, procId, 0, MPI_COMM_WORLD, &status);
	int *path = new int[size];
	MPI_Recv(path, size, MPI_INT, procId, 0, MPI_COMM_WORLD, &status);
	AnsData ans;
	ans.ans = result;
	ans.connfd = connfdMapping[procId];
	ans.size = size;
	ans.path = path;
	pthread_mutex_lock(&jobMutex);
	freeQueryProc.push(procId);
	answers.push(ans);
	pthread_mutex_unlock(&jobMutex);

}
void receiveQuery(int rank, int portNum, char *s){

	getReady();
	readID(s);
	initFreeQueryProc();
	initSocket(portNum);

	MPI_Request recvReq;
	MPI_Status status;
	int flag = 0;
	int recving = 0;
	double result;

	int threadNum = 7;

	pthread_mutex_init(&jobMutex, NULL);
	//pthread_mutex_init(&ans_mutex, NULL);
	//pthread_barrier_init(&barr, NULL, (unsigned) threadNum);
	pthread_t* threads = (pthread_t*)malloc(threadNum * sizeof(pthread_t));
	pthread_create(&threads[0], NULL, linstenConnection, NULL);
	pthread_create(&threads[1], NULL, sendAns, NULL);
	pthread_create(&threads[2], NULL, sendAns, NULL);
	pthread_create(&threads[3], NULL, sendQuery, NULL);
	pthread_create(&threads[4], NULL, sendQuery, NULL);
	pthread_create(&threads[5], NULL, sendUpdate, NULL);
	pthread_create(&threads[6], NULL, sendUpdate, NULL);

	int jobType = ANSWER_QUERY;
	while(1){
		pthread_mutex_lock(&jobMutex);
		while(!queryJobs.empty()){
			QueryJob job = queryJobs.front();
			MPI_Send(&jobType, 1, MPI_INT, job.procId, 1, MPI_COMM_WORLD);
			MPI_Send(job.a, 2, MPI_INT, job.procId, 0, MPI_COMM_WORLD);
			MPI_Send(job.b, 2, MPI_INT, job.procId, 2, MPI_COMM_WORLD);
			queryJobs.pop();
		}
		pthread_mutex_unlock(&jobMutex);
		pthread_mutex_lock(&jobMutex);
		while(!updateQueue.empty()){
			UpdateJob job = updateQueue.front();
			for(int i=0;i<updateProc.num;i++){
				if(job.updateEdges[i].size()!=0){
					int target = updateProc.start + i;
					int jobType = UPDATE_GRAPH;
					MPI_Send(&jobType, 1, MPI_INT, target, 1, MPI_COMM_WORLD);
					int edgeNum = job.updateEdges[i].size()/4;
					fprintf(stderr, "receiver server update %d %d\n", target, edgeNum);
					MPI_Send(&edgeNum, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
					MPI_Send(job.updateEdges[i].data(), edgeNum*4, MPI_INT, target, 0, MPI_COMM_WORLD);
					MPI_Send(job.updateValues[i].data(), edgeNum, MPI_DOUBLE, target, 0, MPI_COMM_WORLD);
				}
			}
			for (int i = 0 ; i < queryProc.num; i++) {
				if (job.queryEdges.size() == 0) {
					continue;
				}
				int target = queryProc.start + i;
				int jobType = UPDATE_GRAPH;
				MPI_Send(&jobType, 1, MPI_INT, target, 1, MPI_COMM_WORLD);
				int edgeNum = job.queryEdges.size()/4;
				fprintf(stderr, "receiver server update %d %d\n", target, edgeNum);
				MPI_Send(&edgeNum, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
				MPI_Send(job.queryEdges.data(), edgeNum*4, MPI_INT, target, 0, MPI_COMM_WORLD);
				MPI_Send(job.queryValues.data(), edgeNum, MPI_DOUBLE, target, 0, MPI_COMM_WORLD);
			}

			updateQueue.pop();
		}
		pthread_mutex_unlock(&jobMutex);
		if (!recving) {
			MPI_Irecv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recvReq);
		}
		MPI_Test(&recvReq, &flag, &status);
		if (!flag) {
			recving = 1;
		} else {
			recvQueryResult(status.MPI_SOURCE, result);
			recving = 0;
		}
	}
}
