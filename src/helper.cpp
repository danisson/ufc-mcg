#include "helper.h"

bool tnw::box_intersection(glm::vec3 c1, double l1, double h1, double d1, glm::vec3 c2, double l2, double h2, double d2){
	bool x = std::abs(c1[0] - c2[0]) < (l1/2.0 + l2/2.0);
	bool y = std::abs(c1[1] - c2[1]) < (h1/2.0 + h2/2.0);
	bool z = std::abs(c1[2] - c2[2]) < (d1/2.0 + d2/2.0);

	return x && y && z;
}
