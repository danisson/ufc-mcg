#include "model.h"
#include "helper.h"
#include "interface.h"
#include "shapes.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <sstream>
#include <limits>
#include <glm/gtx/string_cast.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#undef near
#undef far
#endif

// struct Ray {
//     glm::vec3 o, dir, invdir;

//     Ray(glm::vec3 _o, glm::vec3 _dir){
//         o = _o;
//         dir = _dir;
//         invdir = glm::vec3(1,1,1)/_dir;
//     }
//     static Ray computePrimRay(glm::vec3 pixel, glm::vec3 eye) {
//         return Ray(pixel, pixel - eye);
//     }
// };

// class Sphere
// {
// public:
//     glm::vec3 center;                           /// position of the sphere
//     float radius, radius2;                  /// sphere radius and radius^2
//     glm::vec3 surfaceColor, emissionColor;      /// surface color and emission (light)
//     float transparency, reflection;         /// surface transparency and reflectivity
//     Sphere(
//         const glm::vec3 &c,
//         const float &r,
//         const glm::vec3 &sc) :
//         center(c), radius(r), radius2(r * r), surfaceColor(sc)
//     { /* empty */ }
//     bool intersect(const glm::vec3 &rayorig, const glm::vec3 &raydir) const
//     {
//         glm::vec3 l = center - rayorig;
//         float tca = glm::dot(l, raydir);
//         if (tca < 0) return false;
//         float d2 = glm::dot(l,l) - tca * tca;
//         if (d2 > radius2) return false;
//         float thc = sqrt(radius2 - d2);
//         return true;
//     }
// };

// struct BoxS {
//     //Ponto máximo e mínimo da caixa
//     glm::vec3 min, max;
//     float color[3];

//     BoxS(glm::vec3 _min, glm::vec3 _max){
//         min = _min;
//         max = _max;
//         color[0] = 0.0f; color[1] = 0.0f; color[2] = 1.0f;
//     }

//     BoxS(glm::vec3 _min, glm::vec3 _max, float _color[3]){
//         min = _min;
//         max = _max;
//         color[0] = _color[0];
//         color[1] = _color[1];
//         color[2] = _color[2];
//     }

//     bool intersect(Ray &r) {
//         float tmin , tmax,
//               tymin, tymax,
//               tzmin, tzmax;

//         //Plano x
//         if (r.invdir.x > 0) {
//             tmin = (min.x - r.o.x) * r.invdir.x;
//             tmax = (max.x - r.o.x) * r.invdir.x;
//         } else {
//             tmin = (max.x - r.o.x) * r.invdir.x;
//             tmax = (min.x - r.o.x) * r.invdir.x;
//         }
//         //Plano y
//         if (r.invdir.y > 0) {
//             tymin = (min.y - r.o.y) * r.invdir.y;
//             tymax = (max.y - r.o.y) * r.invdir.y;
//         } else {
//             tymin = (max.y - r.o.y) * r.invdir.y;
//             tymax = (min.y - r.o.y) * r.invdir.y;
//         }

//         if (tmin > tymax || tymin > tmax) {
//             return false;
//         }
//         if (tymin > tmin) {
//             tmin = tymin;
//         }
//         if (tymax < tmax) {
//             tmax = tymax;
//         }

//         //Plano z
//         if (r.invdir.z > 0) {
//             tzmin = (min.z - r.o.z) * r.invdir.z;
//             tzmax = (max.z - r.o.z) * r.invdir.z;
//         } else {
//             tzmin = (max.z - r.o.z) * r.invdir.z;
//             tzmax = (min.z - r.o.z) * r.invdir.z;
//         }

//         if (tmin > tzmax || tzmin > tmax) {
//             return false;
//         }
//         if (tzmin > tmin) {
//             tmin = tzmin;
//         }
//         if (tzmax < tmax) {
//             tmax = tzmax;
//         }

//         return true;
//     }
// };

void key_callback(GLFWwindow*, int, int, int, int);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//Camera initalization
IsometricCamera camera;
double xpos, ypos;
unsigned int swidth, sheight;
bool imgChanged = false, genImg = false;

struct Raycast
{
	int width, height;
	float dx, dy, near, far;

	Raycast(int _width, int _height, float _near, float _far) : width(_width), height(_height), near(_near), far(_far) {
		dx = 1/_width;
		dy = 1/_height; 
	}
};

