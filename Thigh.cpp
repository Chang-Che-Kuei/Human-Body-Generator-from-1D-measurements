#include "Thigh.h"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <math.h>
using namespace std;
#define ThighArea 0.04
#define N_FACE 25000
#define NONE -1

Thigh::Thigh(){
	//The vertices which are used to disconnect left and right legs.
	//They are the vertices in the area of crotch
	fill_n(crotch,N_Vertex,false);
	crotch[4337] = crotch[4289] = crotch[12486] = crotch[12487] = crotch[12491] =
	crotch[12493] = crotch[12498] = crotch[4122] = crotch[12499] = crotch[12489] =
	crotch[4206] = crotch[4320] = crotch[4486] = crotch[4553] = crotch[4620] = 
	crotch[4354] = crotch[4394] = crotch[4490] = crotch[4564] = crotch[4642] =
	crotch[4732] = crotch[4933] = crotch[4975] = crotch[5158] = crotch[5281] =
	crotch[12489] = crotch[4206] = crotch[4320] = crotch[4486] = crotch[4553] =
	crotch[4620] = crotch[4669] = crotch[4728] = 
	crotch[4760] = crotch[4837] = crotch[5012] = crotch[5166] = crotch[5343] =true;

	for(int i=0;i<N_Vertex;++i)
		Map2Thigh[i] = Map2Ori[i] = NONE;
	nThigh=0;

	// Init 2D array
	dis = new double*[GraphSize];
	for(int i = 0; i < GraphSize; ++i)
    	dis[i] = new double[GraphSize]();
    if(dis==NULL)cout<<"Hip memory not enough\n";

    length = 0;
}

void Thigh::SetThighVertexMapping(Vertex v[]){
	ThighBaseV = v[ThighBaseID];

	double high = this->ThighBaseV.z + ThighArea; 
	double low  = this->ThighBaseV.z - ThighArea; 
	for(int i=0;i<N_Vertex;++i)// use variable crotch to disable the vertices on the area of crotch
		if(v[i].z < high && v[i].z > low && this->crotch[i]==false){
			//cout<<i<<endl;
			this->Map2Thigh[i]=this->nThigh;
			this->Map2Ori[this->nThigh]=i;
			++this->nThigh;
		}
}

void Thigh::SetGraph(int face[][3], Vertex v[]){
	FILE* check = fopen("CheckThigh.obj","w");
	if(check==NULL){
		cout<<"Cannot open Check.obj\n";
		return;
	}
	int nowV = 1;

	for(int i=0;i<N_FACE;++i){
		int A = face[i][0], B = face[i][1], C = face[i][2];
		if(this->Map2Thigh[A]!=NONE && this->Map2Thigh[B]!=NONE && this->Map2Thigh[C]!=NONE ){//Thigh triangle mesh
			//cut the circle to be a plane
			double vX[3] = {v[A].x, v[B].x, v[C].x};
			sort(vX,vX+3);
			double pivot = this->ThighBaseV.x;
			if( (vX[0]<pivot && vX[1]>pivot) || (vX[1]<pivot && vX[2]>pivot) ||
				(vX[0]<pivot && vX[2]>pivot)  )
				//Don't cut triangles in the back of Thigh.
				//Suppose the thickness of front and back thigh exceeds 0.02m.
				if( this->ThighBaseV.y-0.03< v[A].y && v[A].y < this->ThighBaseV.y+0.03 )
					continue ;
		
			double d12 = sqrt( pow(v[A].x-v[B].x,2) + pow(v[A].y-v[B].y,2) );
			double d13 = sqrt( pow(v[A].x-v[C].x,2) + pow(v[A].y-v[C].y,2) );
			double d23 = sqrt( pow(v[B].x-v[C].x,2) + pow(v[B].y-v[C].y,2) );
			int v1 = this->Map2Thigh[A], v2 = this->Map2Thigh[B], v3 = this->Map2Thigh[C];
			this->dis[v1][v2] = this->dis[v2][v1] = d12;
			this->dis[v1][v3] = this->dis[v3][v1] = d13;
			this->dis[v2][v3] = this->dis[v3][v2] = d23;

			fprintf(check,"v %f %f %f\n",v[A].x, v[A].y, v[A].z);
			fprintf(check,"v %f %f %f\n",v[B].x, v[B].y, v[B].z);
			fprintf(check,"v %f %f %f\n",v[C].x, v[C].y, v[C].z);
			fprintf(check,"l %d %d %d %d\n",nowV,nowV+1,nowV+2,nowV);
			nowV+=3;
		}
	}
	fclose(check);
}

