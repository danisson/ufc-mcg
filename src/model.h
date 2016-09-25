#ifndef _H_MODEL
#define _H_MODEL
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <functional>
#include "octree.h"

namespace tnw {

class Model {
public:
	virtual void draw() = 0;
	// Geometric operations
	virtual void translate(const glm::vec3& dv) = 0;
	// Boolean operations
	virtual void bool_and(const Model& y) = 0;
	// Geometric analysis
	virtual double volume() = 0;
};

class Octree : public Model {};

} // namespace tnw
#endif
