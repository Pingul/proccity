#ifndef _DRAWABLE_
#define _DRAWABLE_

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "texture.hpp"

class Drawable
{
public:
	Drawable(Size3<float> size)
		: _position{0.0, 0.0, 0.0}, _size{size} {}
	Drawable(glm::vec3 position, Size3<float> size)
		: _position{position}, _size{size} {}
	virtual ~Drawable();

	virtual void draw() = 0;
	virtual void upload(GLuint program);

	virtual void move(glm::vec3 newPosition) { _position = newPosition; }
	virtual glm::vec3 at() { return _position; }
	virtual Size3<float> dim() { return _size; }

protected:
	virtual glm::mat4 transformation();

	glm::vec3 _position{0.0, 0.0, 0.0};
	Size3<float> _size;

	std::vector<GLfloat> _vertices;
	std::vector<GLuint> _elements;
	std::vector<GLfloat> _texCoords;

	Texture _texture;

	GLuint _VAO, _TBO, _VBO, _EBO;
	GLuint _program;
	GLuint _textureHandle;

};

#endif