#include <stdio.h>
#include <iostream>

using namespace std;

int main(){
	int dimension = 16;
	char fileName[2][100] = {"male.txt","female.txt"};
	char nfileName[2][100] = {"Male.txt","Female.txt"};
	for(int i=0;i<2;++i){
		FILE *f = fopen(fileName[i],"r");
		FILE *newF = fopen(nfileName[i],"w");

		double data[dimension];
		while(fscanf(f,"%*s")!=EOF){
			for(int d=0;d<dimension;++d) fscanf(f,"%lf",&data[d]);
			for(int d=0;d<dimension;++d) fprintf(newF,"%f ",data[d]);
			fprintf(newF, "\n");
		}
	}
}



