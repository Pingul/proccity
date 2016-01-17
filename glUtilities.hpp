#ifndef	_GL_UTILITIES_
#define _GL_UTILITIES_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH = 800; 

bool logError(const char* str);
void initWindow(GLFWwindow** window);
GLuint createShader(const char* source, GLenum shaderType);
GLuint buildProgram(const char* vertexShaderFile, const char* fragmentShaderFile);

// experimental
void uploadMat4(GLuint progID, const float* mat, const char* loc);

#endif