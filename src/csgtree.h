#ifndef _H_CSG
#define _H_CSG
#include "model.h"
#include "octree.h"
#include <memory>
#include <array>

using std::array;
using std::unique_ptr;

namespace tnw {
namespace csg {
	class Node : public Shape {
	public:
		virtual Color intersect_point(const glm::vec3&) const = 0;
		virtual Color intersect_box(const BoundingBox&) const = 0;
		IntersectionList intersect_ray(const Ray& ray) const override;
		double volume() const;
		virtual std::string serialize() const = 0;
		BoundingBox boundingBox() const = 0;
	};

	class AndNode : public Node {
		array<unique_ptr<Shape>,2> children;
	public:
		AndNode(unique_ptr<Shape>&&, unique_ptr<Shape>&&);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		std::string serialize() const override;
		BoundingBox boundingBox() const override;
	};

	class OrNode : public Node {
		array<unique_ptr<Shape>,2> children;
	public:
		OrNode(unique_ptr<Shape>&&, unique_ptr<Shape>&&);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		std::string serialize() const override;
		BoundingBox boundingBox() const override;
	};

	class ScaleNode : public Node {
		unique_ptr<Shape> child;
	public:
		float dv;
		ScaleNode(unique_ptr<Shape>&&,float);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		std::string serialize() const override;
		BoundingBox boundingBox() const override;
	};

	class TranslateNode : public Node {
		unique_ptr<Shape> child;
	public:
		glm::vec3 dx;
		TranslateNode(unique_ptr<Shape>&&,glm::vec3);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		std::string serialize() const override;
		BoundingBox boundingBox() const override;
	};

} // namespace tnw::csg

class CSGTree : public tnw::Model {
	unique_ptr<csg::Node> root;
	float color[3];
	void rdraw() override;

	Octree render_model;
	bool should_update;
public:
	CSGTree(unique_ptr<Shape>&&);
	CSGTree(owner_ptr<Shape>);
	// Shape
	// Geometric operations
	Color intersect_point(const glm::vec3&) const override;
	Color intersect_box(const BoundingBox&) const override;
	IntersectionList intersect_ray(const Ray& ray) const override;
	// Geometric analysis
	BoundingBox boundingBox() const override;
	double volume() const override;
	// Model
	virtual owner_ptr<Model> clone() const;
	// Geometric operations
	void translate(const glm::vec3& dv) override;
	void scale(const float dx) override;
	// Boolean operations
	BooleanErrorCodes bool_and(const Model& y) override;
	BooleanErrorCodes bool_or(const Model& y) override;
	//Serialize
	std::string serialize() const override;
	//Set color
	void setColor(const float c[3]) override;
	PaintColor getColor() const override;
};

} // namespace tnw
#endif
