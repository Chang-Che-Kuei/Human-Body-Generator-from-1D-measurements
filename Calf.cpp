#include "Calf.h"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <math.h>
using namespace std;
#define CalfArea 0.03
#define N_FACE 25000
#define NONE -1

Calf::Calf(){
	for(int i=0;i<N_Vertex;++i)
		Map2Calf[i] = Map2Ori[i] = NONE;
	nCalf=0;

	// Init 2D array
	dis = new double*[GraphSize];
	for(int i = 0; i < GraphSize; ++i)
    	dis[i] = new double[GraphSize]();
    if(dis==NULL)cout<<"Hip memory not enough\n";

    length = 0;
}

void Calf::SetCalfVertexMapping(Vertex v[]){
	CalfBaseV = v[CalfBaseID];

	double high = this->CalfBaseV.z + CalfArea; 
	double low  = this->CalfBaseV.z - CalfArea; 
	for(int i=0;i<N_Vertex;++i)// use variable crotch to disable the vertices on the area of crotch
		if(v[i].z < high && v[i].z > low ){
			//cout<<i<<endl;
			this->Map2Calf[i]=this->nCalf;
			this->Map2Ori[this->nCalf]=i;
			++this->nCalf;
		}
}

void Calf::SetGraph(int face[][3], Vertex v[]){
	/*FILE* check = fopen("Check.obj","w");
	if(check==NULL){
		cout<<"Cannot open Check.obj\n";
		return;
	}
	int nowV = 1;*/

	for(int i=0;i<N_FACE;++i){
		int A = face[i][0], B = face[i][1], C = face[i][2];
		if(this->Map2Calf[A]!=NONE && this->Map2Calf[B]!=NONE && this->Map2Calf[C]!=NONE ){//Calf triangle mesh
			//cut the circle to be a plane
			double vX[3] = {v[A].x, v[B].x, v[C].x};
			sort(vX,vX+3);
			double pivot = this->CalfBaseV.x;
			if( (vX[0]<pivot && vX[1]>pivot) || (vX[1]<pivot && vX[2]>pivot) ||
				(vX[0]<pivot && vX[2]>pivot)  )
				//Don't cut triangles in the back of calf.
				//Suppose the thickness of front and back calf exceeds 0.02m.
				if( this->CalfBaseV.y-0.02< v[A].y && v[A].y < this->CalfBaseV.y+0.02 )
					continue ;
		
			double d12 = sqrt( pow(v[A].x-v[B].x,2) + pow(v[A].y-v[B].y,2) );
			double d13 = sqrt( pow(v[A].x-v[C].x,2) + pow(v[A].y-v[C].y,2) );
			double d23 = sqrt( pow(v[B].x-v[C].x,2) + pow(v[B].y-v[C].y,2) );
			int v1 = this->Map2Calf[A], v2 = this->Map2Calf[B], v3 = this->Map2Calf[C];
			this->dis[v1][v2] = this->dis[v2][v1] = d12;
			this->dis[v1][v3] = this->dis[v3][v1] = d13;
			this->dis[v2][v3] = this->dis[v2][v3] = d23;

			/*fprintf(check,"v %f %f %f\n",v[A].x, v[A].y, v[A].z);
			fprintf(check,"v %f %f %f\n",v[B].x, v[B].y, v[B].z);
			fprintf(check,"v %f %f %f\n",v[C].x, v[C].y, v[C].z);
			fprintf(check,"l %d %d %d %d\n",nowV,nowV+1,nowV+2,nowV);
			nowV+=3;*/
		}
	}
}

void Calf::DisConnectLeft(Vertex v[]){
	int id = this->Map2Calf[CalfBaseID];
	double CalfX = this->CalfBaseV.x;
	for(int i=0;i<this->nCalf;++i){
		int oriId = this->Map2Ori[i];
		if(this->dis[id][i]!=0)
			if(v[oriId].x < CalfX)
				this->dis[id][i] = 0;//Disconnect.
	}
}


void Calf::CalfDijkstra(Vertex v[]){
	double d[this->nCalf]; fill_n(d,this->nCalf,1e6);
	bool visit[this->nCalf]={};

	int startId = this->Map2Calf[CalfBaseID];
	int parent[this->nCalf]; fill_n(parent,this->nCalf,-1);
	parent[startId] = startId; d[startId] = 0;
	for(int i=0;i<this->nCalf;++i){
		double min = 1e6, nearBaseZ = 10;
		int id;
		for(int j=0;j<this->nCalf;++j)
			if(visit[j] == false && d[j]!=1e6){
				int OriId = this->Map2Ori[j];//Use the vertices whose z value is closer to the z of BasePoint 
				double diff = v[OriId].z - v[CalfBaseID].z;
				if(diff<0)diff=-diff;
				if(diff<nearBaseZ)
					min = d[j], id = j, nearBaseZ = diff;	
			}

		if(min==1e6)break;//all the vertices connected to start point have been used

		visit[id] = true;
		for(int j=0;j<this->nCalf;++j)
			if(visit[j]==false && this->dis[id][j]!=0 && d[id] + this->dis[id][j]<d[j]){
				d[j] = d[id] + this->dis[id][j];
				parent[j] = id;
			}
	}

	int last;
	for(int i=0;i<this->nCalf;++i)
		if(d[i]!=1e6 && d[i]>this->length)
			this->length = d[i], last = i;

	//Debug： Output the hip path
	FILE *CalfPath = fopen("CalfPath.obj", "w");
	if(CalfPath == NULL){
		cout<<"Cannot create CalfPath.obj\n";
		return;
	}
	int now = last, all=1;
	while(now!=startId){
		Vertex p = v[this->Map2Ori[now]];
		fprintf(CalfPath,"v %f %f %f\n",p.x,p.y,p.z);
		now = parent[now];
		all++;
	}
	Vertex p = v[this->Map2Ori[startId]];
	fprintf(CalfPath,"v %f %f %f\n",p.x,p.y,p.z);
	fprintf(CalfPath,"l");
	for(int i=1;i<=all;++i)fprintf(CalfPath," %d",i);
	fprintf(CalfPath," 1");//Connect to the last point. Make it a complete circle.
	fclose(CalfPath);

	cout<<"Calf length:"<<this->length<<endl;
}