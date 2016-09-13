#include "model.h"
#include <vector>
using namespace tnw::octree;

tnw::octree::Tree::Tree(Tree* parent) {
	children.fill(nullptr);
	color = Color::black;
	this->parent = parent;
}

tnw::octree::Tree::Tree(std::array<Tree*,8> children, Tree* parent) {
	this->children = children;
	for (auto&& child : children) {
		child->parent = this;
	}
	color = Color::gray;
	this->parent = parent;
}

Tree*& tnw::octree::Tree::operator[](size_t i) {
	return children[i];
}

Tree tnw::octree::make_from_file(FILE* f) {
	char c;
	std::vector<int> counter{0};
	fgetc(f); // ignore first '('

	Tree root;
	root.color = Color::gray;

	Tree* cursor = &root;

	while ((c = std::fgetc(f)) != '\n' && c != EOF) {
		switch(c) {
			case '(': {
				(*cursor)[counter.back()] = new Tree(cursor);
				cursor = (*cursor)[counter.back()];
				cursor->color = Color::gray;
				counter.back()++;
				counter.push_back(0);
				break;
			}
			case 'b': {
				(*cursor)[counter.back()++] = new Tree(cursor);
				break;
			}
			case 'w': {
				(*cursor)[counter.back()++] = nullptr;
				break;
			}
		}

		if (counter.back() == 8) {
			counter.pop_back();
			cursor = cursor->parent;
		}
	}

	return root;
}

// Todo - Make recursive version using std::vector
std::string tnw::octree::Tree::serialize() const {
	std::string o;
	if (color == Color::gray) {
		o += "(";
		for (auto&& c : children) {
			if (c)
				o += c->serialize();
			else
				o += "w";
		}
	}
	if (color == Color::black) {
		o += "b";
	}

	return o;
}
