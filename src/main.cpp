#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>

struct UIState
{
	int mousex;
	int mousey;
	int mousedown;

	int hotitem;
	int activeitem;
};


double colorTable[3][3] = {{0.1, 0.2, 0.3}, {0.2, 0.3, 0.1}, {0.3, 0.1, 0.2}}; 
int currentColor = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    	currentColor++;
    	if (currentColor > 2)
    		currentColor = 0;
    } if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)  {
    	currentColor--;
    	if (currentColor < 0)
    		currentColor = 2;
    }

}

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

		glfwSetKeyCallback(window, key_callback);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);
		glBegin(GL_TRIANGLES);
		glColor3f(colorTable[currentColor][0], colorTable[currentColor][1], colorTable[currentColor][2]);
		glVertex3f(0, 0, 0);
		//glColor3f(0.4, 0.5, 0.6);
		glVertex3f(1, 0, 0);
		//glColor3f(0.7, 0.8, 0.9);
		glVertex3f(0, 1, 0);
		glEnd();

		if (ImGui::Button("Red")){
			glClearColor(1.,0.,0.,1.);
		}
		if (ImGui::Button("Green")){
			glClearColor(0.,1.,0.,1.);
		}
		if (ImGui::Button("Blue")){
			glClearColor(0.,0.,1.,1.);
		}

		{
			ImGui::Text("Hello, world!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Mouse position: %.3f x %.3f y", ImGui::GetMousePos().x, ImGui::GetMousePos().y);
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
