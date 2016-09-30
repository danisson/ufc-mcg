#ifndef _H_INTERFACE
#define _H_INTERFACE
#include <imgui.h>
#include "model.h"
#include <vector>
#include <memory>

/* Classes e funções relativas à interface */

struct IsometricCamera {
	float scale = 2.0, near = -10.0, far = 10.0;
	bool positive_hor = true, positive_ver = true;
};

class MainMenu {
	std::vector<std::unique_ptr<tnw::Model>>& models;
	IsometricCamera& camera;
	uint8_t rot = 0;
	const uint8_t rots[4] = {3,1,0,2};
	int md = 4;
	float x=0.5, y=0, z=-.5, r=.5, bx=0, by=0, bz=0, bd=1, h=0, l=0, d=0;
public:
	MainMenu(std::vector<std::unique_ptr<tnw::Model>>& m, IsometricCamera& c);
	~MainMenu(){};
	void draw();
};

#endif
