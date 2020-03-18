#include "Hip.h"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <vector>
using namespace std;
#define HipArea 0.04//0.02~0.03
#define N_FACE 25000
#define NONE -1

Hip::Hip(){
	int id[N_HipVertex] = 
	{5753,5490,5293,5126,5634,5511,5244,5130,5106,5645,
	 5694,5499,5343,5166,5012,5765,5414,5187,5038,5728,
	 5580,5323,5081,5654,5398,5204,5077,5742,5389,5078,
	 5692,5418,5176,5615,5354,5248,5000};
	copy(id,id+N_HipVertex,this->vID);

	for(int i=0;i<N_Vertex;++i)
		Map2Hip[i] = Map2Ori[i] = NONE;
	this->nHip=0;

	// Init 2D array
	this->dis = new double*[GraphSize];
	for(int i = 0; i < GraphSize; ++i)
    	this->dis[i] = new double[GraphSize]();
    if(this->dis==NULL)cout<<"Hip memory not enough\n";

    this->length = 0;
}

void Hip::SetHipBaseV(Vertex v[]){
	double MaxY = -10; 
	//Find the most perky vertex on hip
	for(int i=0;i<N_HipVertex;++i)
		if(v[this->vID[i]].y > MaxY){
			MaxY = v[this->vID[i]].y;
			this->HipBaseV = v[this->vID[i]];
			this->baseID = this->vID[i];
		}
}

void Hip::SetHipVertexMapping(Vertex v[]){
	double high = this->HipBaseV.z + HipArea; 
	double low  = this->HipBaseV.z - HipArea; 
	for(int i=0;i<N_Vertex;++i)
		if(v[i].z < high && v[i].z > low){
			//cout<<i<<endl;
			this->Map2Hip[i]=this->nHip;
			this->Map2Ori[this->nHip]=i;
			++this->nHip;
		}
	//cout<<"Hips vertices: "<<this->nHip<<endl;
}

void Hip::SetGraph(int face[][3], Vertex v[]){
	for(int i=0;i<N_FACE;++i){
		int A = face[i][0], B = face[i][1], C = face[i][2];
		if(this->Map2Hip[A]!=NONE && this->Map2Hip[B]!=NONE && this->Map2Hip[C]!=NONE ){//Hip triangle mesh
			//cut the circle to be a plane
			double vX[3] = {v[A].x, v[B].x, v[C].x};
			sort(vX,vX+3);
			double pivot = this->HipBaseV.x;
			if( (vX[0]<pivot && vX[1]>pivot) || (vX[1]<pivot && vX[2]>pivot) ||
				(vX[0]<pivot && vX[2]>pivot)  )
				//Don't cut triangles in the back of hip.
				//Suppose the thickness of front and back hip exceeds 0.03m.
				if( this->HipBaseV.y-0.03< v[A].y && v[A].y < this->HipBaseV.y+0.03 )
					continue ;
		
			double d12 = sqrt( pow(v[A].x-v[B].x,2) + pow(v[A].y-v[B].y,2) );
			double d13 = sqrt( pow(v[A].x-v[C].x,2) + pow(v[A].y-v[C].y,2) );
			double d23 = sqrt( pow(v[B].x-v[C].x,2) + pow(v[B].y-v[C].y,2) );
			int v1 = this->Map2Hip[A], v2 = this->Map2Hip[B], v3 = this->Map2Hip[C];
			this->dis[v1][v2] = this->dis[v2][v1] = d12;
			this->dis[v1][v3] = this->dis[v3][v1] = d13;
			this->dis[v2][v3] = this->dis[v3][v2] = d23;
		}
	}
	//cout<<"set graph finished\n";
}

// Disconnect the left path of the HipBaseV. Make the Hip path circle the body.
void Hip::DisConnectLeft(Vertex v[]){
	int id = this->Map2Hip[this->baseID];
	double hipX = this->HipBaseV.x;
	for(int i=0;i<this->nHip;++i){
		int oriId = this->Map2Ori[i];
		if(this->dis[id][i]!=0)
			if(v[oriId].x < hipX)
				this->dis[id][i] = 0;//Disconnect.
	}
}

void Hip::HipDijkstra(Vertex v[]){
	double d[this->nHip]; fill_n(d,this->nHip,1e6);
	bool visit[this->nHip]={};

	int startId = this->Map2Hip[this->baseID];
	int parent[this->nHip]; fill_n(parent,this->nHip,-1);
	parent[startId] = startId; d[startId] = 0;
	for(int i=0;i<this->nHip;++i){
		double min = 1e6, nearBaseZ = 10;
		int id;
		for(int j=0;j<this->nHip;++j)
			if(visit[j] == false && d[j]!=1e6){
				int OriId = this->Map2Ori[j];//Use the vertices whose z value is closer to the z of BasePoint 
				double diff = v[OriId].z - v[baseID].z;
				if(diff<0)diff=-diff;
				if(diff<nearBaseZ)
					min = d[j], id = j, nearBaseZ = diff;
				
			}

		if(min==1e6)break;//all the vertices connected to start point have been used

		visit[id] = true;
		for(int j=0;j<this->nHip;++j)
			if(visit[j]==false && this->dis[id][j]!=0 && d[id] + this->dis[id][j]<d[j]){
				d[j] = d[id] + this->dis[id][j];
				parent[j] = id;
			}
	}

	int last = FindEndVertex(d,v);
	//Debug： Output the hip path
	FILE *hipPath = fopen("HipPath.obj", "w");
	if(hipPath == NULL){
		cout<<"Cannot create HipPath.obj\n";
		return;
	}
	int now = last, all=1;
	while(now!=startId){
		Vertex p = v[this->Map2Ori[now]];
		fprintf(hipPath,"v %f %f %f\n",p.x,p.y,p.z);
		now = parent[now];
		all++;
	}
	Vertex p = v[this->Map2Ori[startId]];
	fprintf(hipPath,"v %f %f %f\n",p.x,p.y,p.z);
	fprintf(hipPath,"l");
	for(int i=1;i<=all;++i)fprintf(hipPath," %d",i);
	fprintf(hipPath," 1");//Connect to the last point. Make it a complete circle.
	fclose(hipPath);

	//cout<<"Hip length:"<<this->length<<endl;
}

int Hip::FindEndVertex(double d[], Vertex v[]){
	vector<EndPoint> e;
	for(int i=0;i<this->nHip;++i)
		if(d[i]!=1e6){
			EndPoint newE = {d[i],i};
			e.push_back(newE);
		}
	sort(e.begin(),e.end());

	int closetId, baseId=this->baseID;
	double closet=99;
	for(int i=0;i<10;++i){//Pick the vertex nearnest to the basePoint from the farthest 10 vertices
		int id = this->Map2Ori[e[i].id];
		double dis = v[id].z-v[baseId].z;
		if(dis<0)dis=-dis;

		if(dis<closet)closet=dis, closetId=e[i].id;
	}
	this->length = d[closetId];
	return closetId;
}

Hip::~Hip(){
	for (int i = 0; i < GraphSize; i++) {
        delete[] dis[i];
    }
    delete[] dis;
}