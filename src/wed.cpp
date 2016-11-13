#include "wed.h"
#include <iostream>

using namespace tnw;
using namespace tnw::wed;

using std::cout;
using std::endl;

tnw::wed::Vertex::Vertex(size_t _id, glm::vec3 _position, tnw::wed::WEdge* _iedge) : id(_id), position(_position), iedge(_iedge) { }

tnw::wed::WEdge::WEdge(size_t _id) : id(_id) { };

tnw::wed::WEdge::WEdge(size_t _id, tnw::wed::Vertex* _vstart, tnw::wed::Vertex* _vend, tnw::wed::Loop* _lloop, tnw::wed::Loop* _rloop, tnw::wed::WEdge* _lpred, tnw::wed::WEdge* _lsucc, tnw::wed::WEdge* _rpred, tnw::wed::WEdge* _rsucc) : id(_id), vstart(_vstart), vend(_vend), lloop(_lloop), rloop(_rloop), lpred(_lpred), lsucc(_lsucc), rpred(_rpred), rsucc(_rsucc) { };

tnw::wed::Loop::Loop(size_t _id, tnw::wed::WEdge* _iedge) : id(_id), iedge(_iedge) {};



std::vector<tnw::wed::WEdge*> tnw::wed::Vertex::adjedge() {
	WEdge* curredge = iedge; 
	std::vector<WEdge*> adjedgev;

	//Escolhe uma direção pra percorrer as arestas
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

	return adjvertexv;
}
std::vector<tnw::wed::Loop*> tnw::wed::Vertex::adjloop() {
	WEdge* curredge = iedge; 
	std::vector<Loop*> adjloopv;

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
