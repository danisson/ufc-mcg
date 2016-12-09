#include "wed.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	glColor3fv(color);
	glLineWidth(0.5);
	glBegin(GL_LINES);
	for (auto&& e : edges) {
		// const auto e = std::get<1>(i);
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

//Euler operator: Make Vertex Face Shell
void tnw::BRep::mvfs(glm::vec3 position) {
	//Creates a "fake" vertex
	WEdge* e = new WEdge(currEdgeId++);
	//Creates a vertex with the assigned position and the fake incident edge
	Vertex* V = new Vertex(currVertexId++, position, e);
	//Creates a face
	Loop* l = new Loop(currLoopId++, e);

	e->vstart = V;
	e->vend = nullptr;
	e->lloop = l;
	e->rloop = l;
	e->lpred = e;
	e->lsucc = e;
	e->rpred = e;
	e->rsucc = e;

	vertices.push_back(*V);
	edges.push_back(*e);
	loops.push_back(*l);
}

void tnw::BRep::translate(const glm::vec3& dv) {
	const glm::mat4 translateMatrix = glm::translate(glm::mat4(), dv);
	applyTransform(translateMatrix);
}

void tnw::BRep::scale(const float dx) {
	const glm::mat4 scaleMatrix = glm::scale(glm::mat4(), {dx, dx, dx});
	applyTransform(scaleMatrix);
}

void tnw::BRep::applyTransform(const glm::mat4& t) {
	for (Vertex& v : vertices) {
		//Position of point in homogeneous coordinates
		const glm::vec4 homPosition(v.position, 1);
		//Transformed position
		const glm::vec4 transHomPosition = t * homPosition;
		//Transforms to common 3D coordinates again
		v.position = glm::vec3(transHomPosition);
	}
}

//Shouldn't be called!
Color tnw::BRep::intersect_point(const glm::vec3&) const {
	throw 1;
}

//Shouldn't be called!
Color tnw::BRep::intersect_box(const BoundingBox&) const {
	throw 1;
}

//Shouldn't be called!
IntersectionList tnw::BRep::intersect_ray(const Ray&) const {
	throw 1;
}

//Shouldn't be called!
tnw::BoundingBox tnw::BRep::boundingBox() const {
	throw 1;
}

//Shouldn't be called!
double tnw::BRep::volume() const {
	throw 1;
}

//Shouldn't be called!
tnw::BooleanErrorCodes tnw::BRep::bool_and(const Model& y) {
	return BooleanErrorCodes::unimplementedType;
}

//Shouldn't be called!
tnw::BooleanErrorCodes tnw::BRep::bool_or(const Model& y) {
	return BooleanErrorCodes::unimplementedType;
}

void tnw::BRep::setColor(const float c[3]) {
	for (int i = 0; i < 3; ++i)
		color[i] = c[i];
}
PaintColor tnw::BRep::getColor() const {
	PaintColor ret;
	for (int i = 0; i < 3; ++i)
		ret[i] = color[i];
	return ret;
}
