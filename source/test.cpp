#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mpi.h>
void * sendQueryResult(void *ptr) {
	MPI_Status status;
	double result;
	while (1) {
		sleep(1);
		if (MPI_SUCCESS != MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD))
			fprintf(stderr, "SEND error \n");

		fprintf(stderr, "SEND\n");
	}
}
void * recvQueryResult(void *ptr) {
	MPI_Status status;
	double result;
	sleep(20);
	while (1) {
		MPI_Recv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		fprintf(stderr, "RECV\n");
	}
}
int main(int args, char *argv[]) {
	MPI_Init(&args, &argv);
	pthread_t sendThread, recvThread;
	pthread_create(&sendThread, NULL, sendQueryResult, NULL);
	pthread_create(&recvThread, NULL, recvQueryResult, NULL);
	while (1) {
		
	}
	MPI_Finalize();
	return 0;
}
