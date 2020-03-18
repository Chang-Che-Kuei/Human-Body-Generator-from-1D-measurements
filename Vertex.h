#ifndef _VERTEX_H_
#define _VERTEX_H_

struct Vertex{
	double x,y,z;
};

struct EndPoint{
	double distance;
	int id;
	bool operator < (EndPoint right){
		return distance > right.distance;
	}
};
#endif