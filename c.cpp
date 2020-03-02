#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <set>
#include "Hip.h"
#include "Thigh.h"
#include "Calf.h"
#include "Chest.h"
#include "Vertex.h"
# define N_Vertex 12500
# define N_VN     12500
# define N_FACE   25000
# define WaistBasePoint 7294
# define DisSize 1500
# define NONE -1
# define WaistArea 0.03
# define  N_HeadVertex 10
using namespace std;



//Get the vertices away from Vertex 7294 are closer than 2cm
vector<int> GetWaistVertex(Vertex v[], int Map[],int MapOri[]){
	double high = v[WaistBasePoint].z+WaistArea; // 0.02m
	double low  = v[WaistBasePoint].z-WaistArea;
	vector<int> waistIndex;
	for(int i=0;i<N_Vertex;++i)
		if(v[i].z < high && v[i].z > low)
			Map[i]=waistIndex.size(), MapOri[waistIndex.size()]=i, waistIndex.push_back(i);
	return waistIndex;
}

void InitWaistDistance(int waistMap2Vec[], Vertex v[], double waistDis[][DisSize], 
	int index1, int index2, int index3){
	if(waistMap2Vec[index1]!=NONE && waistMap2Vec[index2]!=NONE && waistMap2Vec[index3]!=NONE){
		//cut the circle to be a plane
		float vertexX[3]={v[index1].x, v[index2].x, v[index3].x};
		sort(vertexX,vertexX+3);
		float pivot = v[WaistBasePoint].x;
		if( (vertexX[0]<pivot && vertexX[1]>pivot) || (vertexX[1]<pivot && vertexX[2]>pivot) 
			|| (vertexX[0]<pivot && vertexX[2]>pivot) ){
			//Don't cut triangles in the back of waist.
			//Suppose the thickness of front and back waist exceeds 0.05m.
			if(v[WaistBasePoint].y-0.05< v[index1].y && v[index1].y<v[WaistBasePoint].y+0.05)
				return;
		}
			

		double d12 = sqrt(pow(v[index1].x-v[index2].x,2) + pow(v[index1].y-v[index2].y,2));
		double d13 = sqrt(pow(v[index1].x-v[index3].x,2) + pow(v[index1].y-v[index3].y,2));
		double d23 = sqrt(pow(v[index2].x-v[index3].x,2) + pow(v[index2].y-v[index3].y,2));
		int v1=waistMap2Vec[index1], v2=waistMap2Vec[index2], v3=waistMap2Vec[index3];
		waistDis[v1][v2] = waistDis[v2][v1] = d12;
		waistDis[v1][v3] = waistDis[v3][v1] = d13;
		waistDis[v2][v3] = waistDis[v3][v2] = d23;
	}
}

// Disconnect the left path of the waistPoint. Make the waist path circle the body.
void WaistDisconnectLeft(double waistDis[][DisSize], int waistMap2Vec[],
						 int waistMap2_Ori[], Vertex v[],int size){
	int waistIdInVec = waistMap2Vec[WaistBasePoint];
	double waistX = v[WaistBasePoint].x;
	for(int i=0;i<size;++i){ // Find the vertices connected with WaistBasePoint
		int OriId = waistMap2_Ori[i];
		if(waistDis[waistIdInVec][i]!=0){
			//cout<<"1\n";
			//cout<<v[OriId].x<<" "<<waistX <<endl;
			if(v[OriId].x<waistX ) // the x value is smaller than the WaistBasePoint
				{waistDis[waistIdInVec][i] = 0;//Disconnect.
					//cout<<"X\n";
				}
		}
	}
}

double WaistDijkstra(double waistDis[][DisSize], int startId, int size,Vertex v[], int waistMap2_Ori[]){
	double d[size]; fill_n(d,size,1e6);
	bool visit[size]={};

	int parent[size]; fill_n(parent,size,-1);
	parent[startId]=startId;
	d[startId] = 0; 
	for(int i=0;i<size;++i){
		double min=1e6, nearBaseZ = 10;
		int id;
		for(int j=0;j<size;++j)
			if(visit[j] == false && d[j]!=1e6){
				int OriId =  waistMap2_Ori[j];//Use the vertices whose z value is closer to the z of BasePoint 
				double diff = v[OriId].z - v[WaistBasePoint].z;
				if(diff<0)diff=-diff;
				if(diff<nearBaseZ)
					min = d[j], id = j, nearBaseZ = diff;
			}
		//cout<<i<<" "<<id<<" "<<min<<" "<<endl;
		if(min==1e6)break;//all the vertices connected to start point have been used

		visit[id] = true;
		for(int j=0;j<size;++j)
			if(visit[j]==false && waistDis[id][j]!=0 && d[id]+waistDis[id][j]<d[j]){
				d[j] = d[id]+waistDis[id][j];
				parent[j] = id;
				
			}
	}

	double longest = 0;
	int last;
	for(int i=0;i<size;++i)
		if(d[i]!=1e6 && d[i]>longest)
			longest = d[i],last=i;

	//Debug： Output the path
	FILE *waistPath= fopen("WaistPath.obj", "w");
	if(waistPath==NULL){
		cout<<"Cannot create WaistPath.obj\n";
		return longest*2;
	}
	int now = last, all=1;
	while(now!=startId){
		Vertex p = v[waistMap2_Ori[now]];
		fprintf(waistPath,"v %f %f %f\n",p.x,p.y,p.z);
		now = parent[now];
		all++;
	}
	Vertex p = v[waistMap2_Ori[startId]];
	fprintf(waistPath,"v %f %f %f\n",p.x,p.y,p.z);
	fprintf(waistPath,"l");
	for(int i=1;i<=all;++i)fprintf(waistPath," %d",i);
	fprintf(waistPath," 1");//Connect to the last point. Make it a complete circle.
	fclose(waistPath);
	////////////////////////////

	return longest;
}


