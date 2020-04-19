#ifndef _LowerChest_H_
#define _LowerChest_H_

#include "Vertex.h"
#define N_Vertex 12500
#define GraphSizeLowerChest 3000


class LowerChest{
	public:
		LowerChest();
		void SetLowerChestBaseV(Vertex v[]);
		void SetLowerChestVertexMapping(Vertex v[]);
		void SetGraph(int face[][3], Vertex v[]);
		void DisConnectLeft(Vertex v[]);
		void LowerChestDijkstra(Vertex v[]);
		~LowerChest();
		double length;

	private:
		int LowerChestBaseID;
		bool underArm[N_Vertex];
		Vertex LowerChestBaseV;

		int Map2LowerChest[N_Vertex], Map2Ori[N_Vertex];
		int nLowerChest;

		double** dis; // 2D array
		

		int FindEndVertex(double d[], Vertex v[]);
};

#endif