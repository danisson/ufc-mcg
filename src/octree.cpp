#include "model.h"
#include <GL/gl.h>
#include <iostream>

using namespace tnw;

octree::Tree::Tree() {
	children.fill(nullptr);
	color = Color::black;
}

octree::Tree::Tree(std::array<Tree*,8> children) {
	this->children = children;
	color = Color::gray;
}

/* Recebe a própria bounding box do pai, e dependendo da sua cor faz o seguinte
 * Cinza - Chama o draw nos filhos, e desenha a própria bounding box como  wireframe
 * Preto - Desenha a própria bounding box como preenchida
 */
void octree::Tree::draw(const octree::BoundingBox& bb){
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
