#ifndef _OTHERMEASURE_H_
#define _OTHERMEASURE_H_
#include "Vertex.h"

void OtherMeasure(Vertex v[], double otherM[]);
double ArmLength(Vertex v[]);
double LegLength(Vertex v[]);
double BackLength(Vertex v[]);
double UpperBodyLength(Vertex v[]);
double ShoulderHeight(Vertex v[]);
double ChestHeight(Vertex v[]);
double ChestDepth(Vertex v[]);
double HipHeight(Vertex v[]);
double KneeHeight(Vertex v[]);
double HipDepth(Vertex v[]);


#endif