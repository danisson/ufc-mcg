#include "shapes.h"
#include "octree.h"
#include "helper.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

using tnw::Color;
using namespace tnw::octree;

// ------------------------------------------------------------------------- //
tnw::Sphere::Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {}

double tnw::Sphere::volume() const{
	return 4*radius*tnw::pi*tnw::pi;
}

Color tnw::Sphere::intersect_point(const glm::vec3& x) const {
	auto dis = glm::distance(x,center);

	if (dis  > radius) return tnw::Color::white;
	if (dis == radius) return tnw::Color::gray;
	return tnw::Color::black;
}

Color tnw::Sphere::intersect_box(const BoundingBox& bb) const{
	if (tnw::sphere_box_intersection(center, radius, bb.getCenter(), bb.depth, bb.depth, bb.depth)) {
		unsigned int count = 0;
		for (int i = 0; i < 8; ++i) {
			if (glm::distance(center, bb.getVertice(i)) < radius) {
				count++;
			}
		}
		if (count >= 8) {
			return tnw::Color::black;
		}
		else {
			return tnw::Color::gray;
		}
	} else {
		return tnw::Color::white;
	}
}

// ------------------------------------------------------------------------- //
tnw::Box::Box(glm::vec3 center, float length, float depth, float height) : center(center), length(length), depth(depth), height(height){}

double tnw::Box::volume() const{
	return length*depth*height;
}

Color tnw::Box::intersect_point(const glm::vec3& x) const {
	auto minX = center[0] - length/2.f,
	     minY = center[1] - height/2.f,
	     minZ = center[2] -  depth/2.f,
	     maxX = center[0] + length/2.f,
	     maxY = center[1] + height/2.f,
	     maxZ = center[2] +  depth/2.f;

	bool in = (minX < x[0]) && (minY < x[1]) && (minZ < x[2]) &&
	          (maxX > x[0]) && (maxY > x[1]) && (maxZ > x[2]);

	bool out= (minX > x[0]) || (minY > x[1]) || (minZ > x[2]) ||
	          (maxX < x[0]) || (maxY < x[1]) || (maxZ < x[2]);

	bool on = !in && !out;

	if (in ) return tnw::Color::black;
	if (on ) return tnw::Color::gray;
	return tnw::Color::white;
}

Color tnw::Box::intersect_box(const BoundingBox& bb) const{
	unsigned int count = 0;
	glm::vec3 p;

	//Bounding box intercepta a caixa
	if (tnw::box_intersection(bb.getCenter(), bb.depth, bb.depth, bb.depth, center, length, height, depth)){
		for (int i = 0; i < 8; ++i) {
			unsigned int countcoords = 0;
			p = bb.getVertice(i);

			if (p[0] >= center[0] - length/2.f && p[0] <= center[0] + length/2.f) {
				countcoords++;
			}
			if (p[1] >= center[1] - height/2.f && p[1] <= center[1] + height/2.f) {
				countcoords++;
			}
			if (p[2] >= center[2] - depth/2.f && p[2] <= center[2] + depth/2.f) {
				countcoords++;
			}
			if (countcoords >= 3) {
				count++;
			}
		}
		if (count >= 8){
			return tnw::Color::black;
		} else {
			return tnw::Color::gray;
		}
	} else {
		return tnw::Color::white;
	}

}

// ------------------------------------------------------------------------- //
tnw::Cilinder::Cilinder(glm::vec3 inferiorPoint, float height, float radius) : inferiorPoint(inferiorPoint), height(height), radius(radius) {}

double tnw::Cilinder::volume() const{
	return tnw::pi*radius*radius*height;
}

Color tnw::Cilinder::intersect_point(const glm::vec3& x) const{
	auto v = x - inferiorPoint;

	bool in = (v[1] < height) && (v[1] > 0) &&
	          ((v[0]*v[0]+v[2]*v[2]) < radius*radius);

	bool out= (v[1] > height) || (v[1] < 0) ||
	          ((v[0]*v[0]+v[2]*v[2]) > radius*radius);

	bool on = !in && !out;

	if (in ) return tnw::Color::black;
	if (on ) return tnw::Color::gray;
	return tnw::Color::white;
}

Color tnw::Cilinder::intersect_box(const BoundingBox& bb) const{
	unsigned int count = 0;
	glm::vec3 p, y(0,1,0);
	for (int i = 0; i < 8; ++i) {
		p = bb.getVertice(i);
		if ((p[1] >= inferiorPoint[1]) && (p[1] <= inferiorPoint[1]+height) && (glm::distance(p, inferiorPoint+(p[1]-inferiorPoint[1])*y) <= radius)){
			count++;
		}
	}

	if (count >= 8){
		return tnw::Color::black;
	} else {
		return tnw::Color::gray;
	}
}

// ------------------------------------------------------------------------- //
tnw::SquarePyramid::SquarePyramid(glm::vec3 inferiorPoint, float height, float basis) : inferiorPoint(inferiorPoint), height(height), basis(basis) {}

double tnw::SquarePyramid::volume() const{
	return basis*basis*height*1/2;
}

Color tnw::SquarePyramid::intersect_point(const glm::vec3& x) const{
	auto v = x - inferiorPoint;
	auto top_coord = inferiorPoint[1] + height;
	auto proportionalBasis = basis*(top_coord-x[1]) / height;

	bool in = (v[1] < height) && (v[1] > 0);

	bool out= (v[1] > height) || (v[1] < 0);


	auto minX = inferiorPoint[0] - proportionalBasis/2.f,
	     minZ = inferiorPoint[2] - proportionalBasis/2.f,
	     maxX = inferiorPoint[0] + proportionalBasis/2.f,
	     maxZ = inferiorPoint[2] + proportionalBasis/2.f;

	in = in  && (minX < x[0]) && (minZ < x[2]) &&
	            (maxX > x[0]) && (maxZ > x[2]);

	out= out || (minX > x[0]) || (minZ > x[2]) ||
	            (maxX < x[0]) || (maxZ < x[2]);

	bool on = !in && !out;

	if (in ) return tnw::Color::black;
	if (on ) return tnw::Color::gray;
	return tnw::Color::white;
}

Color tnw::SquarePyramid::intersect_box(const BoundingBox& bb) const{
	unsigned int count = 0;

	glm::vec3 pyramidCenter(inferiorPoint[0], inferiorPoint[1]+(height/2.0), inferiorPoint[2]);
	if (box_intersection(bb.getCenter(), bb.depth, bb.depth, bb.depth, pyramidCenter, basis, 2.0*height, basis)) {
		glm::vec3 p;
		double top_coord;
		for (int i = 0; i < 8; ++i) {
			bool xPos, yPos, zPos;
			float proportionalBasis;

			p = bb.getVertice(i);
			top_coord = inferiorPoint[1] + height;
			proportionalBasis = basis*(top_coord-p[1]) / height;
			xPos = (p[0] >= inferiorPoint[0]-proportionalBasis/2.0) && (p[0] <= inferiorPoint[0]+proportionalBasis/2.0);
			yPos = (p[1] >= inferiorPoint[1]) && (p[1] <= inferiorPoint[1]+height);
			zPos = (p[2] >= inferiorPoint[2]-proportionalBasis/2.0) && (p[2] <= inferiorPoint[2]+proportionalBasis/2.0);
			if (yPos && xPos && zPos){
				count++;
			}
		}
		if (count >= 8){
			return tnw::Color::black;
		} else {
			return tnw::Color::gray;
		}
	}
	return tnw::Color::white;
}
// ------------------------------------------------------------------------- //
