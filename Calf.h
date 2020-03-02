#ifndef _CALF_H_
#define _CALF_H_

#include "Vertex.h"
#define N_Vertex 12500
#define GraphSize 1500
#define CalfBaseID 2122

class Calf{
	public:
		Calf();
		void SetCalfVertexMapping(Vertex v[]);
		void SetGraph(int face[][3], Vertex v[]);
		void DisConnectLeft(Vertex v[]);
		void CalfDijkstra(Vertex v[]);

	private:
		Vertex CalfBaseV;

		int Map2Calf[N_Vertex], Map2Ori[N_Vertex];
		int nCalf;

		double** dis; // 2D array
		double length;
};

#endif