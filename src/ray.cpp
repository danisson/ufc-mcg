#include "model.h"

using namespace tnw;

Ray::Ray(glm::vec3 _a, glm::vec3 _b) : a(_a), b(_b), dir(_b-_a) {
	invdir = glm::vec3(1,1,1)/dir;
}
