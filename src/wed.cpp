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

tnw::BRep::BRep(size_t vi, size_t ei, size_t li) : currVertexId(vi), currEdgeId(ei), currLoopId(li)  {};

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
	std::vector<Vertex*> adjvertexv;
	for (auto&& e : adjedge()) {
		adjvertexv.push_back(e->vstart);
		if(e->vend) adjvertexv.push_back(e->vend);
	}

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
	WEdge* curredge = iedge;
	std::vector<WEdge*> adjedge;
	auto less = [](auto* a, auto* b){return a->id < b->id;};
	std::set<WEdge*,decltype(less)> adjedgev(less);

	// Qual loop eu sou?
	bool ccwtravel = (iedge->ccwloop->id == this->id);
	bool og_ccwtravel = ccwtravel;

	do {
		printf("%zu %s ", curredge->id,ccwtravel?"t":"f");
		adjedgev.insert(curredge);
		if (ccwtravel) {
			if ((curredge->vstart->id == curredge->ccwsucc->vstart->id) || (curredge->vend->id == curredge->ccwsucc->vend->id))
				ccwtravel = !ccwtravel;
			curredge = curredge->ccwsucc;
			printf("%zu %s\n", curredge->id,ccwtravel?"t":"f");
		}
		else {
			if ((curredge->vstart->id == curredge->cwsucc->vstart->id) || (curredge->vend->id == curredge->cwsucc->vend->id))
				ccwtravel = !ccwtravel;
			curredge = curredge->cwsucc;
			printf("%zu %s\n", curredge->id,ccwtravel?"t":"f");
		}
	} while (curredge->id != iedge->id || og_ccwtravel != ccwtravel);

	printf("%zu\n\n",id);
	std::copy(adjedgev.begin(), adjedgev.end(), std::back_inserter(adjedge));
	return adjedge;
}

std::vector<tnw::wed::Vertex*> tnw::wed::Loop::adjvertex() {
	std::vector<Vertex*> adjvertexv;
	for (auto&& e : adjedge()) {
		adjvertexv.push_back(e->vstart);
		if(e->vend) adjvertexv.push_back(e->vend);
	}

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

	adjedgev.push_back(cwpred);
	adjedgev.push_back(cwsucc);
	adjedgev.push_back(ccwpred);
	adjedgev.push_back(ccwsucc);

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

	adjloopv.push_back(cwloop);
	adjloopv.push_back(ccwloop);

	return adjloopv;
}

// Desenha wireframe do BRep
void tnw::BRep::rdraw() {
	glColor3fv(color);
	glLineWidth(2);
	glBegin(GL_LINES);
	for (auto&& e : edges) {
		if (!e.vstart || !e.vend) {
			continue;
		}
		bool is_marked = marked.count(std::make_tuple(e.id,1));
		bool is_selected = (e.id == selected_edge);
		const auto& vstart = *e.vstart;
		const auto& vend = *e.vend;

		// Set color if selected
		if (is_marked)   glColor3f(.6,.7,.45);
		if (is_selected) glColor3f(1,0,1);

		glVertex3fv(glm::value_ptr(vstart.position));
		glVertex3fv(glm::value_ptr(vend.position));

		// Reset color
		if (is_marked || is_selected)
			glColor3fv(color);
	}
	glEnd();
	glPointSize(10);
	glBegin(GL_POINTS);
	for (auto&& p : vertices) {
		const bool is_marked = marked.count(std::make_tuple(p.id,0));
		if (is_marked) glColor3f(0,1,0);
		glVertex3fv(glm::value_ptr(p.position));
		if (is_marked) glColor3fv(color);
	}
	glEnd();
	glLineWidth(1);
	glPointSize(1);
}

