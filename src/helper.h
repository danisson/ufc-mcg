#ifndef _H_HELPER
#define _H_HELPER
#include <glm/glm.hpp>
#include <cmath>

//Algumas funções de apoio
namespace tnw
{
	//Calcula se duas caixas intersecionam. Os valores são:
	//c - centro; l - largura; d - profundidade; h - altura
	bool box_intersection(glm::vec3 c1, double l1, double h1, double d1, glm::vec3 c2, double l2, double h2, double d2);
}

#endif
