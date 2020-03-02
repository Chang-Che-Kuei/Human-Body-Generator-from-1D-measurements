#include "Chest.h"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <math.h>
using namespace std;
#define ChestArea 0.05
#define N_FACE 25000
#define NONE -1

Chest::Chest(){
	//The vertices which are used to disconnect the path to arms.
	fill_n(underArm,N_Vertex,false);
	//Left underarm
	underArm[10314] = underArm[10200] = underArm[10131] = underArm[10059] = underArm[10032] = 
	underArm[9989] = underArm[9935] = underArm[9833] = underArm[9770] = underArm[9712] = 
	underArm[9715] = underArm[9702] = underArm[9756] = underArm[9849] = underArm[9881] = 
	underArm[9962] = underArm[10056] = underArm[10150] = true;
	//rigth undrearm
	underArm[9932] = underArm[9956] = underArm[10034] = underArm[10128] = underArm[9890] = 
	underArm[9824] = underArm[9804] = underArm[9784] = underArm[9650] = underArm[9572] = 
	underArm[9550] = underArm[9574] = underArm[9594] = underArm[9708] = underArm[9743] = 
	underArm[9807] = underArm[9852] = underArm[9974] = underArm[10099] = underArm[10108] = true;

	//BasePoint
	int id[N_BaseChest] = {
		9649,9496,9422,9241,9092,9041,8958,8794,8891,8290,
		9043,9172,9559,9427,9340,9434,9261,9141,9261,9580,
		9440,9243,9051,8928
	};
	copy(id,id+N_BaseChest,this->baseChest);

	for(int i=0;i<N_Vertex;++i)
		Map2Chest[i] = Map2Ori[i] = NONE;
	nChest=0;

	// Init 2D array
	dis = new double*[GraphSize];
	for(int i = 0; i < GraphSize; ++i)
    	dis[i] = new double[GraphSize]();
    if(dis==NULL)cout<<"Hip memory not enough\n";

    length = 0;
}

void Chest::SetChestBaseV(Vertex v[]){
	double MinY = 10; 
	//Find the most perky vertex on chest
	for(int i=0;i<N_BaseChest;++i)
		if(v[this->baseChest[i]].y < MinY){
			MinY = v[this->baseChest[i]].y;
			this->ChestBaseV = v[this->baseChest[i]];
			this->ChestBaseID = this->baseChest[i];
		}
}

void Chest::SetChestVertexMapping(Vertex v[]){
	double high = this->ChestBaseV.z + ChestArea; 
	double low  = this->ChestBaseV.z - ChestArea; 
	for(int i=0;i<N_Vertex;++i)// use variable crotch to disable the vertices on the area of crotch
		if(v[i].z < high && v[i].z > low && this->underArm[i]==false){
			//cout<<i<<endl;
			this->Map2Chest[i]=this->nChest;
			this->Map2Ori[this->nChest]=i;
			++this->nChest;
		}
	//cout<<"There are "<<nChest<<" vertices in the chest\n";
}

void Chest::SetGraph(int face[][3], Vertex v[]){
	FILE* check = fopen("Check.obj","w");
	if(check==NULL){
		cout<<"Cannot open Check.obj\n";
		return;
	}
	int nowV = 1;

	for(int i=0;i<N_FACE;++i){
		int A = face[i][0], B = face[i][1], C = face[i][2];
		if(this->Map2Chest[A]!=NONE && this->Map2Chest[B]!=NONE && this->Map2Chest[C]!=NONE ){//Chest triangle mesh
			//cut the circle to be a plane
			double vX[3] = {v[A].x, v[B].x, v[C].x};
			sort(vX,vX+3);
			double pivot = this->ChestBaseV.x;
			if( (vX[0]<pivot && vX[1]>pivot) || (vX[1]<pivot && vX[2]>pivot) ||
				(vX[0]<pivot && vX[2]>pivot)  )
				//Don't cut triangles in the back of Chest.
				//Suppose the thickness of front and back Chest exceeds 0.05m.
				if( this->ChestBaseV.y-0.05< v[A].y && v[A].y < this->ChestBaseV.y+0.05 )
					continue ;
		
			double d12 = sqrt( pow(v[A].x-v[B].x,2) + pow(v[A].y-v[B].y,2) );
			double d13 = sqrt( pow(v[A].x-v[C].x,2) + pow(v[A].y-v[C].y,2) );
			double d23 = sqrt( pow(v[B].x-v[C].x,2) + pow(v[B].y-v[C].y,2) );
			int v1 = this->Map2Chest[A], v2 = this->Map2Chest[B], v3 = this->Map2Chest[C];
			this->dis[v1][v2] = this->dis[v2][v1] = d12;
			this->dis[v1][v3] = this->dis[v3][v1] = d13;
			this->dis[v2][v3] = this->dis[v2][v3] = d23;

			fprintf(check,"v %f %f %f\n",v[A].x, v[A].y, v[A].z);
			fprintf(check,"v %f %f %f\n",v[B].x, v[B].y, v[B].z);
			fprintf(check,"v %f %f %f\n",v[C].x, v[C].y, v[C].z);
			fprintf(check,"l %d %d %d %d\n",nowV,nowV+1,nowV+2,nowV);
			nowV+=3;
		}
	}
}

