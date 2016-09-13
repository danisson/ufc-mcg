#include "model.h"

using namespace tnw;

octree::Tree() {
	children.fill(nullptr);
	color = Color.black;
}

octree::Tree(std::array<Tree*,8> children) {
	this->children = children;
	color = Color.gray;
}
