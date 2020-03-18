#include "UpperArm.h"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <math.h>
using namespace std;
#define ArmArea 0.04
#define N_FACE 25000
#define NONE -1

Arm::Arm(){
	ArmBaseID = 9401;
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

	for(int i=0;i<N_Vertex;++i)
		Map2Arm[i] = Map2Ori[i] = NONE;
	nArm=0;

	// Init 2D array
	dis = new double*[GraphSize];
	for(int i = 0; i < GraphSize; ++i)
    	dis[i] = new double[GraphSize]();
    if(dis==NULL)cout<<"Arm memory not enough\n";

    length = 0;
}


void Arm::SetArmVertexMapping(Vertex v[]){
	ArmBaseV = v[ArmBaseID];
	double high = this->ArmBaseV.z + ArmArea; 
	double low  = this->ArmBaseV.z - ArmArea; 
	for(int i=0;i<N_Vertex;++i)// use variable crotch to disable the vertices on the area of crotch
		if(v[i].z < high && v[i].z > low && this->underArm[i]==false){
			//cout<<i<<endl;
			this->Map2Arm[i]=this->nArm;
			this->Map2Ori[this->nArm]=i;
			++this->nArm;
		}
	//cout<<"There are "<<nArm<<" vertices in the Arm\n";
}

void Arm::SetGraph(int face[][3], Vertex v[]){
	FILE* check = fopen("CheckArm.obj","w");
	if(check==NULL){
		cout<<"Cannot open Check.obj\n";
		return;
	}
	int nowV = 1;

	for(int i=0;i<N_FACE;++i){
		int A = face[i][0], B = face[i][1], C = face[i][2];
		if(this->Map2Arm[A]!=NONE && this->Map2Arm[B]!=NONE && this->Map2Arm[C]!=NONE ){//Arm triangle mesh
			//cut the circle to be a plane
			double vX[3] = {v[A].x, v[B].x, v[C].x};
			sort(vX,vX+3);
			double pivot = this->ArmBaseV.x;
			if( (vX[0]<pivot && vX[1]>pivot) || (vX[1]<pivot && vX[2]>pivot) ||
				(vX[0]<pivot && vX[2]>pivot)  )
				//Don't cut triangles in the back of Arm.
				//Suppose the thickness of front and back Arm exceeds 0.03m.
				if( this->ArmBaseV.y-0.03< v[A].y && v[A].y < this->ArmBaseV.y+0.03 )
					continue ;
		
			double d12 = sqrt( pow(v[A].x-v[B].x,2) + pow(v[A].y-v[B].y,2)  );
			double d13 = sqrt( pow(v[A].x-v[C].x,2) + pow(v[A].y-v[C].y,2)  );
			double d23 = sqrt( pow(v[B].x-v[C].x,2) + pow(v[B].y-v[C].y,2)  );
			int v1 = this->Map2Arm[A], v2 = this->Map2Arm[B], v3 = this->Map2Arm[C];
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
	//cout<<"Set finished\n";
}

void Arm::DisConnectLeft(Vertex v[]){
	FILE* fp =fopen("DisconnectArm.obj","w");
	int now = 1;
	int id = this->Map2Arm[ArmBaseID];
	double ArmX = this->ArmBaseV.x;
	for(int i=0;i<this->nArm;++i){
		int oriId = this->Map2Ori[i];
		if(this->dis[id][i]!=0)
			if(v[oriId].x < ArmX){
				this->dis[id][i] = 0;//Disconnect.
				fprintf(fp,"v %f %f %f\n",ArmBaseV.x,ArmBaseV.y,ArmBaseV.z);
				fprintf(fp,"v %f %f %f\n",v[oriId].x, v[oriId].y, v[oriId].z);;
				fprintf(fp,"l %d %d\n",now,now+1 );
				now+=2;
			}
	}
	fclose(fp);
}


void Arm::ArmDijkstra(Vertex v[]){
	double d[this->nArm]; fill_n(d,this->nArm,1e6);
	bool visit[this->nArm]={};

	int startId = this->Map2Arm[ArmBaseID];
	int parent[this->nArm]; fill_n(parent,this->nArm,-1);
	parent[startId] = startId; d[startId] = 0;
	for(int i=0;i<this->nArm;++i){
		double min = 1e6;
		int id;
		for(int j=0;j<this->nArm;++j)
			if(visit[j] == false && d[j]<min)				
				min = d[j], id = j;

		if(min==1e6)break;//all the vertices connected to start point have been used

		visit[id] = true;
		for(int j=0;j<this->nArm;++j)
			if(visit[j]==false && this->dis[id][j]!=0 && d[id] + this->dis[id][j]<d[j]){
				d[j] = d[id] + this->dis[id][j];
				parent[j] = id;
			}
	}

	int last = FindEndVertex(d,v);


	//Debug： Output the hip path
	FILE *ArmPath = fopen("ArmPath.obj", "w");
	if(ArmPath == NULL){
		cout<<"Cannot create ArmPath.obj\n";
		return;
	}
	int now = last, all=1;
	while(now!=startId){
		Vertex p = v[this->Map2Ori[now]];
		fprintf(ArmPath,"v %f %f %f\n",p.x,p.y,p.z);
		now = parent[now];
		all++;
	}
	Vertex p = v[this->Map2Ori[startId]];
	fprintf(ArmPath,"v %f %f %f\n",p.x,p.y,p.z);
	fprintf(ArmPath,"l");
	for(int i=1;i<=all;++i)fprintf(ArmPath," %d",i);
	fprintf(ArmPath," 1");//Connect to the last point. Make it a complete circle.
	fclose(ArmPath);

	//cout<<"Arm length:"<<this->length<<endl;
}

int Arm::FindEndVertex(double d[], Vertex v[]){
	vector<EndPoint> e;
	for(int i=0;i<this->nArm;++i)
		if(d[i]!=1e6){
			EndPoint newE = {d[i],i};
			e.push_back(newE);
		}
	sort(e.begin(),e.end());

	int closetId, baseId=ArmBaseID;
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

Arm::~Arm(){
	for (int i = 0; i < GraphSize; i++) {
        delete[] dis[i];
    }
    delete[] dis;
}