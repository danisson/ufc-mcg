#include "model.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>

double colorTable[3][3] = {{0.1, 0.2, 0.3}, {0.2, 0.3, 0.1}, {0.3, 0.1, 0.2}};
int currentColor = 0;

void key_callback(auto window, auto key, auto scancode, auto action, auto mods) {
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		currentColor++;
		if (currentColor > 2)
			currentColor = 0;
	} if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)  {
		currentColor--;
		if (currentColor < 0)
			currentColor = 2;
	}
	ImGui_ImplGlFw_KeyCallback(window,key,scancode,action,mods);
}

int main(void) {
	GLFWwindow* window;

	// Initialize the library
	if (!glfwInit())
		return 1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return 1;
	}

	// Init ImGui
	ImGui_ImplGlfw_Init(window, true);
	// Disable .ini
	ImGui::GetIO().IniFilename = nullptr;
	// Make the window's context current
	glfwMakeContextCurrent(window);

	float angle = 0.;
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		glfwSetKeyCallback(window, key_callback);

		// Render here
		glClear(GL_COLOR_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);
		// glBegin(GL_TRIANGLES);
		// glColor3f(colorTable[currentColor][0], colorTable[currentColor][1], colorTable[currentColor][2]);
		// glVertex3f(0, 0, 0);
		// //glColor3f(0.4, 0.5, 0.6);
		// glVertex3f(1, 0, 0);
		// //glColor3f(0.7, 0.8, 0.9);
		// glVertex3f(0, 1, 0);
		// glEnd();

		angle = (angle < 360)? angle + 0.5 : 0;
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		glPushMatrix();
		glRotatef(angle, 0., 1., 0.);
		//Outro render
		tnw::octree::BoundingBox bb = tnw::octree::BoundingBox(glm::vec3(0.,0.,0.), 0.5);
		bb.draw();
		glPopMatrix();

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		
		if (ImGui::Button("Red")){
			glClearColor(1.,0.,0.,1.);
		}
		if (ImGui::Button("Green")){
			glClearColor(0.,1.,0.,1.);
		}
		if (ImGui::Button("Blue")){
			glClearColor(0.,0.,1.,1.);
		}

		ImGui::Text("Hello, world!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Mouse position: %.3f x %.3f y", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

		//ImGui::ShowTestWindow();

		// Swap front and back buffers
		ImGui::Render();
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}
