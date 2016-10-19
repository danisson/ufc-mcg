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
		std::tuple<Color,bool> intersect_box(const BoundingBox& root, const BoundingBox& test) const;
		Color intersect_point(const BoundingBox& root, const glm::vec3& x) const;
	};

	// Builds a tree from a file, stops reading until end of line
	tnw::owner_ptr<Tree> make_from_file(FILE* f);
	tnw::owner_ptr<Tree> tree_and(Tree* t1, Tree* t2);
	tnw::owner_ptr<Tree> tree_or(Tree* t1, Tree* t2);
	std::string serialize(Tree* t);
	tnw::owner_ptr<Tree> classify(const Shape& shape, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth);

	// Builds a tree from a file, stops reading until end of line
	tnw::owner_ptr<Tree> make_from_file(FILE* f);
} // namespace tnw::octree

	class Octree : public Model {
	public:
		//Octree com raiz vazia
		Octree(const BoundingBox& _bb);
		//Octree com raiz pronta
		Octree(std::unique_ptr<octree::Tree> tree, const BoundingBox& _bb);
		//Octree a partir de um forma e uma Bounding Box
		Octree(const Shape& c, const BoundingBox& _bb, unsigned int depth);
		//Octree a partir de um arquivo
		Octree(FILE *f);

		void setColor(float c[3]) override;

		// Geometric operations
		void translate(const glm::vec3& dv) override;
		void scale(const float dx) override;
		Color intersect_box(const BoundingBox&) const override;
		Color intersect_point(const glm::vec3&) const override;
		// Boolean operations
		BooleanErrorCodes bool_and(const Model& y) override;
		BooleanErrorCodes bool_or(const Model& y) override;
		// Geometric analysis
		double volume() const override;
		//Serialize
		std::string serialize() const override;

	private:
		std::unique_ptr<octree::Tree> tree;
		BoundingBox bb;
		void rdraw() const override;
	};

}
#endif
