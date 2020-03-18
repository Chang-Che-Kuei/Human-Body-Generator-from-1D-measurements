#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <set>
# define N_Vertex 12500
# define N_VN     12500
# define N_FACE   25000
# define WaistBasePoint 7294
# define DisSize 1000
# define NONE -1
# define WaistArea 0.025
using namespace std;

struct Vertex{
	double x,y,z;
	int id;

	bool operator < (Vertex right){
		return y<right.y;
	}
};

int main(){
	string a = "123";
	a+="456";
	cout<<a;
}