#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>

int main(void) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return 1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return 1;
	}

	/* Init ImGui */
	ImGui_ImplGlfw_Init(window, true);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);
		glBegin(GL_TRIANGLES);
		glColor3f(0.1, 0.2, 0.3);
		glVertex3f(0, 0, 0);
		glColor3f(0.4, 0.5, 0.6);
		glVertex3f(1, 0, 0);
		glColor3f(0.7, 0.8, 0.9);
		glVertex3f(0, 1, 0);
		glEnd();

		{
			ImGui::Text("Hello, world!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		/* Swap front and back buffers */
		ImGui::Render();
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}
