#define GL_DO_NOT_WANR_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#define GLEW_STATIC

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

#include "glUtilities.hpp"

bool logError(const char* str)
{
	GLenum error{glGetError()};
	if (error == GL_NO_ERROR)
		return false;
	std::cout << str << std::endl;
	do 
	{
		std::cout << "\t error " << std::hex << error;
		switch (error)
		{
			// Error codes taken from https://www.opengl.org/wiki/OpenGL_Error
			case 0x0500: std::cout << " : GL_INVALID_ENUM"; break;
			case 0x0501: std::cout << " : GL_INVALID_VALUE"; break;
			case 0x0502: std::cout << " : GL_INVALID_OPERATION"; break;
			case 0x0503: std::cout << " : GL_STACK_OVERFLOW"; break;
			case 0x0504: std::cout << " : GL_STACK_UNDERFLOW"; break;
			case 0x0505: std::cout << " : GL_OUT_OF_MEMORY"; break;
			case 0x0506: std::cout << " : GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			case 0x0507: std::cout << " : GL_CONTEXT_LOST"; break;
			case 0x8031: std::cout << " : GL_TABLE_TOO_LARGE"; break;
			default: std::cout << " : UNKNOWN" << std::endl;
		}
		std::cout << std::endl;
	} while ((error = glGetError()) != GL_NO_ERROR);
	return true;
}

void initWindow(GLFWwindow** window)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Procity", nullptr, nullptr);
	glfwMakeContextCurrent(*window);

	glewExperimental = GL_TRUE;
	glewInit();
	logError("unknown error from glewInit");
}

GLuint createShader(const char* source, GLenum shaderType)
{
	GLuint shader{glCreateShader(shaderType)};
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	GLint isCompiled{0};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint length{0};
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		std::vector<GLchar> errorLog(length);
		glGetShaderInfoLog(shader, length, &length, &errorLog.at(0));
		std::cout << "SHADER ";
		for (auto& c : errorLog)
		{
			std::cout << c;
		}

		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
	GLuint vShader{createShader(vertexSource, GL_VERTEX_SHADER)};
	GLuint fShader{createShader(fragmentSource, GL_FRAGMENT_SHADER)};
	GLuint program{glCreateProgram()};
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	glUseProgram(program);
	return program;
}

std::string fileContents(const char* file)
{
	std::string contents;
	std::fstream f{file};
	if (f.is_open())
	{
		std::string line;
		while (getline(f, line))
		{
			contents += line + "\n";
		}
	}
	return contents;
}

GLuint buildProgram(const char* vertexShaderFile, const char* fragmentShaderFile)
{
	return createProgram(fileContents(vertexShaderFile).c_str(), fileContents(fragmentShaderFile).c_str());
}



// experimental

void uploadMat4(GLuint progID, const float* mat, const char* loc)
{
	glUniformMatrix4fv(glGetUniformLocation(progID, loc), 1, GL_FALSE, mat);
}

#include <glm/gtc/type_ptr.hpp>
void uploadMat4(GLuint progID, glm::mat4& mat, const char* loc)
{
	uploadMat4(progID, glm::value_ptr(mat), loc);
}