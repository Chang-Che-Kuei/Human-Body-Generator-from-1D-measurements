#ifndef _CHEST_H_
#define _CHEST_H_

#include "Vertex.h"
#define N_Vertex 12500
#define GraphSizeChest 2000
#define N_BaseChest 24

class Chest{
	public:
		Chest();
		void SetChestBaseV(Vertex v[]);
		void SetChestVertexMapping(Vertex v[]);
		void SetGraph(int face[][3], Vertex v[]);
		void DisConnectLeft(Vertex v[]);
		void ChestDijkstra(Vertex v[]);
		~Chest();
		double length;

	private:
		int ChestBaseID,baseChest[N_BaseChest];
		bool underArm[N_Vertex];
		Vertex ChestBaseV;

		int Map2Chest[N_Vertex], Map2Ori[N_Vertex];
		int nChest;

		double** dis; // 2D array
		

		int FindEndVertex(double d[], Vertex v[]);
};

#endif