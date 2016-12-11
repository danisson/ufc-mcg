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

tnw::wed::WEdge::WEdge(size_t id, Vertex* vstart, Vertex* vend, Loop* cwloop, Loop* ccwloop, WEdge* cwpred, WEdge* cwsucc, WEdge* ccwpred, WEdge* ccwsucc) : id(id), vstart(vstart), vend(vend), cwloop(cwloop), ccwloop(ccwloop), cwpred(cwpred), cwsucc(cwsucc), ccwpred(ccwpred), ccwsucc(ccwsucc) {};

tnw::wed::Loop::Loop(size_t id, WEdge* iedge) : id(id), iedge(iedge) {};

bool tnw::wed::WEdge::cwpred_dir() {
	if (cwpred->vend->id == this->vstart->id) {
		return true;
	}
	return false;
}

bool tnw::wed::WEdge::cwsucc_dir() {
	if (cwsucc->vstart->id == this->vend->id) {
		return true;
	}
	return false;
}

bool tnw::wed::WEdge::ccwpred_dir() {
	if (ccwpred->vend->id == this->vstart->id) {
		return true;
	}
	return false;
}

bool tnw::wed::WEdge::ccwsucc_dir() {
	if (ccwsucc->vstart->id == this->vend->id) {
		return true;
	}
	return false;
}

