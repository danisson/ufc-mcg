#ifndef _H_OCTREE
#define _H_OCTREE
#include "model.h"
#include <memory>

namespace tnw {
	template <typename T>
	using owner_ptr = T*;
namespace octree {
	enum class Color {
		white, black, gray
	};

	struct BoundingBox {
		glm::vec3 corner;
		float depth;
		BoundingBox(glm::vec3 _corner, float _depth);
		void draw() const;
		BoundingBox operator[](size_t position) const;
		glm::vec3 getVertice(unsigned int i) const;
		glm::vec3 getCenter() const;
	};

	using Classifier = std::function<Color(const BoundingBox&)>;
	using std::unique_ptr;
	using std::array;

	struct Tree {
		Color color;
		array<unique_ptr<Tree>,8> children;
		Tree* parent;
		float drawColor[3];

		// Constructor for intermediate nodes
		Tree(array<unique_ptr<Tree>,8> children, Tree* parent = nullptr);
		// Constructor for leafs
		Tree(Tree* parent = nullptr);
		//Drawing function
		void draw(const BoundingBox& bb);

		std::string serialize() const;
		void classify(Classifier function, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth);
	};

	class Sphere {
	public:
		Sphere(glm::vec3 center, float radius);
		Color operator()(const BoundingBox&);
	private:
		glm::vec3 center;
		float radius;
	};

	class Box {
	public:
		Box(glm::vec3 center, float length, float depth, float height);
		Color operator()(const BoundingBox&);
	private:
		glm::vec3 center;
		float length, depth, height;
	};

	class Cilinder {
	public:
		Cilinder(glm::vec3 inferiorPoint, float height, float radius);
		Color operator()(const BoundingBox&);
	private:
		glm::vec3 inferiorPoint;
		float height, radius;
	};

	class SquarePyramid {
	public:
		SquarePyramid(glm::vec3 inferiorPoint, float height, float basis);
		Color operator()(const BoundingBox&);
	private:
		glm::vec3 inferiorPoint;
		float basis, radius;
	};

	// Builds a tree from a file, stops reading until end of line
	Tree make_from_file(FILE* f);
}} // namespace tnw::octree
#endif
