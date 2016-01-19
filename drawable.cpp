#include "drawable.hpp"
#include <iostream>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glUtilities.hpp"


glm::mat4 Drawable::transformation()
{
	glm::mat4 translation = glm::translate(glm::mat4(), _position);
	return translation;
}

void Drawable::upload(GLuint program)
{
	_program = program;
	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);


	glGenBuffers(1, &_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size()*sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);
	GLint posAttrib = glGetAttribLocation(program, "in_Position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(posAttrib);


	glGenBuffers(1, &_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elements.size()*sizeof(GLuint), _elements.data(), GL_STATIC_DRAW);

	if (_texCoords.size() > 0)
	{
		glGenBuffers(1, &_TBO);
		glBindBuffer(GL_ARRAY_BUFFER, _TBO);
		glBufferData(GL_ARRAY_BUFFER, _texCoords.size()*sizeof(GLfloat), _texCoords.data(), GL_STATIC_DRAW);
		GLint texAttrib = glGetAttribLocation(program, "texCoord");
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
		glEnableVertexAttribArray(texAttrib);
	}

	if (_texture.size().height > 0 || _texture.size().width > 0)
	{
		glGenTextures(1, &_textureHandle);
		glBindTexture(GL_TEXTURE_2D, _textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texture.size().width, _texture.size().height, 0, GL_RGB, GL_FLOAT, _texture.pixels.data());
	}
}

 Drawable::~Drawable()
{
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_TBO);
	glDeleteBuffers(1, &_EBO);
	glDeleteTextures(1, &_textureHandle);
}