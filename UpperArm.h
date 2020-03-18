#ifndef _Arm_H_
#define _Arm_H_

#include "Vertex.h"
#define N_Vertex 12500
#define GraphSize 1500

class Arm{
	public:
		Arm();
		void SetArmVertexMapping(Vertex v[]);
		void SetGraph(int face[][3], Vertex v[]);
		void DisConnectLeft(Vertex v[]);
		void ArmDijkstra(Vertex v[]);
		~Arm();

		double length;

	private:
		int ArmBaseID;
		bool underArm[N_Vertex];
		Vertex ArmBaseV;

		int Map2Arm[N_Vertex], Map2Ori[N_Vertex];
		int nArm;

		double** dis; // 2D array
		

		int FindEndVertex(double d[], Vertex v[]);
};

#endif