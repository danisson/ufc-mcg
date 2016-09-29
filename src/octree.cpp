#include "model.h"
#include "helper.h"
#include <vector>
#include <GL/gl.h>
#include <iostream>
#include <memory>
#include <random>

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
owner_ptr<Tree> tnw::octree::classify(Classifier c, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth){
	switch (c(bb)) {
		case Color::white:
			return nullptr;
		case Color::black:
			return new Tree();
		case Color::gray: break;
	}
	if (currDepth >= maxDepth) return nullptr;

	owner_ptr<Tree> r = new Tree();
	r->color = Color::gray;

	for (size_t i = 0; i < 8; ++i)
		r->children[i].reset(classify(c, bb[i], maxDepth, currDepth+1));

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

			// Desenha wireframe cinza
			// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			// glColor3f(.5,.5,.5);
			// glLineWidth(0.5);
			// bb.draw();
			// glLineWidth(1.0);
			// glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			break;
		}
		case Color::black: {
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
		}

	}
}

//SHAPE IMPLEMENTATIONS
tnw::octree::Sphere::Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {}

Color tnw::octree::Sphere::operator()(const BoundingBox& bb){
	unsigned int count = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (glm::distance(center, bb.getVertice(i)) < radius) {
			count++;
		}
	}
	if (count >= 8) { /*std::cout << "black\n";*/ return tnw::octree::Color::black; }
	//std::cout << "gray\n";
	return tnw::octree::Color::gray;
}

tnw::octree::Box::Box(glm::vec3 center, float length, float depth, float height) : center(center), length(length), depth(depth), height(height) {}

Color tnw::octree::Box::operator()(const BoundingBox& bb){
	unsigned int count = 0;
	glm::vec3 p;

	//Bounding box intercepta a caixa
	if (tnw::box_intersection(bb.getCenter(), bb.depth, bb.depth, bb.depth, center, length, height, depth)){
		for (int i = 0; i < 8; ++i){
			unsigned int countcoords = 0;
			p = bb.getVertice(i);
			//std::cout << "p: " << to_string(p) << "\n";
			if (p[0] >= center[0] - length/2.f && p[0] <= center[0] + length/2.f) {
				countcoords++;
			}
			if (p[1] >= center[1] - height/2.f && p[1] <= center[1] + height/2.f) {
				countcoords++;
			}
			if (p[2] >= center[2] - depth/2.f && p[2] <= center[2] + depth/2.f) {
				countcoords++;
			}
			//std::cout << "count coords: " << countcoords << "\n";
			if (countcoords >= 3) {
				count++;
			}
		}
		//std::cout << "count: " << count << "\n";
		if (count >= 8){
			return tnw::octree::Color::black;
		} else {
			return tnw::octree::Color::gray;
		}
	} else {
		return tnw::octree::Color::white;
	}
}

tnw::octree::Cilinder::Cilinder(glm::vec3 inferiorPoint, float height, float radius) : inferiorPoint(inferiorPoint), height(height), radius(radius) {}

Color tnw::octree::Cilinder::operator()(const BoundingBox& bb){
		unsigned int count = 0;
	glm::vec3 p, y(0,1,0);
	for (int i = 0; i < 8; ++i)
	{
		p = bb.getVertice(i);
		//std::cout << "p: " << to_string(p) << "c: " << to_string(c+(p[1]-c[1])*y) << "\n";
		if ((p[1] >= inferiorPoint[1]) && (p[1] <= inferiorPoint[1]+height) && (glm::distance(p, inferiorPoint+(p[1]-inferiorPoint[1])*y) <= radius)){
			count++;
		}
	}
	//std::cout << "===\n";
	if (count >= 8){
		return tnw::octree::Color::black;
	} else {
		return tnw::octree::Color::gray;
	}
}

tnw::octree::SquarePyramid::SquarePyramid(glm::vec3 inferiorPoint, float height, float basis) : inferiorPoint(inferiorPoint), height(height), basis(basis) {}
Color tnw::octree::SquarePyramid::operator()(const BoundingBox& bb){
	unsigned int count = 0;
	glm::vec3 p;
	for (int i = 0; i < 8; ++i)
	{
		bool xPos, yPos, zPos;
		float proportionalBasis;

		p = bb.getVertice(i);
		proportionalBasis = basis*(height-p[1]) / height;
		xPos = (p[0] >= inferiorPoint[0]-proportionalBasis/2.0) && (p[0] <= inferiorPoint[0]+proportionalBasis/2.0);
		yPos = (p[1] >= inferiorPoint[1]) && (p[1] <= inferiorPoint[1]+height);
		zPos = (p[2] >= inferiorPoint[2]-proportionalBasis/2.0) && (p[2] <= inferiorPoint[2]+proportionalBasis/2.0);
		if (yPos && xPos && zPos){
			count++;
		}
	}
	if (count >= 8){
		return tnw::octree::Color::black;
	} else {
		return tnw::octree::Color::gray;
	}
}
