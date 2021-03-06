#include "model.h"
#include "octree.h"
#include "helper.h"
#include <vector>
#include <GL/gl.h>
#include <iostream>
#include <memory>
#include <random>

//Implementação da classe tree!

using tnw::Color;
using namespace tnw::octree;
using tnw::owner_ptr;
using std::make_unique;

tnw::octree::Tree::Tree(Tree* parent) {
	color = Color::black;
	this->parent = parent;

	std::random_device r;
	std::default_random_engine gen(r());

	std::uniform_real_distribution<> dis(0,1);

	for (int i = 0; i < 3; ++i) {
		drawColor[i] = dis(gen);
	}
}

tnw::octree::Tree::Tree(array<unique_ptr<Tree>,8>&& children, Tree* parent) {
	this->children = std::move(children);
	for (auto&& child : children) {
		if (child) {
			child->parent = this;
		}
	}
	color = Color::gray;
	this->parent = parent;

	std::random_device r;
	std::default_random_engine gen(r());

	std::uniform_real_distribution<> dis(0,1);

	for (int i = 0; i < 3; ++i) {
		drawColor[i] = dis(gen);
	}
}

tnw::octree::Tree::Tree(const Tree& o) {
	parent = nullptr;
	color = o.color;
	for (size_t i = 0; i < 3; ++i) {
		drawColor[i] = o.drawColor[i];
	}
	for (size_t i = 0; i < 8; ++i) {
		if (o.children[i]) {
			children[i] = make_unique<Tree>(*o.children[i]);
			children[i]->parent = this;
		}
	}
}

void tnw::octree::Tree::set(size_t i, unique_ptr<Tree>&& t) {
	children[i] = std::move(t);
}

Tree* tnw::octree::Tree::get(size_t i) {
	return children[i].get();
}


using std::tuple;
using std::make_tuple;
using std::tie;
using std::get;

Color tnw::octree::Tree::intersect_point(const BoundingBox& root, const glm::vec3& x) const {
	if (color == Color::black) return root.intersect_point(x);

	size_t count = 0;
	for (size_t i = 0; i < 8; ++i) {
		if (children[i]) {
			auto cC = children[i]->intersect_point(root[i],x);
			switch(cC) {
				case Color::black: return cC;
				case Color::gray :
					count++;
					if (count > 1)
						return Color::black;
					else break;
				case Color::white: break;
			}
		}
	}

	if (count == 1) return Color::gray;
	return Color::white;
}

tuple<Color,bool> tnw::octree::Tree::intersect_box(const BoundingBox& root, const BoundingBox& test) const {
	if (root == test) return make_tuple(color,color != Color::black);
	switch (root.intersect_box(test)) {
		case Color::black: {
			if (color == Color::black) return make_tuple(Color::black,false);

			int count = 0;
			bool whiteIn = false;

			Color cC;
			bool cB;

			for (size_t i = 0; i < 8; ++i) {
				if (children[i]) {
					tie(cC,cB) = children[i]->intersect_box(root[i],test);
					switch(cC) {
						case Color::black:
						case Color::gray : count++;
						case Color::white: break;
					}
					whiteIn |= cB;
				}
				else whiteIn |= (root[i].intersect_box(test) != Color::white);
			}

			if (count == 0) return make_tuple(Color::white,true);
			if (count > 0 &&  whiteIn) return make_tuple(Color::gray,true);
			if (count > 0 && !whiteIn) return make_tuple(Color::black,false);
		}
		case Color::gray: {
			if (test.intersect_box(root) == Color::black) return make_tuple(color,color!=Color::black);
			if (color == Color::black) return make_tuple(Color::gray,false);
			int count = 0;
			for (size_t i = 0; i < 8; ++i) {
				if (children[i]) {
					switch(std::get<0>(children[i]->intersect_box(root[i],test))) {
						case Color::black:
						case Color::gray : count++;
						case Color::white: break;
					}
				}
			}

			if (count == 0) return make_tuple(Color::white,true);
			if (count > 0) return make_tuple(Color::gray,true);
		}
		case Color::white:
			break;
	}
	return make_tuple(Color::white,false);
}

tnw::owner_ptr<Tree> tnw::octree::tree_and(Tree* t1, Tree* t2) {
	if (!t1 || !t2) return nullptr;
	if (t1->color == Color::black) return new Tree(*t2);
	if (t2->color == Color::black) return new Tree(*t1);

	owner_ptr<Tree> r = new Tree();
	r->color = Color::gray;
	size_t whiteCounter = 0;

	for (size_t i = 0; i < 8; ++i) {
		r->children[i] = unique_ptr<Tree>(tree_and(t1->get(i),t2->get(i)));
		if (!r->children[i]) whiteCounter++;
	}

	if (whiteCounter == 8) return nullptr;

	return r;
}

