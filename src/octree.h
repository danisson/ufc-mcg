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
		glm::vec3 minPoint() const;
		glm::vec3 maxPoint() const;
		double volume() const;
		Color intersect(const BoundingBox& bb) const;
		bool operator==(const BoundingBox& s) const;
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
		Tree(array<unique_ptr<Tree>,8>&& children, Tree* parent = nullptr);
		// Constructor for leafs
		Tree(Tree* parent = nullptr);
		// Copy
		Tree(const Tree& o);

		//Drawing function
		void draw(const BoundingBox& bb);

		void set(size_t i, unique_ptr<Tree>&& t);
		Tree* get(size_t);

		double volume() const;
		Color classify(const BoundingBox& root, const BoundingBox& test) const;
	};

	// Builds a tree from a file, stops reading until end of line
	tnw::owner_ptr<Tree> make_from_file(FILE* f);
	tnw::owner_ptr<Tree> tree_and(Tree* t1, Tree* t2);
	std::string serialize(Tree* t);
	tnw::owner_ptr<Tree> classify(Classifier function, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth);

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
		float height, basis;
	};

	// Builds a tree from a file, stops reading until end of line
	tnw::owner_ptr<Tree> make_from_file(FILE* f);
}} // namespace tnw::octree
#endif
