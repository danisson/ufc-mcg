#include "model.h"
#include "helper.h"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <array>

tnw::BoundingBox::BoundingBox(glm::vec3 _corner, float _depth) : corner(_corner), depth(_depth){}

bool tnw::BoundingBox::operator==(const tnw::BoundingBox& y) const{
	return (depth == y.depth) && (corner == y.corner);
}

bool tnw::BoundingBox::operator!=(const tnw::BoundingBox& y) const {
	return (depth != y.depth) || (corner != y.corner);
}

// os Vértices são gerados pela seguinte ordem
//      y
//      |                       v1
//      4------5           .------.
//     /|      |          /5555555|
//    / |     /|         /5555555/|
//   6--|----7 |        .-------.2|
//   |  0----|-1---x  3>|4444444|2.
//   | /     |/         |4444444|/
//   2-------3          .-------.
//  /                        ^0
// z
void tnw::BoundingBox::draw() const{
	glm::vec3 x = {1.,0.,0.}, y = {0.,1.,0.}, z = {0.,0.,1.},
	          v0 = corner,
	          v1 = v0 + depth*x,
	          v2 = v0 + depth*z,
	          v3 = v0 + depth*z + depth*x,
	          v4 = v0 + depth*y,
	          v5 = v0 + depth*y + depth*x,
	          v6 = v0 + depth*y + depth*z,
	          v7 = v0 + depth*y + depth*z + depth*x;

	std::array<glm::vec3, 4> f0 = {v2, v0, v1, v3},
	                         f1 = {v0, v4, v5, v1},
	                         f2 = {v1, v5, v7, v3},
	                         f3 = {v2, v6, v4, v0},
	                         f4 = {v7, v6, v2, v3},
	                         f5 = {v5, v4, v6, v7};

	std::array<std::array<glm::vec3, 4>, 6> quads = {f0,f1,f2,f3,f4,f5};



	glBegin(GL_QUADS);

		for (auto&& face : quads) {
			for (auto&& vertice : face) {
				glVertex3f(vertice.x, vertice.y, vertice.z);
			}
		}

	glEnd();
}

void tnw::BoundingBox::translate(const glm::vec3& dv) {
	corner += dv;
}

void tnw::BoundingBox::scale(const float dx) {
	corner *= dx;
	depth  *= dx;
}

//Retorna o centro da bounding box
glm::vec3 tnw::BoundingBox::getCenter() const {
	glm::vec3 x = glm::vec3(1.,0.,0.),
	          y = glm::vec3(0.,1.,0.),
	          z = glm::vec3(0.,0.,1.);
	float depth = this->depth/2;
	return this->corner + depth*(x+y+z);
}

//Retorna o menor ponto da box (vértice 0?)
glm::vec3 tnw::BoundingBox::minPoint() const {
	return getVertice(0);
}

//Retorna o maior ponto da box (vértice 7?)
glm::vec3 tnw::BoundingBox::maxPoint() const {
	return getVertice(7);
}

//Retorna cada canto da bouding box
glm::vec3 tnw::BoundingBox::getVertice(unsigned int i) const {
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
			corner = this->corner + depth*z;
			break;
		}

		case 3: {
			corner = this->corner + depth*z + depth*x;
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
			corner = this->corner + depth*y + depth*z;
			break;
		}

		case 7: {
			corner = this->corner + depth*y + depth*z + depth*x;
		}
	}

	return corner;
}
// A posição das sub-bounding boxes é de acordo com a ordem definida em sala
tnw::BoundingBox tnw::BoundingBox::operator[](size_t position) const{
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
			corner = this->corner + depth*z;
			break;
		}

		case 3: {
			corner = this->corner + depth*z + depth*x;
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
			corner = this->corner + depth*y + depth*z;
			break;
		}

		case 7: {
			corner = this->corner + depth*y + depth*z + depth*x;
		}
	}

	return tnw::BoundingBox(corner, depth);
}

double tnw::BoundingBox::volume() const {
	return depth*depth*depth;
}

tnw::Color tnw::BoundingBox::intersect_box(const BoundingBox& bb) const {
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
		if (count >= 8) return tnw::Color::black;
		else return tnw::Color::gray;

	}
	else return tnw::Color::white;
}

tnw::Color tnw::BoundingBox::intersect_point(const glm::vec3& x) const {
	auto center = getCenter();
	auto minX = center[0] - depth/2.f,
	     minY = center[1] - depth/2.f,
	     minZ = center[2] - depth/2.f,
	     maxX = center[0] + depth/2.f,
	     maxY = center[1] + depth/2.f,
	     maxZ = center[2] + depth/2.f;

	bool in = (minX < x[0]) && (minY < x[1]) && (minZ < x[2]) &&
	          (maxX > x[0]) && (maxY > x[1]) && (maxZ > x[2]);

	bool out= (minX > x[0]) || (minY > x[1]) || (minZ > x[2]) ||
	          (maxX < x[0]) || (maxY < x[1]) || (maxZ < x[2]);

	bool on = !in && !out;

	if (in ) return tnw::Color::black;
	if (on ) return tnw::Color::gray;
	return tnw::Color::white;
}

tnw::IntersectionList tnw::BoundingBox::intersect_ray(const tnw::Ray&) const {
	return tnw::IntersectionList();
}

tnw::BoundingBox tnw::BoundingBox::least_boundingbox(const tnw::BoundingBox& bb) const {

	using std::min;
	glm::vec3 mn = minPoint();
	glm::vec3 mnb = bb.minPoint();

	mn = {min(mn[0],mnb[0]),min(mn[1],mnb[1]),min(mn[2],mnb[2])};

	using std::max;
	glm::vec3 mx = maxPoint();
	glm::vec3 mxb = bb.maxPoint();
	mx = {max(mx[0],mxb[0]),max(mx[1],mxb[1]),max(mx[2],mxb[2])};

	glm::vec3 dp = mx-mn;
	float d = 0;
	d = std::max(d,dp[0]);
	d = std::max(d,dp[1]);
	d = std::max(d,dp[2]);

	return tnw::BoundingBox(mn,d);
}

tnw::BoundingBox tnw::BoundingBox::boundingBox() const {
	return *this;
}
