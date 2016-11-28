#include "wed.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>

using namespace tnw;
using namespace tnw::wed;

using std::cout;
using std::endl;

tnw::wed::Vertex::Vertex(size_t id, glm::vec3 position, tnw::wed::WEdge* iedge) : id(id), position(position), iedge(iedge) {}

tnw::wed::WEdge::WEdge(size_t id) : id(id) {};

tnw::wed::WEdge::WEdge(size_t id, Vertex* vstart, Vertex* vend, Loop* lloop, Loop* rloop, WEdge* lpred, WEdge* lsucc, WEdge* rpred, WEdge* rsucc) : id(id), vstart(vstart), vend(vend), lloop(lloop), rloop(rloop), lpred(lpred), lsucc(lsucc), rpred(rpred), rsucc(rsucc) {};

tnw::wed::Loop::Loop(size_t id, WEdge* iedge) : id(id), iedge(iedge) {};

std::vector<tnw::wed::WEdge*> tnw::wed::Vertex::adjedge() {
	WEdge* curredge = iedge;
	std::vector<WEdge*> adjedgev;

	do {
		adjedgev.push_back(curredge);
		//Checa se é vstart ou vend
		if (curredge->vstart == this) {
			curredge = curredge->rpred;
		} else {
			curredge = curredge->lpred;
		}

	} while (curredge != iedge);

	return adjedgev;
}
std::vector<tnw::wed::Vertex*> tnw::wed::Vertex::adjvertex() {
	WEdge* curredge = iedge;
	std::vector<Vertex*> adjvertexv;

	do {
		//Checa se é vstart ou vend
		if (curredge->vstart == this) {
			curredge = curredge->rpred;
			adjvertexv.push_back(curredge->vend);
		} else {
			curredge = curredge->lpred;
			adjvertexv.push_back(curredge->vend);
		}

	} while (curredge != iedge);

	return adjvertexv;
}
std::vector<tnw::wed::Loop*> tnw::wed::Vertex::adjloop() {
	WEdge* curredge = iedge;
	std::vector<Loop*> adjloopv;
	//Como a gente pode encontrar faces repetidas, adicionamos em um set para
	//garantir que são únicas
	auto less = [](auto* a, auto* b){return a->id < b->id;};
	std::set<Loop*,decltype(less)> adjloops(less);
	do {

		adjloops.insert(curredge->rloop);
		adjloops.insert(curredge->lloop);

		//Checa se é vstart ou vend
		if (curredge->vstart == this) {
			curredge = curredge->rpred;
		} else {
			curredge = curredge->lpred;
		}

	} while (curredge != iedge);

	//Copia os valores para um vetor
	std::copy(adjloops.begin(), adjloops.end(), std::back_inserter(adjloopv));

	return adjloopv;
}

std::vector<tnw::wed::WEdge*> tnw::wed::Loop::adjedge() {
	WEdge* curredge = iedge;
	std::vector<WEdge*> adjedgev;

	do {
		cout << "curredge: " << curredge->id << endl;
		adjedgev.push_back(curredge);

		//Primeiro, descobre se é rloop ou lloop
		if (curredge->rloop == this) {
			curredge = curredge->rsucc;
		} else {
			curredge = curredge->lsucc;
		}

	} while (curredge != iedge);

	return adjedgev;
}

std::vector<tnw::wed::Vertex*> tnw::wed::Loop::adjvertex() {
	WEdge* curredge = iedge;
	std::vector<Vertex*> adjvertexv;

	do {
		cout << "curredge: " << curredge->id << endl;

		//Primeiro, descobre se é rloop ou lloop
		//Se for rloop, coloca o vend, se for lloop, coloca vbegin
		if (curredge->rloop == this) {
			adjvertexv.push_back(curredge->vend);
			curredge = curredge->rsucc;
		} else {
			adjvertexv.push_back(curredge->vstart);
			curredge = curredge->lsucc;
		}

	} while (curredge != iedge);

	return adjvertexv;
}

std::vector<tnw::wed::Loop*> tnw::wed::Loop::adjloop() {

	WEdge* curredge = iedge;
	std::vector<Loop*> adjloopv;

	do {
		cout << "curredge: " << curredge->id << endl;

		//Primeiro, descobre se é rloop ou lloop,
		if (curredge->rloop == this) {
			adjloopv.push_back(curredge->lloop);
			curredge = curredge->rsucc;
		} else {
			adjloopv.push_back(curredge->rloop);
			curredge = curredge->lsucc;
		}

	} while (curredge != iedge);

	return adjloopv;
}


std::vector<WEdge*> tnw::wed::WEdge::adjedge() {
	std::vector<WEdge*> adjedgev;

	adjedgev.push_back(lpred);
	adjedgev.push_back(lsucc);
	adjedgev.push_back(rpred);
	adjedgev.push_back(rsucc);

	return adjedgev;
}
std::vector<Vertex*> tnw::wed::WEdge::adjvertex() {
	std::vector<Vertex*> adjvertexv;

	adjvertexv.push_back(vstart);
	adjvertexv.push_back(vend);

	return adjvertexv;
}
std::vector<Loop*> tnw::wed::WEdge::adjloop() {
	std::vector<Loop*> adjloopv;

	adjloopv.push_back(lloop);
	adjloopv.push_back(rloop);

	return adjloopv;
}

// Desenha wireframe do BRep
void tnw::BRep::rdraw() {
	glColor3f(.5,.5,.5);
	glLineWidth(0.5);
	glBegin(GL_LINES);
	for (auto&& i : edges) {
		const auto e = std::get<1>(i);
		const auto& vstart = *e.vstart;
		const auto& vend = *e.vend;
		// const auto& vstart = vertices[e.vstart];
		// const auto& vend = vertices[e.vend];
		glVertex3fv(glm::value_ptr(vstart.position));
		glVertex3fv(glm::value_ptr(vend.position));
	}
	glEnd();
	glLineWidth(1);
}
