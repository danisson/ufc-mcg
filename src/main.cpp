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
bool imgChanged = false, genImg = true;
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
	auto w = 6;
	auto h = 6;
	tnw::Image img(w,h);

	for (int i = 0; i < w; ++i) {
		for (int j = 0; j < h; ++j) {
			if (i%2 == 0)
				img(i,j) = std::make_tuple(1,1,1);
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, img);

	//Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	camera.aspect = 480/640.;
	MainMenu mainMenu(models,camera);
	tnw::Box b(glm::vec3(2.5,2.5,0), 3, 2, 5);
	tnw::Box b2(glm::vec3(-5,5,-5), 10, 10, 10);
	tnw::Ray r(glm::vec3(1,-1,0), glm::vec3(1,6,0));
	tnw::Ray r2(glm::vec3(1,-1,1), glm::vec3(-11,11,-11));
	tnw::Ray r3(glm::vec3(0,0,0), glm::vec3(6,0,0));
	tnw::IntersectionList il = b.intersect_ray(r);
	tnw::IntersectionList il2 = b2.intersect_ray(r2);
	tnw::IntersectionList il3 = b.intersect_ray(r3);

	for (std::tuple<tnw::Color, float> ilel : il) {
		tnw::Color c;
		float f;
		std::tie(c,f) = ilel;
		std::cout << "color: " << (int)c << " length: " << f << std::endl;
	}
	std::cout << "=======\n";
	for (std::tuple<tnw::Color, float> ilel : il2) {
		tnw::Color c;
		float f;
		std::tie(c,f) = ilel;
		std::cout << "color: " << (int)c << " length: " << f << std::endl;
	}
	std::cout << "=======\n";
	for (std::tuple<tnw::Color, float> ilel : il3) {
		tnw::Color c;
		float f;
		std::tie(c,f) = ilel;
		std::cout << "color: " << (int)c << " length: " << f << std::endl;
	}
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		ImGui::SetNextWindowSize(ImVec2(350,350), ImGuiSetCond_FirstUseEver);
		// mainMenu.draw();
		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (genImg) {
			// std::cout << "Generating image...";
			// std::flush(std::cout);
			// genImg = false;

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
			// glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, sheight, swidth, GL_RGB, GL_FLOAT, img);
			// std::cout << " generated image\n";
			// std::flush(std::cout);
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