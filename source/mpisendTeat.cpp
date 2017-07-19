#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string>       // std::string
#include <iostream>     // std::cout, std::ostream, std::hex
#include <sstream>      // std::stringbuf



#define MPI_ERROR(ret, call, ...)\
	ret = call(__VA_ARGS__);\
	if(ret != MPI_SUCCESS){\
		fprintf(stderr, "%s error at %d\n", #call, __LINE__);\
		exit(-1);\
	}


int byteArrayToInt(char *b) 
{
    int value = 0;
    for (int i = 0; i < 4; i++) {
        int shift = i * 8;
        value += (b[i] & 0xFF) << shift;
    }
    return value;
}

char* intToByteArray(int *a, int len)
{
    char* ret = new char[4*len+1];
    for(int i=0;i<len;i++){
	    ret[i*4+0] = (char) (a[i] & 0xFF);   
	    ret[i*4+1] = (char) ((a[i] >> 8) & 0xFF);   
	    ret[i*4+2] = (char) ((a[i] >> 16) & 0xFF);   
	    ret[i*4+3] = (char) ((a[i] >> 24) & 0xFF);
	}
    return ret;
}

int main(int argc,char** argv) {

    int size, rank;
    int ret;
    std::stringbuf buffer;
    std::ostream os (&buffer);      // associate stream buffer to stream

    int lentag = 0;
    int datatag = 1;


    MPI_ERROR(ret,MPI_Init, &argc, &argv);
    MPI_ERROR(ret,MPI_Comm_size,MPI_COMM_WORLD, &size);
    MPI_ERROR(ret,MPI_Comm_rank,MPI_COMM_WORLD, &rank);
    if(rank == 0){
    	for(int i=0;i<3;i++){

    		os << "I am " << rank << " ";
	    	int *nums;
	    	int len = (int)rand()%5;
	    	nums = (int *)malloc(sizeof(int)*len);
	    	for(int j=0;j<len;j++){
	    		nums[j] = rand()%10;
	    		os << nums[j] << " ";
	    	}
	    	os << std::endl;
	    	std::cout << buffer.str();
	    	char *data = intToByteArray(nums, len);
	    	MPI_ERROR(ret, MPI_Send, &len, 1, MPI_INT, 1, lentag, MPI_COMM_WORLD );
	    	MPI_ERROR(ret, MPI_Send, nums, len*sizeof(int), MPI_BYTE, 1, datatag, MPI_COMM_WORLD );
    	}
    }

    else if(rank == 1){

    	for(int i=0;i<3;i++){
	    	int len;
	        MPI_ERROR(ret, MPI_Recv, &len, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	        char data[len*sizeof(int)+1];
	        MPI_ERROR(ret, MPI_Recv, data, len*sizeof(int), MPI_BYTE, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	        data[len*sizeof(int)] = '\0';
	        //printf("%d%d%d%d", data[0], data[1], data[2],data[3]);
	        os << "I am " << rank << " ";
	        for(int j=0;j<len;j++){
	        	os << byteArrayToInt(&data[j*4]) << " ";
	        }
	        os << std::endl;
	    	std::cout << buffer.str();
    	}

    }

    MPI_ERROR(ret,MPI_Finalize);

}