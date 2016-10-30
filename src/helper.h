#ifndef _H_HELPER
#define _H_HELPER
#include "model.h"
#include "shapes.h"
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

	//Calcula a distância entre dois segmentos, cada um definido por dois pontos
	float seg_to_seg_dist(glm::vec3 s0, glm::vec3 s1, glm::vec3 t0, glm::vec3 t1, float epsilon = 0.000001);

	//Calcula a interseção entre um raio e um triângulo com vértices v0, v1, e v2, o resultado é um vetor composto por
	// [t, u, v], t = distância, u,v = coordenadas no plano do triângulo, calculamos o ponto no triângulo a partir de 
	//T(u,v) = (1-u-v)V0 + uV1 + vV2
	glm::vec3 ray_tri_intersection(const tnw::Ray& r, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2);

	//Remove as interseções da lista que tem tamanho zero
	tnw::IntersectionList removeZeroIntersections(const tnw::IntersectionList& ilist);

	//Checa se um raio está em um plano determinado por 3 pontos
	bool ray_on_plane(const tnw::Ray& r, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const float epsilon = 0.000001);
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
		Image(size_t width, size_t height);
	};
}

#endif
