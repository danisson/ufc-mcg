#ifndef _H_INTERFACE
#define _H_INTERFACE
#include <imgui.h>
#include "model.h"
#include <vector>
#include <memory>

/* Classes e funções relativas à interface */

struct IsometricCamera {
	float scale = 0.5, aspect = 1, near = -10.0, far = 10.0;
	bool positive_hor = true, positive_ver = true;
	glm::vec3 pos = {0,0,0};
};

class MainMenu {
	std::vector<std::unique_ptr<tnw::Model>>& models;
	std::vector<std::string> model_names;
	IsometricCamera& camera;
	uint8_t rot = 0;
	const uint8_t rots[4] = {3,1,0,2};
	int md = 4, red = 0, green = 0, blue = 0;
	float x=0.5, y=0, z=.5, r=.5, bx=0, by=0, bz=0, bd=1, h=1, l=1, d=1, color[3] = {0,0,0.7};
	bool open_type_error_popup = false, open_bb_mismatch_error_popup = false;
	char buffer[1000];
public:
	MainMenu(std::vector<std::unique_ptr<tnw::Model>>& m, IsometricCamera& c);
	~MainMenu(){};
	void draw();
};

#endif
