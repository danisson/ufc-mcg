#include "model.h"
#include <iostream>
using namespace tnw;

Ray::Ray(glm::vec3 _a, glm::vec3 _b) : a(_a), b(_b), dir(_b-_a) {
	invdir = glm::vec3(1,1,1)/dir;
}

float tnw::Ray::length() const {
	return glm::length(b-a);
}

void tnw::Ray::transform(const glm::mat4 &transform) {
	a = glm::vec3(transform * glm::vec4(a.x, a.y, a.z, 1.0));
	b = glm::vec3(transform * glm::vec4(b.x, b.y, b.z, 1.0));
	dir = b-a;
	invdir = glm::vec3(1,1,1)/dir;
}

tnw::Ray tnw::Ray::getTransformedRay(const glm::mat4 &transform) const {
	glm::vec3 aT = glm::vec3(transform * glm::vec4(a.x, a.y, a.z, 1.0)),
			  bT = glm::vec3(transform * glm::vec4(b.x, b.y, b.z, 1.0));
	return tnw::Ray(aT, bT); 
}