float GetHight(double Max, double Min){
	return Max - Min;
}

void WriteNewObj(Vertex v[],int face[][3]){
	FILE *NewOBJ = fopen("NewOBJ.obj","w");
	if(NewOBJ==NULL){
		cout<<"Cannot open NewOBJ.obj\n";
		return;
	}
	for(int i=0;i<N_Vertex;++i)
		fprintf(NewOBJ,"v %f %f %f\n",v[i].x, v[i].y, v[i].z);
	for(int i=0;i<N_FACE;++i)
		fprintf(NewOBJ,"f %d %d %d\n",face[i][0]+1, face[i][1]+1, face[i][2]+1);
}

double waistDis[DisSize][DisSize]={};
int main(){
	FILE *fp;

	fp = fopen("SPRING_MALE/SPRING4739.obj", "r");
	//fp = fopen("mesh/SPRING4702.obj", "r"); //1.16
	if(fp==NULL){
		cout<<"Cannot open the obj file.\n";
		return 0;
	}


	//Load vertex positoin
	Vertex v[N_Vertex];
	Vertex avg = {0,0,0};
	double Max=-10, Min=10;
	for(int i=0;i<N_Vertex;++i){
		fscanf(fp,"%*s %lf %lf %lf",&v[i].x, &v[i].y, &v[i].z);
		avg.x+=v[i].x; avg.y+=v[i].y; avg.z+=v[i].z;
		if(v[i].z<Min)Min=v[i].z;
		if(v[i].z>Max)Max=v[i].z;
		//printf("%f %f %f\n",v[i].x, v[i].y, v[i].z);
	}
	avg.x/=N_Vertex; avg.y/=N_Vertex; avg.z/=N_Vertex;
	for(int i=0;i<N_Vertex;++i){
		v[i].x-=avg.x; v[i].y-=avg.y; v[i].z-=avg.z;

		//rotate around z axis for 53 degrees
		double newX = 0.60181*v[i].x - 0.79863*v[i].y;
		double newY = 0.79863*v[i].x + 0.60181*v[i].y;
		double newZ = v[i].z;
		v[i].x = newX; v[i].y = newY; v[i].z = newZ;
	}
	fscanf(fp," %*c"); // get rid of the \n

	//Get the vertices around waist
	int waistMap2Vec[N_Vertex]={}, waistMap2_Ori[N_Vertex]={};
	fill_n(waistMap2Vec, N_Vertex, NONE);
	vector<int> waistIndex = GetWaistVertex(v,waistMap2Vec,waistMap2_Ori);
	

	//Skip the nornal data
	char skip[100];
	for(int i=0;i<N_Vertex;++i)
		fgets(skip,100,fp);
	
	//Load face
	//Format : f v1//vn1 v2//vn2 v3//vn3
	int index1, index2, index3;
	int face[N_FACE][3];
	for(int i=0;i<N_FACE;++i){
		fscanf(fp,"%*s   %d %*c%*c%*d   %d %*c%*c%*d   %d %*c%*c%*d",
			&index1, &index2, &index3);
		//printf("%d %d %d\n",index1, index2, index3);
		index1--,index2--,index3--;//The obj file start from 1. Minus 1 to match c++ code.
		face[i][0]=index1, face[i][1]=index2, face[i][2]=index3;
		InitWaistDistance(waistMap2Vec,v,waistDis,index1,index2,index3);
	}
	WaistDisconnectLeft(waistDis,waistMap2Vec,waistMap2_Ori,v,waistIndex.size());


	int startId = waistMap2Vec[WaistBasePoint];
	double waistLen = WaistDijkstra(waistDis,startId,waistIndex.size(),v,waistMap2_Ori);

	Hip hip;
	hip.SetHipBaseV(v);
	hip.SetHipVertexMapping(v);
	hip.SetGraph(face, v);
	hip.DisConnectLeft(v);
	hip.HipDijkstra(v);

	Thigh thigh;
	thigh.SetThighVertexMapping(v);
	thigh.SetGraph(face, v);
	thigh.DisConnectLeft(v);
	thigh.ThighDijkstra(v);

	Calf calf;
	calf.SetCalfVertexMapping(v);
	calf.SetGraph(face, v);
	calf.DisConnectLeft(v);
	calf.CalfDijkstra(v);

	Chest chest;
	chest.SetChestBaseV(v);
	chest.SetChestVertexMapping(v);
	chest.SetGraph(face, v);
	chest.DisConnectLeft(v);
	chest.ChestDijkstra(v);

	printf("The height is %.1f cm.\n",GetHight(Max,Min)*100);
	printf("The waist girth is %f\n",waistLen);

	WriteNewObj(v,face);
	fclose(fp);
}