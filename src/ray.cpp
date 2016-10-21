#include "model.h"
#include <iostream>
using namespace tnw;

Ray::Ray(glm::vec3 _a, glm::vec3 _b) : a(_a), b(_b), dir(_b-_a) {
	invdir = glm::vec3(1,1,1)/dir;
}

float tnw::Ray::length() const {
	std::cout << "b: " << b.x << " " << b.y << " " << b.z << "\n";
	std::cout << "a: " << a.x << " " << a.y << " " << a.z << "\n";
	std::cout << "b-a: " << (b-a).x << " " << (b-a).y << " " << (b-a).z << "\n";
	return glm::length(b-a);
}