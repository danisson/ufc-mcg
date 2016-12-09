#ifndef _H_WED
#define _H_WED
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <set>
#include <tuple>
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
		//Clockwise and Counterclockwise faces
		Loop *cwloop, *ccwloop;
		//Clockwise predecessor and successor edges
		WEdge *cwpred, *cwsucc;
		//Counterclockwise predecessor and successor edges
		WEdge *ccwpred, *ccwsucc;

		//Initialize only with id, fill other info later
		WEdge(size_t _id);
		//Initialize with all info
		WEdge(size_t id,
		      Vertex* vstart, Vertex* vend,
		      Loop* cwloop, Loop* ccwloop,
		      WEdge* cwpred, WEdge* cwsucc,
		      WEdge* ccwpred, WEdge* ccwsucc);

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
	//Helper function to geometric transforms
	void applyTransform(const glm::mat4& t);

	size_t currVertexId = 1;
	size_t currEdgeId = 1;
	size_t currLoopId = 1;

	// Wireframe color
	float color[3] = {.5,.5,.5};
	std::set<std::tuple<size_t,char>> marked;
	size_t selected_edge = 0;

public:
	BRep();
	// Geometric operations
	//Intersections won't be implemented
	Color intersect_point(const glm::vec3&) const override;
	Color intersect_box(const BoundingBox&) const override;
	IntersectionList intersect_ray(const Ray&) const override;
	void translate(const glm::vec3& dv) override;
	void scale(const float dx) override;

	// Geometric analysis
	BoundingBox boundingBox() const override;
	double volume() const override;

	// Boolean operations (Won't be implemented)
	BooleanErrorCodes bool_and(const Model& y) override;
	BooleanErrorCodes bool_or(const Model& y) override;

	//Serialize
	// virtual std::string serialize() const = 0;

	//Set color
	void setColor(const float c[3]) override;
	PaintColor getColor() const override;

	// Misc
	// virtual owner_ptr<Model> clone() const = 0;

	//Get operators
	wed::Loop* get_loop(size_t id);
	wed::WEdge* get_wedge(size_t id);
	wed::Vertex* get_vertex(size_t id);

	//Euler operators
	void mvfs(glm::vec3 position);
	void smef(size_t fid, size_t v1id, size_t v2id);
	void mvfs(size_t vid, size_t eid, size_t fid, glm::vec3 position);
};

} // tnw

#endif
