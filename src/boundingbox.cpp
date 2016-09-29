#include "model.h"
#include "helper.h"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <array>

tnw::octree::BoundingBox::BoundingBox(glm::vec3 _corner, float _depth) : corner(_corner), depth(_depth){}

bool tnw::octree::BoundingBox::operator==(const tnw::octree::BoundingBox& y) const{
	return (depth == y.depth) && (corner == y.corner);
}

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
//Retorna o centro da bounding box
glm::vec3 tnw::octree::BoundingBox::getCenter() const {
	glm::vec3 x = glm::vec3(1.,0.,0.), y = glm::vec3(0.,1.,0.), z = glm::vec3(0.,0.,1.);
	float depth = this->depth/2;
	return this->corner + depth*(x+y-z);
}

//Retorna o menor ponto da box (vértice 2?)
glm::vec3 tnw::octree::BoundingBox::minPoint() const {
	return getVertice(2);
}

//Retorna o maior ponto da box (vértice 5?)
glm::vec3 tnw::octree::BoundingBox::maxPoint() const {
	return getVertice(5);
}

//Retorna cada canto da bouding box
glm::vec3 tnw::octree::BoundingBox::getVertice(unsigned int i) const {
	glm::vec3 corner, x = glm::vec3(1.,0.,0.), y = glm::vec3(0.,1.,0.), z = glm::vec3(0.,0.,1.);
	float depth = this->depth;

	switch(i){
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

	return corner;
}
// A posição das sub-bounding boxes é de acordo com a ordem definida em sala
tnw::octree::BoundingBox tnw::octree::BoundingBox::operator[](size_t position) const{
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

double tnw::octree::BoundingBox::volume() const {
	return depth*depth*depth;
}

tnw::octree::Color tnw::octree::BoundingBox::intersect(const BoundingBox& bb) const {
	size_t count = 0;
	auto center = getCenter();
	glm::vec3 p;

	//Bounding box intercepta a caixa
	if (tnw::box_intersection(bb.getCenter(), bb.depth, bb.depth, bb.depth, center, depth, depth, depth)) {
		for (int i = 0; i < 8; ++i) {
			size_t countcoords = 0;
			p = bb.getVertice(i);

			for (size_t i = 0; i < 3; ++i) {
				if (p[i] >= center[i] - depth/2.f &&
					p[i] <= center[i] + depth/2.f)
					countcoords++;
			}

			if (countcoords >= 3) count++;
		}
		if (count >= 8) return tnw::octree::Color::black;
		else return tnw::octree::Color::gray;

	}
	else return tnw::octree::Color::white;
}
