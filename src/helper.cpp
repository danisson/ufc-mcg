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

float tnw::seg_to_seg_dist(glm::vec3 s0, glm::vec3 s1, glm::vec3 t0, glm::vec3 t1, float epsilon) {
	glm::vec3 u = s1-s0;
	glm::vec3 v = t1-t0;
	glm::vec3 w = s0-t0;
	float a = glm::dot(u,u);
	float b = glm::dot(u,v);
	float c = glm::dot(v,v);
	float d = glm::dot(u,w);
	float e = glm::dot(v,w);
	float D = a*c-b*b;
	float sc, sN, sD = D;
	float tc, tN, tD = D;

	if (D < epsilon) {
		sN = 0.0;
		sD = 1.0;
		tN = e;
		tD = c;
	} else {
		sN = (b*e-c*d);
		tN = (a*e-b*d);
		if (sN < 0.0) {
			sN = 0.0;
			tN = e;
			tD = c;
		} else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}
	if (tN < 0.0) {
		tN = 0.0;
		if (-d < 0.0) {
			sN = 0.0;
		} else if (-d > a) {
			sN = sD;
		} else {
			sN = -d;
			sD = a;
		}
	} else if (tN > tD) {
		tN = tD;
		if ((-d+b) < 0.0) {
			sN = 0.0;
		} else if ((-d+b) > a) {
			sN = sD;
		} else {
			sN = -d+b;
			sD = a;
		}
	}

	sc = (std::abs(sN) < epsilon)? 0.0 : (sN/sD);
	tc = (std::abs(tN) < epsilon)? 0.0 : (tN/tD);
	glm::vec3 dP = w + sc*u - tc*w;

	return glm::length(dP);
}

glm::vec3 tnw::ray_tri_intersection(const tnw::Ray& ray, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) {
	glm::vec3 d = glm::normalize(ray.dir),
			  e1 = v1-v0,
			  e2 = v2-v0,
			  t = ray.a-v0,
			  p = glm::cross(d, e2),
			  q = glm::cross(t, e1);
	glm::vec3 res = (1/glm::dot(p, e1))*glm::vec3(glm::dot(q, e2), glm::dot(p, t), glm::dot(q, d));
	return res;
}

bool tnw::ray_on_plane(const tnw::Ray& ray, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const float epsilon) {
	glm::vec3 l1 = v1-v0,
			  l2 = v2-v0,
			  n = glm::cross(l1, l2);
	bool on = (std::abs(0.0-glm::dot(ray.dir, n)) < epsilon);
	return on;
}

tnw::IntersectionList tnw::removeZeroIntersections(const tnw::IntersectionList& ilist) {
	// IntersectionList r;
	// for (unsigned i = 0; i < ilist.size(); i++) {
	// 	if (std::abs(0.0 - std::get<1>(ilist[i])) > 0.0000001) {
	// 		r.push_back(ilist[i]);
	// 	}
	// }
	// return r;
	return ilist;
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

	glm::mat4 o = glm::ortho(-1.f, 1.f, -1*aspect, 1*aspect, -near, -far);
	return o*b*a;
}

tnw::Image::Image(size_t w, size_t h) : h(h), w(w) {
	data.resize(h*w*3,0);
}

tnw::Image::operator float*() {
	return data.data();
}

// float& tnw::Image::operator()(size_t i, size_t j, size_t k) {
// 	return data[k + j*3 + i*w*3];
// }
// float  tnw::Image::operator()(size_t i, size_t j, size_t k) const{
// 	return data[k + j*3 + i*w*3];
// }

// std::tuple<float&,float&,float&> tnw::Image::operator()(size_t i, size_t j) {
// 	return std::tie(data[  j*3+i*w*3],
// 	                data[1+j*3+i*w*3],
// 	                data[2+j*3+i*w*3]);
// }

// std::tuple<float,float,float> tnw::Image::operator()(size_t i, size_t j) const {
// 	return std::make_tuple(data[  j*3+i*w*3],
// 	                       data[1+j*3+i*w*3],
// 	                       data[2+j*3+i*w*3]);
// }

float& tnw::Image::operator()(size_t i, size_t j, size_t k) {
	return data[k + i*3 + j*w*3];
}
float  tnw::Image::operator()(size_t i, size_t j, size_t k) const{
	return data[k + i*3 + j*w*3];
}

std::tuple<float&,float&,float&> tnw::Image::operator()(size_t i, size_t j) {
	return std::tie(data[  i*3+j*w*3],
	                data[1+i*3+j*w*3],
	                data[2+i*3+j*w*3]);
}

std::tuple<float,float,float> tnw::Image::operator()(size_t i, size_t j) const {
	return std::make_tuple(data[  i*3+j*w*3],
	                       data[1+i*3+j*w*3],
	                       data[2+i*3+j*w*3]);
}
