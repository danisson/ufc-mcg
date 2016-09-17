#ifndef _H_MODEL
#define _H_MODEL
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <functional>

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
		white, black, gray
	};

	struct BoundingBox
	{
		glm::vec3 corner;
		float depth;
		BoundingBox(glm::vec3 _corner, float _depth);
		void draw() const;
		BoundingBox operator[](size_t position) const;
		glm::vec3 getVertice(unsigned int i) const;
	};

	using Classifier = std::function<Color(const BoundingBox&)>;

	struct Tree {
		Color color;
		std::array<Tree*,8> children;
		Tree* parent;

		// Constructor for intermediate nodes
		Tree(std::array<Tree*,8> children, Tree* parent = nullptr);
		// Constructor for leafs
		Tree(Tree* parent = nullptr);
		//Drawing function
		void draw(const BoundingBox& bb);

		Tree*& operator[](size_t i);

		std::string serialize() const;
		void classify(Classifier function, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth);
	};

	// Builds a tree from a file, stops reading until end of line
	Tree make_from_file(FILE* f);
} // namespace tnw::octree

class Octree : public Model { };

} // namespace tnw
#endif
