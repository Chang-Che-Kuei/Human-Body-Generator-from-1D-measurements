#include "otherMeasure.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
using namespace std;

void OtherMeasure(Vertex v[], double otherM[]){
	//printf("\nOther Measurements:\n");
	otherM[0] = ArmLength(v);
	otherM[1] = LegLength(v);
	otherM[2] = BackLength(v);
	otherM[3] = ShoulderHeight(v);
	otherM[4] = ChestHeight(v);
	otherM[5] = ChestDepth(v); // BP --> shoulder
	otherM[6] = HipHeight(v);
	otherM[7] = KneeHeight(v);
	otherM[8] = HipDepth(v);
	//cout<<"/////////////////////////\n";

	//Debug
	FILE *other = fopen("other.obj","w");
	if(other == NULL){
		cout<<"Cannot open other.obj.\n";
		return;
	}
	int n = 1;
	fprintf(other,"v %f %f %f\n",v[10751].x,v[10751].y,v[10751].z); //Arm
	fprintf(other,"v %f %f %f\n",v[9046].x,v[9046].y,v[9046].z);
	fprintf(other,"v %f %f %f\n",v[6732].x,v[6732].y,v[6732].z);
	fprintf(other,"l %d %d %d\n",n,n+1,n+2);  n+=3;
	fprintf(other,"v %f %f %f\n",v[12491].x,v[12491].y,v[12491].z); //Leg
	fprintf(other,"v %f %f %f\n",v[12491].x,v[12491].y,v[21].z);
	fprintf(other,"l %d %d\n",n,n+1);   n+=2;
	fprintf(other,"v %f %f %f\n",v[10631].x,v[10631].y,v[10631].z); //back
	fprintf(other,"v %f %f %f\n",v[10751].x,v[10751].y,v[10751].z);
	fprintf(other,"l %d %d\n",n,n+1);   n+=2;
	fprintf(other,"v %f %f %f\n",v[10751].x,v[10751].y,v[10751].z); //ShoulderHeight
	fprintf(other,"v %f %f %f\n",v[10751].x,v[10751].y,v[21].z);
	fprintf(other,"l %d %d\n",n,n+1);  n+=2;
	fprintf(other,"v %f %f %f\n",v[9261].x,v[9261].y,v[9261].z); //ChestHeight
	fprintf(other,"v %f %f %f\n",v[9261].x,v[9261].y,v[21].z);
	fprintf(other,"l %d %d\n",n,n+1);  n+=2;
	fprintf(other,"v %f %f %f\n",v[9261].x,v[9261].y,v[9261].z); //ChestDepth
	fprintf(other,"v %f %f %f\n",v[9261].x,v[8986].y,v[9261].z);
	fprintf(other,"l %d %d\n",n,n+1);  n+=2;
	fprintf(other,"v %f %f %f\n",v[5038].x,v[5038].y,v[5038].z); //HipHeight
	fprintf(other,"v %f %f %f\n",v[5038].x,v[5038].y,v[21].z);
	fprintf(other,"l %d %d\n",n,n+1);  n+=2;
	fprintf(other,"v %f %f %f\n",v[2645].x,v[2645].y,v[2645].z); //KneeHeight
	fprintf(other,"v %f %f %f\n",v[2645].x,v[2645].y,v[21].z);
	fprintf(other,"l %d %d\n",n,n+1);  n+=2;
	fprintf(other,"v %f %f %f\n",v[5038].x,v[5038].y,v[5038].z); //HipDepth
	fprintf(other,"v %f %f %f\n",v[5412].x,v[5412].y,v[5412].z);
	fprintf(other,"l %d %d\n",n,n+1);  n+=2;
	//fprintf(other,"v %f %f %f\n",v[10841].x,v[10841].y,v[10841].z); //UpperBody
	//fprintf(other,"v %f %f %f\n",v[10841].x,v[10841].y,v[6924].z);
	//fprintf(other,"l %d %d\n",8,9);
	fclose(other);
}

double ArmLength(Vertex v[]){
	Vertex shoulder = v[10751];
	Vertex elbow = v[9046];
	Vertex wrist = v[6732];
	double s2e = sqrt( pow(shoulder.x-elbow.x,2) + pow(shoulder.y-elbow.y,2) + pow(shoulder.z-elbow.z,2) );
	double e2w = sqrt( pow(elbow.x-wrist.x,2) + pow(elbow.y-wrist.y,2) + pow(elbow.z-wrist.z,2) );
	//cout<<"Arm length : "<<length<<endl;
	return s2e + e2w;
}

double LegLength(Vertex v[]){
	Vertex crotch = v[12491];
	Vertex soleplate = v[21];
	double length = crotch.z - soleplate.z;
	//cout<<"Leg length : "<<length<<endl;
	return length;
}

double BackLength(Vertex v[]){
	Vertex rightShoulder = v[10631];
	Vertex leftShoulder = v[10751];
	double length = sqrt(pow(rightShoulder.x - leftShoulder.x,2) + pow(rightShoulder.y - leftShoulder.y,2) + pow(rightShoulder.z - leftShoulder.z,2) );
	//cout<<"Back length : "<<length<<endl;
	return length;
}

double ShoulderHeight(Vertex v[]){
	Vertex leftShoulder = v[10751];
	Vertex soleplate = v[21];
	return leftShoulder.z - soleplate.z;
}

double ChestHeight(Vertex v[]){//nipple height
	Vertex chest = v[9261];
	Vertex soleplate = v[21];
	return chest.z - soleplate.z;
}

double ChestDepth(Vertex v[]){
	Vertex chest = v[9261];
	Vertex back = v[8986];
	return back.y - chest.y;
}

double HipHeight(Vertex v[]){//most perky point
	Vertex hip = v[5038];
	Vertex soleplate = v[21];
	return hip.z - soleplate.z;
}

double KneeHeight(Vertex v[]){
	Vertex knee = v[2645];
	Vertex soleplate = v[21];
	return knee.z - soleplate.z;
}

double HipDepth(Vertex v[]){//most perky point
	Vertex hip = v[5038];
	Vertex hipFront = v[5412];
	return hip.y - hipFront.y;
}
/*
double UpperBodyLength(Vertex v[]){
	Vertex TopShoulder = v[10841];
	Vertex TopPant = v[6924];
	double length = TopShoulder.z - TopPant.z;
	//cout<<"Upper body length : "<<length<<endl;
	return length;
}
*/