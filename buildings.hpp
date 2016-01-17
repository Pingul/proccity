#ifndef _BUILDINGS_HPP_
#define _BUILDINGS_HPP_

#include "geometryUtils.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "texture.hpp"
#include "drawable.hpp"

class Cube;

class Building : public Drawable
{
public:
	Building(glm::vec3 position, Size3<float> size);
	~Building() = default;

	void upload(GLuint programID);
	void draw();

private:
	void append(const Cube&, glm::vec3 at);

	Texture createWindowTexture();
	std::vector<GLfloat> createTextureCoords();
};


#endif