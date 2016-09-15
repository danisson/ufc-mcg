#include "model.h"
#include <vector>
#include <GL/gl.h>
#include <iostream>
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
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			//Por enquanto desenhando com um cinza claro
			glColor3f(.5,.5,.5);
			glLineWidth(1.5);
			bb.draw();
			glLineWidth(1.0);
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			for (int i = 0; i < 8; ++i)
			{
				if (children[i] != nullptr) {
					children[i]->draw(bb[i]);
				}
			}
			break;
		}
		case Color::black: {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			//Por enquanto desenhando com um cinza claro
			glColor3f(.5,.5,.5);
			glLineWidth(1.5);
			bb.draw();
			glLineWidth(1.0);
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			//Por enquanto desenha vermelho
			glColor3f(.0,.0,0.8);
			bb.draw();
		}

	}
}


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

void tnw::octree::Tree::classify(Classifier function, BoundingBox bb, unsigned int maxDepth, unsigned int currDepth){
	Color color = function(bb);
	this->color = color;

	if (currDepth == maxDepth) return;
	if (color == Color::gray) {
		for (int i = 0; i < 8; ++i)
		{
			//Cria um filho com ela como pai
			Tree *child = new Tree(this);
			
		}
	}
}
