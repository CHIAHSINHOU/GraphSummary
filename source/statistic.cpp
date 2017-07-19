#include <stdio.h>
#include <map>
#include <cmath>
using namespace std;
map<int, double> Ans;
map<int, double> Time;
int main(){
	

	int testCase[3] = {2001, 2004, 2005};
	int change[3] = {1, 15, 30};
	int errCount[12];

	char in[1000];
	char out[1000];
	FILE *fp, *fout;
	int n;
	double ans, t;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			for(int k=0;k<12;k++)
				errCount[k] = 0;
			sprintf(in, "../Log/updateCache/%d/%dresult", testCase[i], change[j]);
			fp = fopen(in, "r");
			if(fp == NULL){
				fprintf(stderr, "cannot open %s\n", in);
				return -1;
			}
			double total_time=0.0;
			double count=0;
			while(fscanf(fp, "%d %lf %lf", &n, &ans, &t) != EOF){
				Ans[n] = ans;
				Time[n] = t;
				total_time += t;
				count++;
			}
			fprintf(stderr, "%lf\n", total_time/count);
			fclose(fp);
			sprintf(in, "../Log/updateCache/%d/%dresult_baseline", testCase[i], change[j]);
			fp = fopen(in, "r");
			if(fp == NULL){
				fprintf(stderr, "cannot open %s\n", in);
				return -1;
			}

			//fout = fopen(out, "w");
			double total_tdiff = 0.0;
			count = 0;
			total_time = 0.0;
			while(fscanf(fp, "%d %lf %lf", &n, &ans, &t) != EOF){
				if(Ans.find(n) != Ans.end() 
					&& Time.find(n) != Time.end()){
					if(Ans[n] < 0)
						continue;
					double err = fabs(Ans[n] - ans)/ans;
					//fprintf(fout, "%d %lf\n", n, err);
					double tmp = err*10;
					if(err == 0)
						errCount[0]++;
					else if(tmp < 10)
						errCount[(int)tmp+1]++;
					else
						errCount[11]++;
					total_time += t;
					double tdiff = t/Time[n];
					total_tdiff += tdiff;
					count ++;
				}
			}

			double tmp=0;
			for(int k=0;k<11;k++){
				tmp += (double)errCount[k]/count;
				fprintf(stdout, "%lf\t", tmp);
			}
			fprintf(stdout, "\n");
			fclose(fp);
			fprintf(stderr, "%d %d %lf\n", testCase[i], change[j], total_tdiff/count);
			//fclose(fout);
		}
		
	}
	return 0;
}