void Chest::DisConnectLeft(Vertex v[]){
	FILE* fp =fopen("Disconnect.obj","w");
	int now = 1;

	int id = this->Map2Chest[ChestBaseID];
	double ChestX = this->ChestBaseV.x;
	for(int i=0;i<this->nChest;++i){
		int oriId = this->Map2Ori[i];
		if(this->dis[id][i]!=0)
			if(v[oriId].x < ChestX){
				this->dis[id][i] = 0;//Disconnect.
				fprintf(fp,"v %f %f %f\n",ChestBaseV.x,ChestBaseV.y,ChestBaseV.z);
				fprintf(fp,"v %f %f %f\n",v[oriId].x, v[oriId].y, v[oriId].z);;
				fprintf(fp,"l %d %d\n",now,now+1 );
				now+=2;
			}
	}
}


void Chest::ChestDijkstra(Vertex v[]){
	double d[this->nChest]; fill_n(d,this->nChest,1e6);
	bool visit[this->nChest]={};

	int startId = this->Map2Chest[ChestBaseID];
	int parent[this->nChest]; fill_n(parent,this->nChest,-1);
	parent[startId] = startId; d[startId] = 0;
	for(int i=0;i<this->nChest;++i){
		double min = 1e6, nearBaseZ = 10;
		int id;
		for(int j=0;j<this->nChest;++j)
			if(visit[j] == false && d[j]!=1e6){
				int OriId = this->Map2Ori[j];//Use the vertices whose z value is closer to the z of BasePoint 
				double diff = v[OriId].z - v[ChestBaseID].z;
				if(diff<0)diff=-diff;
				if(diff<nearBaseZ)
					min = d[j], id = j, nearBaseZ = diff;
			}

		if(min==1e6)break;//all the vertices connected to start point have been used

		visit[id] = true;
		for(int j=0;j<this->nChest;++j)
			if(visit[j]==false && this->dis[id][j]!=0 && d[id] + this->dis[id][j]<d[j]){
				d[j] = d[id] + this->dis[id][j];
				parent[j] = id;
			}
	}

	int last;
	for(int i=0;i<this->nChest;++i)
		if(d[i]!=1e6 && d[i]>this->length)
			this->length = d[i], last = i;

	//Debug： Output the hip path
	FILE *ChestPath = fopen("ChestPath.obj", "w");
	if(ChestPath == NULL){
		cout<<"Cannot create ChestPath.obj\n";
		return;
	}
	int now = last, all=1;
	while(now!=startId){
		Vertex p = v[this->Map2Ori[now]];
		fprintf(ChestPath,"v %f %f %f\n",p.x,p.y,p.z);
		now = parent[now];
		all++;
	}
	Vertex p = v[this->Map2Ori[startId]];
	fprintf(ChestPath,"v %f %f %f\n",p.x,p.y,p.z);
	fprintf(ChestPath,"l");
	for(int i=1;i<=all;++i)fprintf(ChestPath," %d",i);
	fprintf(ChestPath," 1");//Connect to the last point. Make it a complete circle.
	fclose(ChestPath);

	cout<<"Chest length:"<<this->length<<endl;
}