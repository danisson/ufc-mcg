#include "model.h"
#include "octree.h"
#include <typeindex>
#include <typeinfo>

using namespace tnw;
using namespace tnw::octree;

//Octree com raiz vazia
tnw::Octree::Octree(const BoundingBox& _bb) : bb(_bb) {}
//Octree com raiz pronta
tnw::Octree::Octree(std::unique_ptr<Tree> tree, const BoundingBox& _bb) : bb(_bb) {}
//Octree a partir de um classificador e uma Bounding Box
tnw::Octree::Octree(Classifier c, const BoundingBox& _bb, unsigned int depth) : bb(_bb) {
	tree = std::unique_ptr<Tree>(octree::classify(c, _bb, depth, 0));
}
//Octree a partir de um arquivo
tnw::Octree::Octree(FILE *f) : bb(glm::vec3(), 1) {}

void tnw::Octree::draw() const{
	if (tree) {
		tree->draw(bb);
	}
}
// Geometric operations
void tnw::Octree::translate(const glm::vec3& dv) {
	if (tree) return;
}
// Boolean operations
void tnw::Octree::bool_and(const Model& y) {
	if (std::type_index(typeid(y)) == std::type_index(typeid(Octree))) {
		auto&& y2 = static_cast<const Octree&>(y);
		tree = std::unique_ptr<Tree>(octree::tree_and(tree.get(), y2.tree.get()));
	}
}
// Geometric analysis
double tnw::Octree::volume() const{
	if (tree) {
		return bb.volume() * tree->volume();
	} else {
		return 0;
	}
}
//Serialize
std::string tnw::Octree::serialize() const {
	return octree::serialize(tree.get());
}
