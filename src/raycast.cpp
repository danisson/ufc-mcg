#include "raycast.h"
#undef near
#undef far
using std::unique_ptr;
tnw::Raycast::Raycast(std::vector<unique_ptr<Model>>& _models, IsometricCamera _camera, size_t _width, size_t _height, GLuint _texId) :
	models(_models), camera(_camera), width(_width), height(_height), texId(_texId), image(width, height)
{
	glBindTexture(GL_TEXTURE_2D, texId);

	rays.resize(width);
	for (size_t i = 0; i < width; i++) {
		rays[i].reserve(height);
	}
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, image);
	generateRays();
	paintImage();
}

void tnw::Raycast::generateRays() {
	for (size_t j = 0; j < height; ++j) {
		for (size_t i = 0; i < width; ++i) {

			auto nx = -2.f*(1.f/2.f - float(i+1)/width);
			auto ny = 2.f*(1.f/2.f - float(j+1)/height);
			auto x = -2.f*(1.f/2.f - float(i)/width);
			auto y = 2.f*(1.f/2.f - float(j)/height);

			float pospixx = x+nx/2;
			float pospixy = y+ny/2;

			glm::vec3 a(pospixx, pospixy, camera.near);
			glm::vec3 b(pospixx, pospixy, camera.far);
			// std::cout << glm::to_string(a) << std::endl;

			// tnw::Ray r = tnw::Ray(a,b);
			// rays[i][j] = r;
			rays[i].push_back(tnw::Ray(a,b));
		}
	}
}

void tnw::Raycast::paintImage() {
	for (size_t j = 0; j < height; ++j) {
		for (size_t i = 0; i < width; ++i) {
			float minInter = camera.far;
			PaintColor paintColor = {0.0, 0.0, 0.0};
			for (size_t k = 0; k < models.size(); k++) {
				tnw::IntersectionList ilist = models[k]->intersect_ray(rays[i][j]);
				for (size_t l = 0; l < ilist.size(); l++) {
					tnw::Color modelColor = std::get<0>(ilist[l]);
					if (modelColor == tnw::Color::black || modelColor == tnw::Color::gray) {
						float intersDist = std::get<1>(ilist[l]);
						if (intersDist < minInter) {
							minInter = intersDist;
							paintColor = models[k]->getColor();
							break;
						}
					}
				}
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0 ,0, 0, width, height, GL_RGB, GL_FLOAT, image);
}
