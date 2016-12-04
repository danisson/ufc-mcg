#ifndef _H_WED
#define _H_WED
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <map>
#include "model.h"

using std::unique_ptr;

//Classes que representam a implementação de winged edge
namespace tnw {
namespace wed {

	struct WEdge;
	struct Loop;

	struct Vertex {
		//Identifier
		size_t id;
		//Position
		glm::vec3 position;
		//Incident edge
		WEdge *iedge;
		Vertex(size_t _id, glm::vec3 _position, WEdge* _iedge);

		//Returns the list of adjacent edges
		std::vector<WEdge*> adjedge();
		//Returns the list of adjacent vertices
		std::vector<Vertex*> adjvertex();
		//Returns the list of adjacent faces
		std::vector<Loop*> adjloop();

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
		WEdge(size_t id,
		      Vertex* vstart, Vertex* vend,
		      Loop* lloop, Loop* rloop,
		      WEdge* lpred, WEdge* lsucc,
		      WEdge* rpred, WEdge* rsucc);

		//Returns the list of adjacent edges
		std::vector<WEdge*> adjedge();
		//Returns the list of adjacent vertices
		std::vector<Vertex*> adjvertex();
		//Returns the list of adjacent faces
		std::vector<Loop*> adjloop();

		// void draw();
	};

} // tnw::wed

class BRep : public Model {
private:
	std::vector<wed::Vertex> vertices;
	std::vector<wed::WEdge> edges;
	std::vector<wed::Loop> loops;
	void rdraw() override;

	//We will need this to assign ids to new objects
	size_t currVertexId = 0;
	size_t currEdgeId = 0;
	size_t currLoopId = 0;
public:
	BRep();
	// Geometric operations
	// virtual Color intersect_point(const glm::vec3&) const = 0;
	// virtual Color intersect_box(const BoundingBox&) const = 0;
	// virtual IntersectionList intersect_ray(const Ray&) const = 0;
	// virtual void translate(const glm::vec3& dv) = 0;
	// virtual void scale(const float dx) = 0;
	
	// Geometric analysis
	// virtual BoundingBox boundingBox() const = 0;
	// virtual double volume() const = 0;
	
	// Boolean operations
	// virtual BooleanErrorCodes bool_and(const Model& y) = 0;
	// virtual BooleanErrorCodes bool_or(const Model& y) = 0;
	
	//Serialize
	// virtual std::string serialize() const = 0;
	
	//Set color
	// virtual void setColor(const float c[3]) = 0;
	// virtual PaintColor getColor() const = 0;
	
	// Misc
	// virtual std::string serialize() const = 0;
	// virtual owner_ptr<Model> clone() const = 0;
	
	//Get operators
	wed::Loop& getLoop(size_t id);
	wed::WEdge& getWEdge(size_t id);
	wed::Vertex& getVertex(size_t id);

	//Euler operators
	void mvfs(glm::vec3 position);
};

} // tnw

#endif
