#ifndef _H_SHAPES
#include "model.h"


namespace tnw {
	class Sphere : public Shape {
	public:
		Sphere(glm::vec3 center, float radius);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		double volume() const override;
	private:
		glm::vec3 center;
		float radius;
	};

	class Box : public Shape {
	public:
		Box(glm::vec3 center, float length, float depth, float height);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		double volume() const override;
	private:
		glm::vec3 center;
		float length, depth, height;
	};

	class Cilinder : public Shape {
	public:
		Cilinder(glm::vec3 inferiorPoint, float height, float radius);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		double volume() const override;
	private:
		glm::vec3 inferiorPoint;
		float height, radius;
	};

	class SquarePyramid : public Shape {
	public:
		SquarePyramid(glm::vec3 inferiorPoint, float height, float basis);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		double volume() const override;
	private:
		glm::vec3 inferiorPoint;
		float height, basis;
	};
}
#define _H_SHAPES
#endif
