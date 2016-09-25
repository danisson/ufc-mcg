#include "model.h"
#include <vector>
#include <GL/gl.h>
#include <iostream>
#include <memory>
#include <random>

using namespace tnw::octree;

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

			for (int i = 0; i < 8; ++i)
			{
				if (children[i] != nullptr) {
					children[i]->draw(bb[i]);
				}
			}

			//Desenha wireframe cinza
			// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			// glColor3f(.5,.5,.5);
			// glLineWidth(1.5);
			// bb.draw();
			// glLineWidth(1.0);
			// glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			break;
		}
		case Color::black: {
			//Desenha sólido em Cor aleatória
			//glColor3f(drawColor[0], drawColor[1], drawColor[2]);
			//glColor3f(0,0,0.8);
			//bb.draw();

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


tnw::octree::Tree::Tree(Tree* parent) {
	children.fill(nullptr);
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

tnw::octree::Tree::Tree(std::array<Tree*,8> children, Tree* parent) {
	this->children = children;
	for (auto&& child : children) {
		if (child != nullptr) {
			child->parent = this;
		}
	}
	color = Color::gray;
	this->parent = parent;

	std::random_device r;
	std::default_random_engine gen(r());

	std::uniform_real_distribution<> dis(0,1);

	for (int i = 0; i < 3; ++i)
	{
		drawColor[i] = dis(gen);
	}

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

// Todo - Make recursive version using std::vector
void tnw::octree::Tree::classify(Classifier function, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth){
	Color color = function(bb);
	this->color = color;
	if (currDepth >= maxDepth) return;
	if (color == Color::gray) {
		for (int i = 0; i < 8; ++i)
		{
			//Cria um filho com ela como pai
			Tree *child = new Tree(this);
			//Adiciona o filho a própria árvore
			(*this)[i] = child;
			//Chama recursivamente na árvore filha
			child->classify(function, bb[i], maxDepth, currDepth+1);
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

tnw::octree::Box::Box(glm::vec3 center, float length, float depth, float height) : center(center), length(length), depth(depth), height(height){

}

Color tnw::octree::Box::operator()(const BoundingBox& bb){
	unsigned int count = 0;
	glm::vec3 p/*, x(1,0,0), y(0,1,0), z(0,0,1)*/;
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
	} else if (count > 0) {
		return tnw::octree::Color::gray;
	} else {
		return tnw::octree::Color::gray;
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
		p = bb.getVertice(i);

		xPos = (p[0] >= inferiorPoint[0]-basis/2.0) && (p[0] <= inferiorPoint[0]+basis/2.0);
		yPos = (p[1] >= inferiorPoint[1]) && (p[1] <= inferiorPoint[1]+height);
		zPos = (p[2] >= inferiorPoint[2]-basis/2.0) && (p[2] <= inferiorPoint[2]+basis/2.0); 
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
