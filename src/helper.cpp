#include "helper.h"
#include <glm/gtx/string_cast.hpp>
#include <GL/gl.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#undef near
#undef far
#endif

bool tnw::box_intersection(glm::vec3 c1, double l1, double h1, double d1, glm::vec3 c2, double l2, double h2, double d2){
	bool x = std::abs(c1[0] - c2[0]) < (l1/2.0 + l2/2.0);
	bool y = std::abs(c1[1] - c2[1]) < (h1/2.0 + h2/2.0);
	bool z = std::abs(c1[2] - c2[2]) < (d1/2.0 + d2/2.0);

	return x && y && z;
}

double tnw::squared_dist_point_box(glm::vec3 p, glm::vec3 c, double l, double h, double d){

	const auto check = [] (const double pn, const double bmin, const double bmax) -> double {
		double out = 0;
		if (pn < bmin) {
			double val = (bmin - pn);
			out += val * val;
		}
		if (pn > bmax) {
			double val = (pn - bmax);
			out += val * val;
		}
		return out;
	};

	glm::vec3 minPoint = c - glm::vec3(l/2, h/2 , d/2);
	glm::vec3 maxPoint = c + glm::vec3(l/2, h/2 , d/2);
	double sq = 0;

	sq += check(p[0], minPoint[0], maxPoint[0]);
	sq += check(p[1], minPoint[1], maxPoint[1]);
	sq += check(p[2], minPoint[2], maxPoint[2]);

	return sq;

}

bool tnw::sphere_box_intersection(glm::vec3 s_center, double s_radius, glm::vec3 c, double l, double h, double d) {
	double squaredDistance = squared_dist_point_box(s_center, c, l, h, d);
	return squaredDistance < (s_radius * s_radius);
}

void tnw::draw_axis(){
	glBegin(GL_LINES);
		glColor3f(1.000000f, 0.000000f, 0.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(1, 0, 0);
		glColor3f(0.000000f, 1.000000f, 0.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 1, 0);
		glColor3f(0.000000f, 0.000000f, 1.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 1);
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

glm::mat4 tnw::isometric(float aspect, float near, float far, bool positive_hor, bool positive_ver) {
	float rot_y = glm::radians(45.0);
	float rot_x = std::asin(std::tan(glm::radians(30.0f)));

	if (!positive_hor){
		// rot_x = -rot_x;
		rot_y = glm::radians(3*45.0);
	}
	if (!positive_ver){
		rot_y = -rot_y;
	}

	glm::mat4 a = glm::rotate(glm::mat4(), rot_y, glm::vec3(0.0,1.0,0.0));
	glm::mat4 b = glm::rotate(glm::mat4(), rot_x, glm::vec3(1.0,0.0,0.0));

	glm::mat4 o = glm::ortho(-1.f, 1.f, -1*aspect, 1*aspect, near, far);

	return o*b*a;
}
