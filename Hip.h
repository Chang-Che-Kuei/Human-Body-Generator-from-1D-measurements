#ifndef _HIP_H_
#define _HIP_H_

#include "Vertex.h"
#define N_HipVertex 37
#define N_Vertex 12500
#define GraphSize 1500


class Hip{
	public:
		Hip();
		void SetHipBaseV(Vertex v[]);
		void SetHipVertexMapping(Vertex v[]);
		void SetGraph(int face[][3], Vertex v[]);
		void DisConnectLeft(Vertex v[]);
		void HipDijkstra(Vertex v[]);
		~Hip();
		double length;

	private:
		Vertex HipBaseV;
		int baseID;

		int vID[N_HipVertex];
		int Map2Hip[N_Vertex], Map2Ori[N_Vertex];
		int nHip;

		double** dis; // 2D array
		

		int FindEndVertex(double d[], Vertex v[]);
};


#endif