std::vector<tnw::wed::WEdge*> tnw::wed::Vertex::adjedge() {
	WEdge* curredge = iedge;
	std::vector<WEdge*> adjedgev;

	do {
		adjedgev.push_back(curredge);
		//Checa se é vstart ou vend
		if (curredge->vstart == this) {
			curredge = curredge->ccwpred;
		} else {
			curredge = curredge->cwpred;
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
			curredge = curredge->ccwpred;
			adjvertexv.push_back(curredge->vend);
		} else {
			curredge = curredge->cwpred;
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

		adjloops.insert(curredge->ccwloop);
		adjloops.insert(curredge->cwloop);

		//Checa se é vstart ou vend
		if (curredge->vstart == this) {
			curredge = curredge->ccwpred;
		} else {
			curredge = curredge->cwpred;
		}

	} while (curredge != iedge);

	//Copia os valores para um vetor
	std::copy(adjloops.begin(), adjloops.end(), std::back_inserter(adjloopv));

	return adjloopv;
}

std::vector<tnw::wed::WEdge*> tnw::wed::Loop::adjedge() {
	WEdge *curredge = iedge, *prevedge = nullptr;
	bool prev_positive_dir = false;
	std::vector<WEdge*> adjedgev;

	do {
		// prevedge = curredge;
		cout << "curredge: " << curredge->id << endl;
		cout << "prevedge: " << prevedge->id << endl;

		//Primeiro, descobre se é ccwloop ou cwloop
		if (curredge->ccwloop == this) {
			//Se a aresta tá na direção correta
			if (prev_positive_dir) {
				curredge = curredge->ccwsucc;
				prev_positive_dir = curredge->ccwsucc_dir();
			}
			//Se a aresta tá na direção oposta, inverte a direção 
			else {
				curredge = curredge->cwpred;
			}
		} else {
			//Se a aresta tá na direção correta
			if (!prev_positive_dir) {
				curredge = curredge->cwsucc;
			} else {
				curredge = curredge->ccwpred;
			}
		}

	} while (curredge != iedge);

	return adjedgev;
}

std::vector<tnw::wed::Vertex*> tnw::wed::Loop::adjvertex() {
	WEdge* curredge = iedge;
	std::vector<Vertex*> adjvertexv;

	do {
		cout << "curredge: " << curredge->id << endl;

		//Primeiro, descobre se é ccwloop ou cwloop
		//Se for ccwloop, coloca o vend, se for cwloop, coloca vbegin
		if (curredge->ccwloop == this) {
			adjvertexv.push_back(curredge->vstart);
			curredge = curredge->ccwsucc;
		} else {
			adjvertexv.push_back(curredge->vend);
			curredge = curredge->cwsucc;
		}

	} while (curredge != iedge);

	return adjvertexv;
}

std::vector<tnw::wed::Loop*> tnw::wed::Loop::adjloop() {

	WEdge* curredge = iedge;
	std::vector<Loop*> adjloopv;

	do {
		cout << "curredge: " << curredge->id << endl;

		//Primeiro, descobre se é ccwloop ou cwloop,
		if (curredge->ccwloop == this) {
			adjloopv.push_back(curredge->cwloop);
			curredge = curredge->ccwsucc;
		} else {
			adjloopv.push_back(curredge->ccwloop);
			curredge = curredge->cwsucc;
		}

	} while (curredge != iedge);

	return adjloopv;
}


std::vector<WEdge*> tnw::wed::WEdge::adjedge() {
	std::vector<WEdge*> adjedgev;

	auto less = [](auto* a, auto* b){return a->id < b->id;};
	std::set<Loop*,decltype(less)> adjedges(less);

	adjedges.push_back(cwpred);
	adjedges.push_back(cwsucc);
	adjedges.push_back(ccwpred);
	adjedges.push_back(ccwsucc);

	std::copy(adjedges.begin(), adjedges.end(), std::back_inserter(adjedgev));

	return adjedgev;
}
std::vector<Vertex*> tnw::wed::WEdge::adjvertex() {
	std::vector<Vertex*> adjvertexv;

	adjvertexv.push_back(vstart);
	if (vstart->id != vend->id) {
		adjvertexv.push_back(vend);
	}

	return adjvertexv;
}
std::vector<Loop*> tnw::wed::WEdge::adjloop() {
	std::vector<Loop*> adjloopv;

	adjloopv.push_back(cwloop);
	if (cwloop->id != ccwloop->id) {
		adjloopv.push_back(ccwloop);
	}

	return adjloopv;
}

tnw::BRep::BRep() {

}

// Desenha wireframe do BRep
void tnw::BRep::rdraw() {
	glColor3fv(color);
	glLineWidth(0.5);
	glBegin(GL_LINES);
	for (auto&& e : edges) {
		bool is_marked = marked.count(std::make_tuple(e.id,1));
		bool is_selected = (e.id == selected_edge);
		const auto& vstart = *e.vstart;
		const auto& vend = *e.vend;

		// Set color if selected
		if (is_marked)   glColor3f(0,1,0);
		if (is_selected) glColor3f(1,0,1);

		glVertex3fv(glm::value_ptr(vstart.position));
		glVertex3fv(glm::value_ptr(vend.position));

		// Reset color
		if (is_marked || is_selected)
			glColor3fv(color);
	}
	glEnd();
	glLineWidth(1);
}

//Euler operator: Make Vertex Face Shell
void tnw::BRep::mvfs(glm::vec3 position) {
	//Creates a "fake" vertex
	WEdge* e = new WEdge(-1);
	//Creates a vertex with the assigned position and the fake incident edge
	Vertex* V = new Vertex(currVertexId++, position, e);
	//Creates a face
	Loop* l = new Loop(currLoopId++, e);

	e->vstart = V;
	e->vend = nullptr;
	e->cwloop = l;
	e->ccwloop = l;
	e->cwpred = e;
	e->cwsucc = e;
	e->ccwpred = e;
	e->ccwsucc = e;

	vertices.push_back(*V);
	edges.push_back(*e);
	loops.push_back(*l);
}

//Euler operator: special Make Edge Face
void tnw::BRep::smef(size_t lid, size_t v1id, size_t v2id) {
	// Loop*   f  = get_loop(lid);
	// Vertex* v1 = get_vertex(v1id);
	// Vertex* v2 = get_vertex(v2id);

	// Checar se v1 e v2 pertencem a f
	// Criar face nova f2
	// Criar aresta nova com faces f f2 e pontas v1,v2
	// Colocar a aresta nova
}

//Euler operator: special Make Edge Vertex
//Covers cases where we want to create a new vertex eid starting from vid_start in face fid
void tnw::BRep::smev(size_t lid, size_t vid_start, glm::vec3 position) {
	WEdge *e = new WEdge(currEdgeId++);
	Vertex *v1 = get_vertex(vid_start);
	Loop *l = get_loop(lid);

	if (!v1 && !l) {
		return;
	}

	Vertex *v2 = new Vertex(currVertexId++, position, e);
	std::cout << "created vertex v2\n";

	e->vstart = v1;
	e->vend = v2;
	e->cwloop = l;
	e->ccwloop = l;
	std::cout << "set v2 basic parameters\n";

	/* Procura uma aresta na face que tenha o vstart no v1, ou vend em v1.
	 */
	std::vector<WEdge*> ledges = l->adjedge();

	for (WEdge*& ebase : ledges) {
		if (ebase->vstart->id == v1->id) {
			std::cout << "choose edge with id " << ebase->id << "\n";
			if (ebase->cwloop->id == l->id) {
				//Case 1: Loop is clockwise loop of the edge
				e->cwsucc = e;
				e->cwpred = ebase;
				e->ccwsucc = e;
				e->ccwpred = ebase->cwsucc;

				ebase->cwsucc = e;
			} else {
				//Case 2: Loop is counterclockwise loop of the edge
				e->cwsucc = e;
				e->cwpred = ebase;
				e->ccwsucc = e;
				e->ccwpred = ebase->ccwpred;

				ebase->ccwpred = e;
			}

			break;

		} else { //ebase->vend->id == v1->id
			std::cout << "choose edge with id " << ebase->id << "\n";

			if (ebase->cwloop->id == l->id) {
				//Case 1: Loop is clockwise loop of the edge
				e->cwsucc = e;
				e->cwpred = ebase->cwpred;
				e->ccwsucc = e;
				e->ccwpred = e;

				ebase->cwpred = e;
			} else {
				//Case 2: Loop is counterclockwise loop of the edge
				e->cwsucc = e;
				e->cwpred = ebase->ccwsucc;
				e->ccwsucc = e;
				e->ccwpred = ebase;

				ebase->ccwsucc = e;
			}

			break;
		}
	}

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

Loop* tnw::BRep::get_loop(size_t id) {
	for (auto&& i : loops)
		if (i.id == id) return &i;
	return nullptr;
}

WEdge* tnw::BRep::get_wedge(size_t id) {
	for (auto&& i : edges)
		if (i.id == id) return &i;
	return nullptr;
}

Vertex* tnw::BRep::get_vertex(size_t id) {
	for (auto&& i : vertices)
		if (i.id == id) return &i;
	return nullptr;
}

std::string tnw::BRep::serialize() const {
	throw 1;
}

owner_ptr<Model> tnw::BRep::clone() const {
	throw 1;
}

void tnw::BRep::draw() {
	rdraw();
}