#ifndef _H_HELPER
#define _H_HELPER
#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <tuple>

//Algumas funções de apoio
namespace tnw
{
	//Calcula se duas caixas intersecionam. Os valores são:
	//c - centro; l - largura; d - profundidade; h - altura
	bool box_intersection(glm::vec3 c1, double l1, double h1, double d1, glm::vec3 c2, double l2, double h2, double d2);
	//Calcula a menor distância de um ponto para algum ponto de uma caixa
	double squared_dist_point_box(glm::vec3 p, glm::vec3 c, double l, double h, double d);

	bool sphere_box_intersection(glm::vec3 s_center, double s_radius, glm::vec3 c, double l, double h, double d);

	constexpr double pi = 3.14159265358979323846;

	void draw_axis();
	glm::mat4 isometric(float aspect, float near, float far, bool positive_hor, bool positive_ver);

	using std::tuple;
	class Image {
	private:
		std::vector<float> data;
		size_t h,w;
	public:
		operator float*();
		float& operator()(size_t i, size_t j, size_t k);
		tuple<float&,float&,float&> operator()(size_t i, size_t j);
		float  operator()(size_t i, size_t j, size_t k) const;
		tuple<float,float,float> operator()(size_t i, size_t j) const;
		Image(size_t height, size_t width);
	};
}

#endif
