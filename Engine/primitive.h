#include "mesh.h"
#include <vector>


class Primitive {

public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Primitive() {
		
	}

};