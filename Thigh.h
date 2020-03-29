#ifndef _THIGH_H_
#define _THIGH_H_

#include "Vertex.h"
#define N_Vertex 12500
#define GraphSize 1500
#define ThighBaseID 3669

class Thigh{
	public:
		Thigh();
		void SetThighVertexMapping(Vertex v[]);
		void SetGraph(int face[][3], Vertex v[]);
		void DisConnectLeft(Vertex v[]);
		void ThighDijkstra(Vertex v[]);
		~Thigh();

		double length;

	private:
		bool crotch[N_Vertex];
		Vertex ThighBaseV;

		int Map2Thigh[N_Vertex], Map2Ori[N_Vertex];
		int nThigh;

		double** dis; // 2D array

		int FindEndVertex(double d[], Vertex v[]);
};

#endif