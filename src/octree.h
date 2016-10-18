#ifndef _H_OCTREE
#define _H_OCTREE
#include "model.h"
#include <memory>
#include <tuple>

namespace tnw {
namespace octree {
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
		void setColor(float c[3]);

		void set(size_t i, unique_ptr<Tree>&& t);
		Tree* get(size_t);

		double volume() const;
		std::tuple<Color,bool> classify(const BoundingBox& root, const BoundingBox& test) const;
	};

	// Builds a tree from a file, stops reading until end of line
	tnw::owner_ptr<Tree> make_from_file(FILE* f);
	tnw::owner_ptr<Tree> tree_and(Tree* t1, Tree* t2);
	tnw::owner_ptr<Tree> tree_or(Tree* t1, Tree* t2);
	std::string serialize(Tree* t);
	tnw::owner_ptr<Tree> classify(const Classifier& function, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth);

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
} // namespace tnw::octree

	class Octree : public Model {
	public:
		//Octree com raiz vazia
		Octree(const BoundingBox& _bb);
		//Octree com raiz pronta
		Octree(std::unique_ptr<octree::Tree> tree, const BoundingBox& _bb);
		//Octree a partir de um classificador e uma Bounding Box
		Octree(octree::Classifier c, const BoundingBox& _bb, unsigned int depth);
		//Octree a partir de um arquivo
		Octree(FILE *f);

		virtual void setColor(float c[3]) override;

		// Geometric operations
		virtual void translate(const glm::vec3& dv) override;
		virtual void scale(const float dx) override;
		virtual Color operator()(const BoundingBox&) const override;
		// Boolean operations
		virtual BooleanErrorCodes bool_and(const Model& y) override;
		virtual BooleanErrorCodes bool_or(const Model& y) override;
		// Geometric analysis
		virtual double volume() const override;
		//Serialize
		virtual std::string serialize() const override;

	private:
		std::unique_ptr<octree::Tree> tree;
		BoundingBox bb;
		virtual void rdraw() const override;
	};

}
#endif