tnw::owner_ptr<Tree> tnw::octree::tree_or(Tree* t1, Tree* t2) {
	if (!t1 && !t2) return nullptr;
	if (!t1) return new Tree(*t2);
	if (!t2) return new Tree(*t1);
	if (t1->color == Color::black) return new Tree();
	if (t2->color == Color::black) return new Tree();

	owner_ptr<Tree> r = new Tree();
	r->color = Color::gray;
	size_t blackCounter = 0;

	for (size_t i = 0; i < 8; ++i) {
		r->children[i] = unique_ptr<Tree>(tree_or(t1->get(i),t2->get(i)));
		if (r->children[i] && r->children[i]->color == Color::black) blackCounter++;
	}

	if (blackCounter == 8) return new Tree();

	return r;
}

tnw::owner_ptr<Tree> tnw::octree::make_from_file(FILE* f) {
	char c;
	std::vector<int> counter{0};

	switch(fgetc(f)) {
		case 'b':
			return new Tree();
		case 'w':
			return nullptr;
	}

	owner_ptr<Tree> root = new Tree();
	root->color = Color::gray;

	Tree* cursor = root;
	while (((c = fgetc(f)) != '\n') && (c != EOF)) {
		switch(c) {
			case '(': {
				Tree* aux = cursor;
				cursor = new Tree(cursor);
				aux->set(counter.back(),unique_ptr<Tree>(cursor));
				cursor->color = Color::gray;
				counter.back()++;
				counter.push_back(0);
				break;
			}
			case 'b': {
				cursor->set(counter.back()++,make_unique<Tree>(cursor));
				break;
			}
			case 'w': {
				cursor->set(counter.back()++,nullptr);
				break;
			}
		}

		while (counter.back() == 8) {
			counter.pop_back();
			cursor = cursor->parent;
		}
	}

	return root;
}

// Todo - Make recursive version using std::vector
std::string tnw::octree::serialize(Tree* t) {
	if(!t) return "w";

	std::string o;
	if (t->color == Color::gray) {
		o += "(";
		for (auto&& c : t->children)
			o += serialize(c.get());
	}
	if (t->color == Color::black) {
		o += "b";
	}

	return o;
}

// Todo - Make recursive version using std::vector
owner_ptr<Tree> tnw::octree::classify(const Shape& s, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth){
	if (currDepth >= maxDepth) return nullptr;

	switch (s.intersect_box(bb)) {
		case Color::white:
			return nullptr;
		case Color::black:
			return new Tree();
		case Color::gray: break;
	}

	owner_ptr<Tree> r = new Tree();
	r->color = Color::gray;
	size_t countB = 0;
	size_t countW = 0;

	for (size_t i = 0; i < 8; ++i) {
		r->children[i].reset(classify(s, bb[i], maxDepth, currDepth+1));
		if(r->children[i] && r->children[i]->color == Color::black) countB++;
		if(!r->children[i]) countW++;
	}

	if (countB == 8) return new Tree();
	if (countW == 8) return nullptr;

	return r;
}

/* Recebe a própria bounding box do pai, e dependendo da sua cor faz o seguinte
 * Cinza - Chama o draw nos filhos, e desenha a própria bounding box como  wireframe
 * Preto - Desenha a própria bounding box como preenchida
 */
void tnw::octree::Tree::draw(const BoundingBox& bb){
	std::vector<std::tuple<Tree*,BoundingBox>> stack = {make_tuple(this,bb)};
	while (!stack.empty()) {
		const Tree* t = std::get<0>(stack.back());
		const BoundingBox b = std::get<1>(stack.back());
		stack.pop_back();

		switch (t->color) {
			case Color::white: break;
			case Color::gray: {
				for (int i = 0; i < 8; ++i) {
					if(t->children[i])
						stack.push_back(make_tuple(t->children[i].get(),b[i]));
				}
				break;
			}

			case Color::black: {
				// Desenha sólido em Cor aleatória
				glColor3f(t->drawColor[0],
				          t->drawColor[1],
				          t->drawColor[2]);
				b.draw();

				//Desenha wireframe cinza
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				glColor3f(.5,.5,.5);
				glLineWidth(0.5);
				b.draw();
				glLineWidth(1.0);
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

				break;
			}
		}
	}
}


void tnw::octree::Tree::setColor(const float c[3]) {
	drawColor[0] = c[0];
	drawColor[1] = c[1];
	drawColor[2] = c[2];
	if (color == Color::gray) {
		for (auto&& child : children) {
			if (child) child->setColor(c);
		}
	}
}

double tnw::octree::Tree::volume() const {
	switch (color) {
		case Color::black : {
			return 1;
			break;
		}
		case Color::gray : {
			double volume_sum = 0;
			for (auto&& child : children) {
				if (child) volume_sum += child->volume();
			}
			return volume_sum/8.;
		}
		case Color::white: break;
	}
	return 0;
}