//Euler operator: Make Vertex Face Shell
void tnw::BRep::mvfs(glm::vec3 position) {
	 //Creates a "fake" vertex
	edges.emplace_front(currEdgeId++);
	WEdge* e = &edges.front();
  //Creates a vertex with the assigned position and the fake incident edge
	vertices.emplace_front(currVertexId++, position, e);
	Vertex* V = &vertices.front();
  //Creates a face
	loops.emplace_front(currLoopId++, e);
	Loop* l = &loops.front();

	e->vstart = V;
	e->vend = nullptr;
	e->cwloop = l;
	e->ccwloop = l;
	e->cwpred = e;
	e->cwsucc = e;
	e->ccwpred = e;
	e->ccwsucc = e;

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

//Euler operator: Make Edge Face
//Given the edges from v1 to v2 and from v3 to v4 incident to loop f1, subdivides the loop with an edge from v1 to v3, creating a new loop f2. The edge v1v2 will be assigned to the new loop
void tnw::BRep::mef(size_t lid, size_t v1id, size_t v2id, size_t v3id, size_t v4id) {
	Vertex *v1 = get_vertex(v1id);
	Vertex *v2 = get_vertex(v2id);
	Vertex *v3 = get_vertex(v3id);
	Vertex *v4 = get_vertex(v4id);

	Loop *l1 = get_loop(lid);

	if (!v1 || !v2 || !v3 || !v4 || !l1) {
		return;
	}

	std::vector<WEdge*> ledges = l1->adjedge();
	WEdge *a = nullptr, *b = nullptr;

	bool a_reverso = false;
	bool b_reverso = false;
	//Get correct edges
	for (auto e : ledges) {
		if (e->vstart && e->vstart->id == v1->id &&
			e->vend   && e->vend->id   == v2->id) {
			a = e;
		}
		if (e->vstart && e->vstart->id == v2->id &&
			e->vend   && e->vend->id   == v1->id) {
			a = e;
			a_reverso = true;
		}
		if (e->vstart && e->vstart->id == v3->id &&
		    e->vend   && e->vend->id   == v4->id) {
			b = e;
		}
		if (e->vstart && e->vstart->id == v4->id &&
		    e->vend   && e->vend->id   == v3->id) {
			b = e;
			b_reverso = true;
		}
	}

	if (!a || !b) {
		printf("Nenhuma aresta\n");
		return;
	}

	if (a_reverso) printf("a_reverso\n");
	if (b_reverso) printf("b_reverso\n");

	l1->iedge = b;

	edges.emplace_front(currEdgeId++);
	WEdge *e = &edges.front();

	loops.emplace_front(currLoopId++, e);
	Loop *l2 = &loops.front();

	if (a->ccwloop->id == l1->id) {
		a->ccwsucc = e;
	}
	if (a->cwloop->id == l1->id) {
		a->cwsucc = e;
	}

	e->vstart = v1;
	e->vend = v3;

	//Starting to walk backwards through the edges that will be transferred to l1
	WEdge *curredge = nullptr, *nextedge = a;
	do {
		curredge = nextedge;

		if (curredge->ccwloop->id == l1->id) {
			curredge->ccwloop = l2;
			nextedge = curredge->ccwpred;
		}
		else if (curredge->cwloop->id == l1->id) {
			curredge->cwloop = l2;
			nextedge = curredge->cwpred;
		}

		printf("loop 1: %zu %zu\n", curredge->id, nextedge->id);
	} while (nextedge->id != b->id);

	if (curredge->ccwloop->id == l1->id) {
		curredge->ccwpred = e;
	}
	if (curredge->cwloop->id == l1->id) {
		curredge->cwpred = e;
	}

	e->ccwloop = l2;
	e->ccwsucc = curredge;
	e->ccwpred = a;

	if (b->ccwloop->id == l1->id) {
		b->ccwsucc = e;
	}
	if (b->cwloop->id == l1->id) {
		b->cwsucc = e;
	}
	//Walking backwards from the v3v4 edge
	curredge = nullptr, nextedge = b;
	do {
		curredge = nextedge;

		if (curredge->ccwloop->id == l1->id) {
			nextedge = curredge->ccwpred;
		}
		else if (curredge->cwloop->id == l1->id) {
			nextedge = curredge->cwpred;
		}

		printf("loop 2: %zu %zu\n", curredge->id, nextedge->id);
	} while (nextedge->id != a->id);

	if (curredge->ccwloop->id == l1->id) {
		curredge->ccwpred = e;
	}
	if (curredge->cwloop->id == l1->id) {
		curredge->cwpred = e;
	}

	e->cwloop = l1;
	e->cwsucc = curredge;
	e->cwpred = b;
}

//Euler operator: special Make Edge Vertex
//Covers cases where we want to create a new vertex eid starting from vid_start in face fid
void tnw::BRep::smev(size_t lid, size_t vid_start, glm::vec3 position) {
	Vertex *v1 = get_vertex(vid_start);
	Loop *l = get_loop(lid);

	if (!v1 || !l) {
		return;
	}

	WEdge *e;
	if (v1->iedge->vend) {
		edges.emplace_front(currEdgeId++);
		e = &edges.front();
	}
	else e = v1->iedge;


	vertices.emplace_front(currVertexId++, position, e);
	Vertex *v2 = &vertices.front();

	e->vstart = v1;
	e->vend = v2;
	e->cwloop = l;
	e->ccwloop = l;

	/* Procura uma aresta na face que tenha o vstart no v1. Caso não exista vértice com vstart em v1
	 * escolhe um vertice com vend em v1
	 */
	std::vector<WEdge*> ledges = l->adjedge();

	for (WEdge* b : ledges) {

		if (b->vstart && (b->vstart->id == v1->id)) {
			//Caso 1: b é um edge com vstart em v1
			if (lid == b->cwloop->id) {
				//Caso 1.1: loop dado é clockwise loop de b
				e->ccwsucc = e;
				e->cwpred = e;
				e->ccwpred = b;
				e->cwsucc = b->cwsucc;

				if (lid == b->cwsucc->ccwloop->id) {
					b->cwsucc->ccwpred = e;
				}
				else if (lid == b->cwsucc->cwloop->id) {
					b->cwsucc->cwpred = e;
				}

				b->cwsucc = e;

			} else {
				//Caso 1.2: loop dado é counterclockwise loop de b
				e->ccwsucc = e;
				e->cwpred = e;
				e->ccwpred = b->ccwpred;
				e->cwsucc = b;

				if (lid == b->ccwpred->cwloop->id) {
					b->ccwpred->cwsucc = e;
				}
				else if (lid == b->ccwpred->ccwloop->id) {
					b->ccwpred->ccwsucc = e;
				}

				b->ccwpred = e;
			}

			break;

		} else if (b->vend && (b->vend->id == v1->id)) {
			//Caso 2: b é um edge com vend em v1

			if (lid == b->cwloop->id) {
				//Caso 2.1: loop dado é clockwise loop de b
				e->ccwsucc = e;
				e->cwpred = e;
				e->ccwpred = b->cwpred;
				e->cwsucc = b;

				if (lid == b->cwpred->ccwloop->id) {
					b->cwpred->ccwsucc = e;
				}
				else if (lid == b->cwpred->cwloop->id) {
					b->cwpred->cwsucc = e;
				}

				b->cwpred = e;
			} else {
				//Caso 2.2: loop dado é counterclockwise loop de b
				e->ccwsucc = e;
				e->cwpred = e;
				e->ccwpred = b;
				e->cwsucc = b->ccwsucc;

				if (lid == b->ccwsucc->cwloop->id) {
					b->ccwsucc->cwpred = e;
				}
				else if (lid == b->ccwsucc->ccwloop->id) {
					b->ccwsucc->ccwpred = e;
				}

				b->ccwsucc = e;
			}
			break;
		}

		//Caso 3: nem vstart nem vend são v1, ignorar
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

std::string tnw::BRep::serialize() const {
	throw 0;
}

tnw::owner_ptr<tnw::Model> tnw::BRep::clone() const {
	throw 0;
}

void tnw::BRep::setColor(const float c[3]) {
	clear_marks();
	for (int i = 0; i < 3; ++i)
		color[i] = c[i];
}
PaintColor tnw::BRep::getColor() const {
	PaintColor ret;
	for (int i = 0; i < 3; ++i)
		ret[i] = color[i];
	return ret;
}

void tnw::BRep::mark_edge(size_t id) {
	marked.emplace(id,1);
}
void tnw::BRep::mark_vertex(size_t id) {
	marked.emplace(id,0);
}
void tnw::BRep::clear_marks() {
	marked.clear();
	selected_edge = 0;
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

void tnw::BRep::print_info() const {
	printf("v { ");
	for (auto&& v : vertices) {
		printf("\n[%lu] iedge [%lu] pos [%f,%f,%f]", v.id, v.iedge->id, v.position[0], v.position[1], v.position[2]);
	}
	printf("\n}\ne {");
	for (auto&& e : edges) {
		printf("\n[%zu] vs [%zu,%zu] loop [%zu,%zu] succ [%zu,%zu] pred [%zu,%zu]", e.id,e.vstart?e.vstart->id:0,e.vend?e.vend->id:0,e.cwloop?e.cwloop->id:0,e.ccwloop?e.ccwloop->id:0,e.cwsucc?e.cwsucc->id:0,e.ccwsucc?e.ccwsucc->id:0,e.cwpred?e.cwpred->id:0,e.ccwpred?e.ccwpred->id:0);
	}
	printf("\n}\nl {");
	for (auto&& l : loops) {
		printf("\n[%lu] iedge [%lu]", l.id, l.iedge->id);
	}
	printf("\n}\n");
}
