#ifndef _H_WED
#define _H_WED
#include <glm/glm.hpp>
#include <memory>
#include <vector>

using std::unique_ptr;

//Classes que representam a implementação de winged edge
namespace tnw {
namespace wed {

	struct WEdge;

	struct Vertex {
		//Identifier
		size_t id;
		//Position
		glm::vec3 position;
		//Incident edge
		WEdge *iedge;
		Vertex(size_t _id, glm::vec3 _position, WEdge* _iedge);

		// void draw();
	};

	struct Loop {
		//Identifier
		size_t id;
		//Incident edge
		WEdge *iedge;

		Loop(size_t _id, WEdge *_iedge);

		//Returns the list of adjacent edges
		std::vector<WEdge*> adjedge();
		//Returns the list of adjacent vertices
		std::vector<Vertex*> adjvertex();
		//Returns the list of adjacent faces
		std::vector<Loop*> adjloop();

		// void draw();	
	};

	struct WEdge {
		//Identifier
		size_t id;
		//Start and end vertices
		Vertex *vstart, *vend;
		//Left and right faces
		Loop *lloop, *rloop;
		//Left predecessor and successor edges
		WEdge *lpred, *lsucc;
		//Right predecessor and successor edges
		WEdge *rpred, *rsucc;

		//Initialize only with id, fill other info later
		WEdge(size_t _id);
		//Initialize with all info
		WEdge(size_t _id, Vertex* _vstart, Vertex* _vend, Loop* _lloop, Loop* _rloop, WEdge* _lpred, WEdge* _lsucc, WEdge* _rpred, WEdge* _rsucc);
	

		// void draw();
	};

};

}

#endif
