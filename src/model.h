#ifndef _H_MODEL
#define _H_MODEL
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <functional>
#include <memory>

namespace tnw {

template <typename T>
using owner_ptr = T*;

enum class BooleanErrorCodes {
	success,
	unimplementedType,
	boundingboxMismatch
};

enum class Color {
	white, black, gray
};

struct BoundingBox;
class Shape {
public:
	// Geometric operations
	virtual Color intersect_point(const glm::vec3&) const = 0;
	virtual Color intersect_box(const BoundingBox&) const = 0;
	// Geometric analysis
	virtual double volume() const = 0;
};

struct BoundingBox : public Shape {
	glm::vec3 corner;
	float depth;
	BoundingBox(glm::vec3 _corner, float _depth);
	void draw() const;
	void translate(const glm::vec3& dv);
	void scale(const float dx);
	BoundingBox operator[](size_t position) const;
	glm::vec3 getVertice(unsigned int i) const;
	glm::vec3 getCenter() const;
	glm::vec3 minPoint() const;
	glm::vec3 maxPoint() const;
	double volume() const override;
	Color intersect_point(const glm::vec3&) const override;
	Color intersect_box(const BoundingBox&) const override;
	BoundingBox least_boundingbox(const BoundingBox& bb) const;
	bool operator==(const BoundingBox& s) const;
	bool operator!=(const BoundingBox& s) const;
};

class Model : public Shape {
private:
	virtual void rdraw() const = 0;
public:
	bool visible = true;
	void toggle() {visible = !visible;}
	void draw() const {if(visible) this->rdraw();}
	virtual owner_ptr<Model> clone() const = 0;
	// Geometric operations
	virtual void translate(const glm::vec3& dv) = 0;
	virtual void scale(const float dx) = 0;
	// Boolean operations
	virtual BooleanErrorCodes bool_and(const Model& y) = 0;
	virtual BooleanErrorCodes bool_or(const Model& y) = 0;
	//Serialize
	virtual std::string serialize() const = 0;
	//Set color
	virtual void setColor(float c[3]) = 0;
};

} // namespace tnw
#endif
