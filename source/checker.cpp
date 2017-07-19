#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <map>

using namespace std;

const int inf = 1 << 28;
map<int, double> ans;
int main(int args, char *argv[]){

	if (args < 2) {
	    fprintf(stderr, "Too few args ./%s ans output\n", argv[0]);
	    return 0;
  	}

  	int queryNum=0, id;
    double value, tmp;
  	FILE *ans_fp = fopen(argv[1], "r");
  	FILE *output_fp = fopen(argv[2], "r");


  	while(fscanf(ans_fp, "%d%lf%lf", &id, &value, &tmp) == 3){
        printf("%d %lf\n", id, value);
        ans[id] = value;
        queryNum++;
  	}

  	double error_rate = 0;
  	int error_num = 0;
  	int cannot_go = 0;
  	while(fscanf(output_fp, "%d%lf%lf", &id, &value, &tmp) == 3){
      //printf("%d %lf\n", id, value);
      if(ans.find(id) == ans.end()){
        printf("cannot find\n");
        continue;
      }

  		if (ans[id] != value){
  			if(value != inf)
  				error_rate += fabs((double)value - ans[id]) / ans[id];
  			else
  				cannot_go ++;
  			error_num ++;
        printf("%d %lf %lf\n", id, ans[id], value);
  		}
  	}

  	printf("error_rate : %lf\n", error_rate/(queryNum-cannot_go));
  	fprintf(stderr, " %lf", error_rate/(queryNum-cannot_go));
  	printf("cannot_go : %d\n", cannot_go);
  	printf("error_num : %d\n", error_num);


	return 0;
}