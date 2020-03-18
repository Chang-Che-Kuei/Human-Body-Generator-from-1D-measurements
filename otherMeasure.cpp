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
	otherM[3] = UpperBodyLength(v);
	//cout<<"/////////////////////////\n";

	//Debug
	FILE *other = fopen("other.obj","w");
	if(other == NULL){
		cout<<"Cannot open other.obj.\n";
		return;
	}
	fprintf(other,"v %f %f %f\n",v[10686].x,v[10686].y,v[10686].z); //Arm
	fprintf(other,"v %f %f %f\n",v[8956].x,v[8956].y,v[8956].z);
	fprintf(other,"v %f %f %f\n",v[6750].x,v[6750].y,v[6750].z);
	fprintf(other,"l %d %d %d\n",1,2,3);
	fprintf(other,"v %f %f %f\n",v[4217].x,v[4217].y,v[4217].z); //Leg
	fprintf(other,"v %f %f %f\n",v[4217].x,v[4217].y,v[21].z);
	fprintf(other,"l %d %d\n",4,5);
	fprintf(other,"v %f %f %f\n",v[10791].x,v[10791].y,v[10791].z); //Back
	fprintf(other,"v %f %f %f\n",v[10827].x,v[10827].y,v[10827].z);
	fprintf(other,"l %d %d\n",6,7);
	fprintf(other,"v %f %f %f\n",v[10841].x,v[10841].y,v[10841].z); //UpperBody
	fprintf(other,"v %f %f %f\n",v[10841].x,v[10841].y,v[6924].z);
	fprintf(other,"l %d %d\n",8,9);
	fclose(other);
}

double ArmLength(Vertex v[]){
	Vertex shoulder = v[10686];
	Vertex elbow = v[8956];
	Vertex wrist = v[6750];
	double s2e = sqrt( pow(shoulder.x-elbow.x,2) + pow(shoulder.y-elbow.y,2) + pow(shoulder.z-elbow.z,2) );
	double e2w = sqrt( pow(elbow.x-wrist.x,2) + pow(elbow.y-wrist.y,2) + pow(elbow.z-wrist.z,2) );
	//cout<<"Arm length : "<<length<<endl;
	return s2e + e2w;
}

double LegLength(Vertex v[]){
	Vertex umbilicus = v[4217];
	Vertex soleplate = v[21];
	double length = umbilicus.z - soleplate.z;
	//cout<<"Leg length : "<<length<<endl;
	return length;
}

double BackLength(Vertex v[]){
	Vertex rightShoulder = v[10791];
	Vertex leftShoulder = v[10827];
	double length = sqrt(pow(rightShoulder.x - leftShoulder.x,2) + pow(rightShoulder.y - leftShoulder.y,2));
	//cout<<"Back length : "<<length<<endl;
	return length;
}

double UpperBodyLength(Vertex v[]){
	Vertex TopShoulder = v[10841];
	Vertex TopPant = v[6924];
	double length = TopShoulder.z - TopPant.z;
	//cout<<"Upper body length : "<<length<<endl;
	return length;
}