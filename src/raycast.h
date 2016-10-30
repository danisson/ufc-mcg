#ifndef _H_RAYCAST
#define _H_RAYCAST

#include "interface.h"
#include "model.h"
#include "helper.h"
#include <GL/gl.h>

using std::unique_ptr;

namespace tnw {

class Raycast {
public:
	Raycast(std::vector<unique_ptr<Model>>& _models, IsometricCamera &_camera, size_t _width, size_t _height, GLuint _texId);
	void generateRays();
	void paintImage();
private:
	std::vector<unique_ptr<Model>>& models;
	IsometricCamera &camera;
	size_t width;
	size_t height;
	GLuint texId;
	Image image;
	std::vector<std::vector<Ray>> rays;
};

}
#endif
