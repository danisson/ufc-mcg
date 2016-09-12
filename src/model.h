#ifndef _H_MODEL
#define _H_MODEL
#include <glm/glm.hpp>
#include <array.h>

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

namespace octree {
	enum class Color {
		white, black, gray;
	};

	struct Tree {
		Color color;
		std::array<Tree*,8> children;
	};
	struct BoundingBox;

	using Classifier = std::function<Color(const BoundingBox&)>;
} // namespace tnw::octree

class Octree : public Model;

} // namespace tnw
#endif
