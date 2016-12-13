#ifndef _H_INTERFACE
#define _H_INTERFACE
#include <imgui.h>
#include "model.h"
#include "helper.h"
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#undef near
#undef far

/* Classes e funções relativas à interface */

struct IsometricCamera {
	float scale = 0.5, aspect = 1, near = 10.0, far = -10.0;
	bool positive_hor = true, positive_ver = true;
	glm::vec3 pos = {0,0,0};
};

class MainMenu {
	std::vector<std::unique_ptr<tnw::Model>>& models;
	std::vector<std::string> model_names;
	IsometricCamera& camera;
	IsometricCamera raycastCamera;
	uint8_t rot = 0;
	const uint8_t rots[4] = {3,1,0,2};
	int md = 4, red = 0, green = 0, blue = 0;
	float x=0.5, y=0, z=.5, r=.5, bx=0, by=0, bz=0, bd=1, h=1, l=1, d=1, color[3] = {0,0,0.7};
	int raycast_width = 200, raycast_height = 200;
	GLuint tex;
	int brep_ids[5] = {1,1,2,3,4};

	double volumeCache;
	bool open_type_error_popup = false, open_bb_mismatch_error_popup = false;
	char buffer[1000] = "./models/igreja.csg";
	const char *esconder_text = "Esconder##toggle";
	const char *mostrar_text = "Mostrar##toggle";

	void errorDialog(char* name, char* msg);
	void errorDialog(const char* name, const char* msg);

	static void renderWindow(const GLuint& t, size_t w, size_t h, bool& should_close);
	bool render_show = false;
public:
	MainMenu(std::vector<std::unique_ptr<tnw::Model>>& m, IsometricCamera& c);
	~MainMenu(){};
	void draw();
};

#endif
