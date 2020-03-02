#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <set>
# define N_Vertex 12500
# define N_VN     12500
# define N_FACE   25000
# define WaistBasePoint 7294
# define DisSize 1000
# define NONE -1
# define WaistArea 0.025
using namespace std;

struct Vertex{
	double x,y,z;
	int id;

	bool operator < (Vertex right){
		return y<right.y;
	}
};

int main(){
	FILE *fp;

	//fp = fopen("SPRING_MALE/SPRING4765.obj", "r");
	fp = fopen("mesh/SPRING4702.obj", "r"); //1.16
	if(fp==NULL){
		cout<<"Cannot open the obj file.\n";
		return 0;
	}

	//Load vertex positoin
	Vertex v[N_Vertex];
	Vertex avg = {0,0,0};
	for(int i=0;i<N_Vertex;++i){
		fscanf(fp,"%*s %lf %lf %lf",&v[i].x, &v[i].y, &v[i].z);
		v[i].id=i;
		
		avg.x+=v[i].x;
		avg.y+=v[i].y;
		avg.z+=v[i].z;
	}
	avg.x/=N_Vertex;
	avg.y/=N_Vertex;
	avg.z/=N_Vertex;

	for(int i=0;i<N_Vertex;++i){
		v[i].x-=avg.x;
		v[i].y-=avg.y;
		v[i].z-=avg.z;

		//rotate around y axis for -45 degrees
		double newX = 1/sqrt(2)*v[i].x - 1/sqrt(2)*v[i].y;
		double newY = 1/sqrt(2)*v[i].x + 1/sqrt(2)*v[i].y;
		double newZ = v[i].z;
		v[i].x = newX;
		v[i].y = newY;
		v[i].z = newZ;
	}


	FILE *rotate = fopen("Rotate.obj","w");
	if(fp==NULL){
		cout<<"Cannot open the Rotate.obj file.\n";
		return 0;
	}
	for(int i=0;i<N_Vertex;++i)
		fprintf(rotate,"v %f %f %f\n",v[i].x, v[i].y, v[i].z );
}