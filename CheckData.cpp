#include <iostream>
#include <stdio.h>
#include <string.h>
#define Gender 2
using namespace std;

void DeleteFile(){
	FILE *fp = fopen("RemoveName.txt","r");
	char file[50];
	for(int i=0;i<15;++i){
		fscanf(fp,"%s",file);
		string path = "female/";
		path += file;
		cout<<path<<endl;
		remove(path.c_str());
	}
}

int main(){
	//DeleteFile();

	
	char fileName[2][30]={"male.txt","female.txt"};
	int number = 0;
	for(int i=1;i<=1;++i){
		FILE *fp = fopen(fileName[i],"r");
		if(fp==NULL){
			cout<<"Cannot open "<<fileName[i]<<endl;
			continue;
		}

		char objName[30];
		double height, chest, lowerChest, waist, hip, thigh, calf, armL, legL, backL, shoulderH, chestH, 
		hipH, kneeH, hipDepth;
		while(fscanf(fp,"%s",objName)!=EOF){
			fscanf(fp,"%lf %lf %lf %lf %lf   %lf %lf %lf %lf %lf    %lf %lf %lf %lf %lf",
				&height, &chest, &lowerChest, &waist, &hip, &thigh, &calf, &armL, &legL, &backL,
				&shoulderH, &chestH, &hipH, &kneeH, &hipDepth);

			bool normal = true;
			//if(chest*1.3<waist)normal = false;
			//if(hip>thigh*3)normal = false;
			//if(hip<thigh*1.8)normal = false; // thigh too fat so the mesh connected the both thighs
			//if(thigh*0.9<calf)normal = false;
			if(chest<lowerChest)normal=false;
			if(normal==false){
				cout<<objName<<endl;
				number++;
			}
		}
	}
	cout<<"There are "<<number <<" suspecious data\n";
}