#include "model.h"
#include "octree_internal.h"
#include <typeindex>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <GL/gl.h>

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
tnw::Octree::Octree(FILE *f) : bb(glm::vec3(), 1) {
	float x,y,z,d;
	fscanf(f,"%f,%f,%f,%f ",&x,&y,&z,&d);
	bb = BoundingBox({x,y,z},d);
	tree = unique_ptr<Tree>(make_from_file(f));
}

void tnw::Octree::rdraw() const {
	//Desenha a bounding box
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glColor3f(0,0,0);
	bb.draw();
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	//Desenha a árvore
	if (tree) {
		tree->draw(bb);
	}
}
// Geometric operations
void tnw::Octree::translate(const glm::vec3& dv) {
	bb.corner += dv;
}
void tnw::Octree::scale(const float dx) {
	bb.depth *= dx;
}
// Boolean operations
tnw::BooleanErrorCodes tnw::Octree::bool_and(const Model& y) {
	if (std::type_index(typeid(y)) == std::type_index(typeid(Octree))) {
		auto&& y_oct = static_cast<const Octree&>(y);
		if (bb != y_oct.bb) { return tnw::BooleanErrorCodes::boundingboxMismatch;}
		tree = std::unique_ptr<Tree>(octree::tree_and(tree.get(), y_oct.tree.get()));
		return tnw::BooleanErrorCodes::success;
	} else return tnw::BooleanErrorCodes::unimplementedType;
}
tnw::BooleanErrorCodes tnw::Octree::bool_or(const Model& y) {
	if (std::type_index(typeid(y)) == std::type_index(typeid(Octree))) {
		auto&& y_oct = static_cast<const Octree&>(y);
		if (bb != y_oct.bb) { return tnw::BooleanErrorCodes::boundingboxMismatch;}
		tree = std::unique_ptr<Tree>(octree::tree_or(tree.get(), y_oct.tree.get()));
		return tnw::BooleanErrorCodes::success;
	} else return tnw::BooleanErrorCodes::unimplementedType;
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
	std::stringstream ss;
	ss << bb.corner[0] << ',';
	ss << bb.corner[1] << ',';
	ss << bb.corner[2] << ',';
	ss << bb.depth << ' ';
	ss << octree::serialize(tree.get());
	return ss.str();
}

//Set color
void tnw::Octree::setColor(float c[3]){
	if (tree) {
		tree->setColor(c);
	}
}
