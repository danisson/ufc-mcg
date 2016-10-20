#ifndef _H_SHAPES
#include "model.h"
#include <vector>
#include <tuple>

namespace tnw {

	//Lista de interseções é representada como um par <Cor>, <Comprimento dessa cor no raio>
	typedef std::vector<std::tuple<Color, float>> IntersectionList;

	class Sphere : public Shape {
	public:
		Sphere(glm::vec3 center, float radius);
		Color intersect_point(const glm::vec3&) const override;
		Color intersect_box(const BoundingBox&) const override;
		IntersectionList intersect_ray(const Ray&) const override;
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
		IntersectionList intersect_ray(const Ray&) const override;
		bool clip_line(int, const Ray&, float& f_low, float& f_high);
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
		IntersectionList intersect_ray(const Ray&) const override;
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
		IntersectionList intersect_ray(const Ray&) const override;
		double volume() const override;
	private:
		glm::vec3 inferiorPoint;
		float height, basis;
	};

}
#define _H_SHAPES
#endif
