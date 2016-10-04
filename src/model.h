#ifndef _H_MODEL
#define _H_MODEL
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <functional>
#include <memory>
#include "octree_internal.h"

namespace tnw {

enum class BooleanErrorCodes {
	success,
	unimplementedType,
	boundingboxMismatch
};

class Model {
private:
	virtual void rdraw() const = 0;
public:
	bool visible = true;
	void toggle() {visible = !visible;}
	void draw() const {if(visible) this->rdraw();}
	// Geometric operations
	virtual void translate(const glm::vec3& dv) = 0;
	virtual void scale(const float dx) = 0;
	// Boolean operations
	virtual BooleanErrorCodes bool_and(const Model& y) = 0;
	virtual BooleanErrorCodes bool_or(const Model& y) = 0;
	// Geometric analysis
	virtual double volume() const = 0;
	//Serialize
	virtual std::string serialize() const = 0;
	//Set color
	virtual void setColor(float c[3]) = 0;
};

class Octree : public Model {
public:
	//Octree com raiz vazia
	Octree(const octree::BoundingBox& _bb);
	//Octree com raiz pronta
	Octree(std::unique_ptr<octree::Tree> tree, const octree::BoundingBox& _bb);
	//Octree a partir de um classificador e uma Bounding Box
	Octree(octree::Classifier c, const octree::BoundingBox& _bb, unsigned int depth);
	//Octree a partir de um arquivo
	Octree(FILE *f);

	virtual void setColor(float c[3]) override;

	// Geometric operations
	virtual void translate(const glm::vec3& dv) override;
	virtual void scale(const float dx) override;
	// Boolean operations
	virtual BooleanErrorCodes bool_and(const Model& y) override;
	virtual BooleanErrorCodes bool_or(const Model& y) override;
	// Geometric analysis
	virtual double volume() const override;
	//Serialize
	virtual std::string serialize() const override;

private:
	std::unique_ptr<octree::Tree> tree;
	octree::BoundingBox bb;
	virtual void rdraw() const override;
};

} // namespace tnw
#endif
