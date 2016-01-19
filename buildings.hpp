#ifndef _BUILDINGS_HPP_
#define _BUILDINGS_HPP_

#include "geometryUtils.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "texture.hpp"
#include "drawable.hpp"

class Block : public Drawable
{
public:
	friend class Building;

	Block(glm::vec3 position, Size3<float> size);
	~Block() = default;

	void upload(GLuint programID);
	void draw();

	static std::vector<Block*> roof(glm::vec3 position, Size3<float> size);
	static Block& office(glm::vec3 position, Size3<float> size);
	static Block& base(glm::vec3 position, Size3<float> size);

	void metricsOnTop(float padding, glm::vec3&, Size3<float>&);

private:
	std::vector<GLfloat> createTextureCoords(float h, float hh, float w, float ww, float www);

	void createWindowTexture();
	void createWindowTextureCoords();

	void createSingleColorTexture(glm::vec3 color = glm::vec3{0.0, 0.0, 0.0});
	void createSingleColorTextureCoords();

	void createElements();
	void createVertices();
};

class Building : public Drawable
{
public:
	Building(glm::vec3 position, Size3<float> size);
	~Building();

	void upload(GLuint programID);
	void draw();

	void append(Block&);

	// We only care about width and depth here
	static Building& generate(glm::vec3 position, Size3<float> size);
private:
	std::vector<Block*> _blocks;
};

#endif