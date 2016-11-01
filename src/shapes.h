#ifndef _H_SHAPES
#include "model.h"
#include <vector>
#include <tuple>

namespace tnw {

	class Sphere : public Shape {
	public:
		Sphere(glm::vec3 center, float radius);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		IntersectionList intersect_ray(const Ray&) const override;

		BoundingBox boundingBox() const override;
		double volume() const override;

		owner_ptr<Shape> clone() const override;
		std::string serialize() const override;
	private:
		glm::vec3 center;
		float radius;
	};

	class Box : public Shape {
	public:
		Box(glm::vec3 center, float length, float depth, float height);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		IntersectionList intersect_ray(const Ray&) const override;
		bool clip_line(int d, const Ray& ray, float& f_low, float& f_high) const;

		BoundingBox boundingBox() const override;
		double volume() const override;

		owner_ptr<Shape> clone() const override;
		std::string serialize() const override;
	private:
		glm::vec3 center;
		float length, depth, height;
	};

	class Cilinder : public Shape {
	public:
		Cilinder(glm::vec3 inferiorPoint, float height, float radius);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		IntersectionList intersect_ray(const Ray&) const override;

		BoundingBox boundingBox() const override;
		double volume() const override;

		owner_ptr<Shape> clone() const override;
		std::string serialize() const override;
	private:
		glm::vec3 inferiorPoint;
		float height, radius;
	};

	class SquarePyramid : public Shape {
	public:
		SquarePyramid(glm::vec3 inferiorPoint, float height, float basis);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		IntersectionList intersect_ray(const Ray&) const override;

		BoundingBox boundingBox() const override;
		double volume() const override;

		owner_ptr<Shape> clone() const override;
		std::string serialize() const override;
	private:
		glm::vec3 inferiorPoint;
		float height, basis;
	};

}
#define _H_SHAPES
#endif
