#include "model.h"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <array>

tnw::octree::BoundingBox::BoundingBox(glm::vec3 _corner, float _depth) : corner(_corner), depth(_depth){ }

// os Vértices são gerados pela seguinte ordem
//    7-------8
//   /|      /|
//  / |     / |
// 5--|----6  |
// |  3----|--4
// | /     | /
// 1-------2
void tnw::octree::BoundingBox::draw(){
	glm::vec3 x = glm::vec3(1.,0.,0.), y = glm::vec3(0.,1.,0.), z = glm::vec3(0.,0.,1.),
			  v1 = corner, 
			  v2 = v1 + depth*x, 
			  v3 = v1 - depth*z, 
			  v4 = v1 - depth*z + depth*x, 
			  v5 = v1 + depth*y, 
			  v6 = v1 + depth*y + depth*x, 
			  v7 = v1 + depth*y - depth*z, 
			  v8 = v1 + depth*y - depth*z + depth*x;
	
	std::array<glm::vec3, 4> f1 = {v1, v3, v4, v2},
							 f2 = {v1, v5, v7, v3},
							 f3 = {v2, v4, v8, v6},
							 f4 = {v1, v2, v6, v5},
							 f5 = {v3, v7, v8, v4},
							 f6 = {v5, v6, v8, v7};

	std::array<std::array<glm::vec3, 4>, 6> quads = {f1,f2,f3,f4,f5,f6};


	glBegin(GL_QUADS);
		glColor3f(0.,1.,1.);

		for (std::array<glm::vec3, 4>& face : quads) {
			for (glm::vec3& vertice : face) {
				glVertex3f(vertice.x, vertice.y, vertice.z);
			}
		}

	glEnd();
}

// A posição das sub-bounding boxes é de acordo com a ordem definida em sala

tnw::octree::BoundingBox tnw::octree::BoundingBox::getSubBox(int position){
	glm::vec3 corner;
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
		}

		case 3: {
			corner = this->corner - depth*z + depth*x;
		}
	}
}