// std::vector<BoxS> boxes;
// std::vector<Sphere> spheres = {Sphere(glm::vec3(0,0,-3), 0.5, glm::vec3(0,1,0))};
int main(void) {
	GLFWwindow* window;
	std::vector<std::unique_ptr<tnw::Model>> models;

	// Initialize the library
	if (!glfwInit())
		return 1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Sickle", NULL, NULL);
	swidth = 640;
	sheight = 480;
	if (!window) {
		glfwTerminate();
		return 1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Init ImGui
	ImGui_ImplGlfw_Init(window, true);
	// Disable .ini
	ImGui::GetIO().IniFilename = nullptr;

	//Opções de OpenGL
	glClearColor(0.,0.,0.,0.);
	// glEnable(GL_LINE_SMOOTH);
	// glEnable(GL_CULL_FACE);
	// glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	auto w = 640;
	auto h = 480;
	Raycast rc(2,2,1,-100);
	tnw::Image img(rc.width,rc.height);

	// for (int i = 0; i < rc.width; ++i) {
	// 	for (int j = 0; j < rc.height; ++j) {
	// 		if (j%5==0) 
	// 			img(i,j) = std::make_tuple(0,1,1);
	// 		else 
	// 			img(i,j) = std::make_tuple(1,0,1);
	// 	}
	// }
	img(0,0) = std::make_tuple(1,0,0);
	img(0,1) = std::make_tuple(0,1,0);
	img(1,0) = std::make_tuple(0,0,1);
	img(1,1) = std::make_tuple(1,0,1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rc.width, rc.height, 0, GL_RGB, GL_FLOAT, img);

	//Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	camera.aspect = 480/640.;
	MainMenu mainMenu(models,camera);

	std::vector<tnw::Shape*> scene;
	tnw::Shape *obj1 = new tnw::Sphere(glm::vec3(0,0,-1),0.5); 
	scene.push_back(obj1);
	scene.push_back(new tnw::Box(glm::vec3(4,1,-3),1,1,1));

	std::cout << "SPHERE TEST\n";
	tnw::Ray r(glm::vec3(0,0,1), glm::vec3(0,0,-100));
	tnw::IntersectionList il5 = obj1->intersect_ray(r);
	for (std::tuple<tnw::Color, float> ilel : il5) {
		tnw::Color c;
		float f;
		std::tie(c,f) = ilel;
		std::cout << "color: " << (int)c << " length: " << f << std::endl;
	}
	std::cout << "==================\n";


	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		ImGui::SetNextWindowSize(ImVec2(350,350), ImGuiSetCond_FirstUseEver);
		// mainMenu.draw();
		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (genImg) {
			std::cout << "Generating image...\n";
			std::flush(std::cout);
			// std::cout << "Generating image...";
			// std::flush(std::cout);
			genImg = false;

			for (int i = 0; i < rc.width; ++i)
			{
				for (int j = 0; j < rc.height; ++j)
				{
					float pospixx, pospixy;
					pospixx = -2.f*(1.f/2.f - ((float)i)/rc.width);
					pospixy = 2.f*(1.f/2.f - ((float)j)/rc.height);
					glm::vec3 a(pospixx, pospixy, rc.near);
					glm::vec3 b(pospixx, pospixy, rc.far);
					
					tnw::Ray r = tnw::Ray(a,b);

					float minInter = rc.far;
					int drawIndice = -1;
					for (int k = 0; k < scene.size(); k++) {
						tnw::IntersectionList ilist = scene[k]->intersect_ray(r);
						if (ilist.size() >= 2) {
							if (minInter < std::abs(std::get<1>(ilist[0]))) {
								minInter = std::abs(std::get<1>(ilist[0]));
								drawIndice = k;
							}
						} 
					}

					if (drawIndice >= 0) {

						std::cout << "\ni: " << i << " j: " << j << " drawIndice: " << drawIndice;
						tnw::IntersectionList ilist = scene[0]->intersect_ray(r);
						std::cout << "\nilist size: " << ilist.size();
						std::cout << "\na: " << glm::to_string(a) << "\nb: " << glm::to_string(b) << "\n";

						img(i,j) = std::make_tuple(0,0,0);
					}
				}
			}

			// for (unsigned i = 0; i < swidth; i++) {
			// 	for (unsigned j = 0; j < sheight; j++) {
			// 		glm::vec3 unprojectedCoords = glm::unProject(glm::vec3(i,j,0), glm::mat4(1.0f), glm::mat4(1.0f), glm::vec4(0,0,swidth,sheight));
			// 		Ray primRay = Ray::computePrimRay(unprojectedCoords, glm::vec3(0,0,0));

			// 		for (BoxS& box : boxes) {
			// 			if (box.intersect(primRay)) {
			// 				std::cout << "box intersect\n";
			// 				std::cout << "unproject: " << unprojectedCoords[0] << " " << unprojectedCoords[1] << " " << unprojectedCoords[2] << "\n";
			// 				std::cout << "primary ray:\n \torigin: " << primRay.o.x << " " << primRay.o.y << " " << primRay.o.z << "\n\t direction: " << primRay.dir.x << " " << primRay.dir.y << " " << primRay.dir.z << "\n";
			// 				std::flush(std::cout);
			// 				img(i,j) = std::make_tuple(box.color[0], box.color[1], box.color[2]);
			// 			} else {
			// 				img(i,j) = std::make_tuple(1,0,0);
			// 			}
			// 		}
			// 	}
			// }
			glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, rc.width, rc.height, GL_RGB, GL_FLOAT, img);
			std::cout << " generated image\n";
			std::flush(std::cout);
		}

		glBegin(GL_QUADS);
		glTexCoord2d(0, 1);
		glVertex2f(-1.,-1.);
		glTexCoord2d(1, 1);
		glVertex2f( 1.,-1.);
		glTexCoord2d(1, 0);
		glVertex2f( 1., 1.);
		glTexCoord2d(0, 0);
		glVertex2f(-1., 1.);
		glEnd();

		ImGui::Render();
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	glDeleteTextures(1, &tex);
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	} else
		ImGui_ImplGlFw_KeyCallback(window, key, scancode, action, mods);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	camera.aspect = height/((float)width);
	glViewport(0,0,width,height);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {

	}
}
