#include "model.h"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <array>

tnw::octree::BoundingBox::BoundingBox(glm::vec3 _corner, float _depth) : corner(_corner), depth(_depth){ }

// os Vértices são gerados pela seguinte ordem
//    6-------7
//   /|      /|
//  / |     / |
// 4--|----5  |
// |  2----|--3
// | /     | /
// 0-------1
void tnw::octree::BoundingBox::draw() const{
	glm::vec3 x = glm::vec3(1.,0.,0.), y = glm::vec3(0.,1.,0.), z = glm::vec3(0.,0.,1.),
			  v0 = corner, 
			  v1 = v0 + depth*x, 
			  v2 = v0 - depth*z, 
			  v3 = v0 - depth*z + depth*x, 
			  v4 = v0 + depth*y, 
			  v5 = v0 + depth*y + depth*x, 
			  v6 = v0 + depth*y - depth*z, 
			  v7 = v0 + depth*y - depth*z + depth*x;
	
	std::array<glm::vec3, 4> f0 = {v0, v2, v3, v1},
							 f1 = {v0, v4, v6, v2},
							 f2 = {v1, v3, v7, v5},
							 f3 = {v0, v1, v5, v4},
							 f4 = {v2, v6, v7, v3},
							 f5 = {v4, v5, v7, v6};

	std::array<std::array<glm::vec3, 4>, 6> quads = {f0,f1,f2,f3,f4,f5};


	glBegin(GL_QUADS);
		
		for (std::array<glm::vec3, 4>& face : quads) {
			for (glm::vec3& vertice : face) {
				glVertex3f(vertice.x, vertice.y, vertice.z);
			}
		}

	glEnd();
}

// A posição das sub-bounding boxes é de acordo com a ordem definida em sala

tnw::octree::BoundingBox tnw::octree::BoundingBox::operator[](int position) const{
	glm::vec3 corner, x = glm::vec3(1.,0.,0.), y = glm::vec3(0.,1.,0.), z = glm::vec3(0.,0.,1.);
	float depth = this->depth/2;

	switch(position){
		case 0: {
			corner = this->corner;
			break;
		}
		case 1: {
			corner = this->corner + depth*x;
			break;
		}
		case 2: {
			corner = this->corner - depth*z;
			break;
		}

		case 3: {
			corner = this->corner - depth*z + depth*x;
			break;
		}

		case 4: {
			corner = this->corner + depth*y;
			break;
		}

		case 5: {
			corner = this->corner + depth*y + depth*x;
			break;
		}

		case 6: {
			corner = this->corner + depth*y - depth*z;
			break;
		}

		case 7: {
			corner = this->corner + depth*y - depth*z + depth*x;
		}
	}

	return tnw::octree::BoundingBox(corner, depth);
}
