#ifndef _GL_TEXT_
#define _GL_TEXT_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "geometryUtils.hpp"
#include "texture.hpp"
#include "drawable.hpp"

class Textbox : public Drawable
{
public:
	Textbox(glm::vec3 position, Size2<float> size);

	void upload(GLuint programID);
	void draw();
	void setText(std::string text);

private:
	std::string _content;

	bool _loaded{false};
	void render();
	void uploadTexture();

};


#endif