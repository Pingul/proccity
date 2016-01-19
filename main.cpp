#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "glUtilities.hpp"
#include "geometryUtils.hpp"
#include "camera.hpp"
#include "buildings.hpp"
#include "glText.hpp"

GLFWwindow* window;
GLuint program;
GLuint textProgram;
Camera* camera;

Building* b;
Textbox* FPSTextbox;
Textbox* FPSTextboxCount;

void initGeometry()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	logError("before shaders");
	program = buildProgram("shaders/shader.vert", "shaders/shader.frag");
	logError("after shaders");

	Size3<float> s{1.0, 3.0, 5.0};
	b = &Building::generate(glm::vec3(0.0, 0.0, 0.0), s);
	b->upload(program);

	// Camera
	camera = new Camera(glm::vec3(1.0, 1.0, -5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 unity = glm::mat4();
	glm::mat4 projectionMatrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
	uploadMat4(program, glm::value_ptr(unity), "MTW");
	uploadMat4(program, glm::value_ptr(camera->WTVMatrix()), "WTV");
	uploadMat4(program, glm::value_ptr(projectionMatrix), "proj");

	logError("init build program");

	textProgram = buildProgram("shaders/text.vert", "shaders/text.frag");
	FPSTextbox = new Textbox(glm::vec3(-0.95, -0.95, 0.0), Size2<float>{0.2, 0.05});
	FPSTextbox->setText("FPS");
	FPSTextbox->upload(textProgram);
	FPSTextboxCount = new Textbox(glm::vec3(-0.75, -0.95, 0.0), Size2<float>{0.1, 0.05});
	FPSTextboxCount->setText("0");
	FPSTextboxCount->upload(textProgram);
	logError("init text program");
}

void draw(float t)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glEnable(GL_DEPTH_TEST);
	glUseProgram(program);
	camera->pollWindowForCameraMovement(window);
	uploadMat4(program, glm::value_ptr(camera->WTVMatrix()), "WTV");
	b->draw();

	glDisable(GL_DEPTH_TEST);
	glUseProgram(textProgram);
	FPSTextbox->draw();

	static float fps = 0.0;
	static float lastTime = 0.0;
	static int it = 0;
	fps += 1.0/(t - lastTime);
	lastTime = t;
	if (++it == 10)
	{
		fps /= (float)it;
		FPSTextboxCount->setText(std::to_string((int)round(fps)));
		it = 0;
		fps = 0.0;
	}
	FPSTextboxCount->draw();

	if (logError("display"))
	{
		std::cout << "error in display, aborting" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwSwapBuffers(window);
}


void show()
{
	while(!glfwWindowShouldClose(window))
	{
		double t{glfwGetTime()};
		glfwPollEvents();
		draw(t);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			static double lastT = 0.0;
			if (t - lastT > 0.2)
			{
				glUseProgram(program);
				delete b;
				Size3<float> s{1.0, 3.0, 5.0};
				b = &Building::generate(glm::vec3(0.0, 0.0, 0.0), s);
				b->upload(program);
				lastT = t;
			}
		}
	}
}

int main()
{
	initWindow(&window);
	initGeometry();
	show();

	return 0;
}