void Thigh::DisConnectLeft(Vertex v[]){
	FILE* fp =fopen("DisconnectThigh.obj","w");
	int now = 1;

	int id = this->Map2Thigh[ThighBaseID];
	double ThighX = this->ThighBaseV.x;
	for(int i=0;i<this->nThigh;++i){
		int oriId = this->Map2Ori[i];
		if(this->dis[id][i]!=0)
			if(v[oriId].x < ThighX){
				this->dis[id][i] = 0;//Disconnect.
				fprintf(fp,"v %f %f %f\n",ThighBaseV.x,ThighBaseV.y,ThighBaseV.z);
				fprintf(fp,"v %f %f %f\n",v[oriId].x, v[oriId].y, v[oriId].z);;
				fprintf(fp,"l %d %d\n",now,now+1 );
				now+=2;
			}
	}
	fclose(fp);

/*
	int id = this->Map2Thigh[ThighBaseID];
	double ThighX = this->ThighBaseV.x;
	for(int i=0;i<this->nThigh;++i){
		int oriId = this->Map2Ori[i];
		if(this->dis[id][i]!=0)
			if(v[oriId].x < ThighX)
				this->dis[id][i] = 0;//Disconnect.
	}*/
}


void Thigh::ThighDijkstra(Vertex v[]){
	double d[this->nThigh]; fill_n(d,this->nThigh,1e6);
	bool visit[this->nThigh]={};

	int startId = this->Map2Thigh[ThighBaseID];
	int parent[this->nThigh]; fill_n(parent,this->nThigh,-1);
	parent[startId] = startId; d[startId] = 0;
	for(int i=0;i<this->nThigh;++i){
		double min = 1e6, nearBaseZ = 10;
		int id;
		for(int j=0;j<this->nThigh;++j)
			if(visit[j] == false && d[j]!=1e6){
				int OriId = this->Map2Ori[j];//Use the vertices whose z value is closer to the z of BasePoint 
				double diff = v[OriId].z - v[ThighBaseID].z;
				if(diff<0)diff=-diff;
				if(diff<nearBaseZ)
					min = d[j], id = j, nearBaseZ = diff;
				
			}

		if(min==1e6)break;//all the vertices connected to start point have been used

		visit[id] = true;
		for(int j=0;j<this->nThigh;++j)
			if(visit[j]==false && this->dis[id][j]!=0 && d[id] + this->dis[id][j]<d[j]){
				d[j] = d[id] + this->dis[id][j];
				parent[j] = id;
			}
	}

	int last = FindEndVertex(d,v);
	//Debug： Output the hip path
	FILE *ThighPath = fopen("ThighPath.obj", "w");
	if(ThighPath == NULL){
		cout<<"Cannot create ThighPath.obj\n";
		return;
	}
	int now = last, all=1;
	while(now!=startId){
		Vertex p = v[this->Map2Ori[now]];
		fprintf(ThighPath,"v %f %f %f\n",p.x,p.y,p.z);
		now = parent[now];
		all++;
	}
	Vertex p = v[this->Map2Ori[startId]];
	fprintf(ThighPath,"v %f %f %f\n",p.x,p.y,p.z);
	fprintf(ThighPath,"l");
	for(int i=1;i<=all;++i)fprintf(ThighPath," %d",i);
	fprintf(ThighPath," 1");//Connect to the last point. Make it a complete circle.
	fclose(ThighPath);

	//cout<<"Thigh length:"<<this->length<<endl;
}

int Thigh::FindEndVertex(double d[], Vertex v[]){
	vector<EndPoint> e;
	for(int i=0;i<this->nThigh;++i)
		if(d[i]!=1e6){
			EndPoint newE = {d[i],i};
			e.push_back(newE);
		}
	sort(e.begin(),e.end());

	int closetId, baseId=ThighBaseID;
	double closet=99;
	for(int i=0;i<10;++i){//Pick the vertex nearnest to the basePoint from the farthest 10 vertices
		int id = this->Map2Ori[e[i].id];
		double dis = v[id].z-v[baseId].z;
		if(dis<0)dis=-dis;

		if(dis<closet)closet=dis, closetId=e[i].id;
	}
	this->length = d[closetId];

	Vertex startP = v[baseId]; //Don't forget to add the length from the last point to start point
	Vertex endP = v[Map2Ori[closetId]];
	this->length += sqrt( pow(startP.x-endP.x,2) + pow(startP.y-endP.y,2) );
	return closetId;
}

Thigh::~Thigh(){
	for (int i = 0; i < GraphSize; i++) {
        delete[] dis[i];
    }
    delete[] dis;
}