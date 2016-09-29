#include "model.h"
#include "octree.h"
#include "helper.h"
#include <vector>
#include <GL/gl.h>
#include <iostream>
#include <memory>
#include <random>

//Implementação da classe tree!

using namespace tnw::octree;
using tnw::owner_ptr;
using std::make_unique;

tnw::octree::Tree::Tree(Tree* parent) {
	color = Color::black;
	this->parent = parent;

	std::random_device r;
	std::default_random_engine gen(r());

	std::uniform_real_distribution<> dis(0,1);

	for (int i = 0; i < 3; ++i)
	{
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

tnw::octree::Tree::Tree(Tree& o) {
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

tnw::owner_ptr<Tree> tnw::octree::tree_and(Tree* t1, Tree* t2) {
	if (!t1 || !t2) return nullptr;
	if (t1->color == Color::black) return new Tree(*t2);
	if (t2->color == Color::black) return new Tree(*t1);

	owner_ptr<Tree> r = new Tree();
	r->color = Color::gray;

	for (size_t i = 0; i < 8; ++i) {
		r->children[i] = unique_ptr<Tree>(tree_and(t1->get(i),t2->get(i)));
	}

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

	while ((c = std::fgetc(f)) != '\n' && c != EOF) {
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

		if (counter.back() == 8) {
			counter.pop_back();
			cursor = cursor->parent;
		}
	}

	return root;
}

// Todo - Make recursive version using std::vector
std::string tnw::octree::serialize(Tree* t) {
	if(!t) return "w";double volume();

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
owner_ptr<Tree> tnw::octree::classify(Classifier c, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth){
	if (currDepth >= maxDepth) return nullptr;

	switch (c(bb)) {
		case Color::white:
			return nullptr;
		case Color::black:
			return new Tree();
		case Color::gray: break;
	}

	owner_ptr<Tree> r = new Tree();
	r->color = Color::gray;
	size_t count = 0;

	for (size_t i = 0; i < 8; ++i) {
		r->children[i].reset(classify(c, bb[i], maxDepth, currDepth+1));
		if(r->children[i] && r->children[i]->color == Color::black) count++;
	}

	if (count == 8) return new Tree();

	return r;
}

/* Recebe a própria bounding box do pai, e dependendo da sua cor faz o seguinte
 * Cinza - Chama o draw nos filhos, e desenha a própria bounding box como  wireframe
 * Preto - Desenha a própria bounding box como preenchida
 */
void tnw::octree::Tree::draw(const BoundingBox& bb){
	switch (color) {
		case Color::white: {
			//Não é pra acontecer!
			break;
		}

		case Color::gray: {
			for (int i = 0; i < 8; ++i) {
				if (children[i]) {
					children[i]->draw(bb[i]);
				}
			}
			break;
		}

		case Color::black: {
			// std::cout << "chamando draw na black???\n";
			// Desenha sólido em Cor aleatória
			glColor3f(drawColor[0], drawColor[1], drawColor[2]);
			// glColor3f(0,0,0.8);
			bb.draw();

			//Desenha wireframe cinza
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glColor3f(.5,.5,.5);
			glLineWidth(0.5);
			bb.draw();
			glLineWidth(1.0);
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			break;
		}

	}
}

double tnw::octree::Tree::volume(){
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