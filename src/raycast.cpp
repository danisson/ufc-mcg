#include "raycast.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#undef near
#undef far
using std::unique_ptr;
tnw::Raycast::Raycast(std::vector<unique_ptr<Model>>& _models, IsometricCamera &_camera, size_t _width, size_t _height, GLuint _texId) :
	models(_models), camera(_camera), width(_width), height(_height), texId(_texId), image(width, height)
{
	glBindTexture(GL_TEXTURE_2D, texId);

	rays.resize(width);
	for (size_t i = 0; i < width; i++) {
		rays[i].reserve(height);
	}
	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			image(i,j) = std::make_tuple(0,0,1);
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, image);
	// std::cout << "================================================\n";
	generateRays();
	paintImage();
	// std::cout << "================================================\n";
}

void tnw::Raycast::generateRays() {
	auto dx = 2.0/width,
		 dy = 2.0/height;
	for (size_t j = 0; j < height; ++j) {
		for (size_t i = 0; i < width; ++i) {

			auto x = -2.f*(1.f/2.f - float(i)/width);
			auto y = 2.f*(1.f/2.f - float(j)/height);

			float pospixx = x+dx/2;
			float pospixy = y-dy/2;

			// std::cout << "untransf a: " << glm::to_string(glm::vec3(pospixx, pospixy, camera.near)) << " unstransf b: " << glm::to_string(glm::vec3(pospixx, pospixy, camera.far)) << std::endl;

			auto cameraMatrix = glm::mat4(1/camera.scale);
			cameraMatrix = glm::translate(glm::mat4(),-camera.pos) * cameraMatrix;
			cameraMatrix = glm::inverse(tnw::isometric(camera.aspect, -camera.near, -camera.far, camera.positive_hor, camera.positive_ver)) * cameraMatrix;

			// auto aM = glm::rotate(glm::mat4(), glm::radians(-45.0f), glm::vec3(0.0,1.0,0.0));
			// auto bM = glm::rotate(glm::mat4(), -std::asin(std::tan(glm::radians(30.0f))), glm::vec3(1.0,0.0,0.0));
			// cameraMatrix = aM*bM*cameraMatrix;

			glm::vec4 a = cameraMatrix * glm::vec4(pospixx, pospixy, camera.near, 1.0f);
			glm::vec4 b = cameraMatrix * glm::vec4(pospixx, pospixy, camera.far, 1.0f);

			// std::cout << "i: " << i << " j: " << j << "\n";
			// std::cout << "a: " << glm::to_string(a) << "b: " << glm::to_string(b) << std::endl;

			// tnw::Ray r = tnw::Ray(a,b);
			// rays[i][j] = r;
			rays[i].push_back(tnw::Ray(glm::vec3(a),glm::vec3(b)));
		}
	}
}

void tnw::Raycast::paintImage() {

	for (size_t j = 0; j < height; ++j) {
		for (size_t i = 0; i < width; ++i) {
			PaintColor paintColor = {0.0, 0.0, 0.0};
			float maxDist = glm::length(rays[i][j].dir);
			// std::cout << "maxDist: " << maxDist << "\n";
			std::tuple<tnw::Color, float> minInter = std::make_tuple(tnw::Color::white, maxDist);
			// printf("%zu,%zu\n",i,j);
			for (size_t k = 0; k < models.size(); k++) {
				tnw::IntersectionList ilist = models[k]->intersect_ray(rays[i][j]);

				assert(ilist.size() > 0);
				switch (std::get<0>(ilist[0])) {
					case tnw::Color::black:
					case tnw::Color::gray:
						if (std::get<1>(minInter) > std::get<1>(ilist[0])) {
							minInter = ilist[0];
							paintColor = models[k]->getColor();
						}
						break;
					case tnw::Color::white:
						if (std::get<1>(ilist[0]) < maxDist && std::get<1>(minInter) > std::get<1>(ilist[0])) {
							minInter = ilist[0];
							paintColor = models[k]->getColor();
						}
				}
			}
			image(i,j) = std::make_tuple(paintColor[0], paintColor[1], paintColor[2]);
		}
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, width, height, GL_RGB, GL_FLOAT, image);
}
