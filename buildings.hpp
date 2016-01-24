#ifndef _BUILDINGS_HPP_
#define _BUILDINGS_HPP_

#include "geometryUtils.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "texture.hpp"
#include "drawable.hpp"
#include "camera.hpp"

class Block : public Drawable
{
public:
	friend class Building;

	Block(glm::vec3 position, Size3<float> size);
	~Block() = default;

	void upload(GLuint programID);
	void draw();

	static std::vector<Block*> roof(glm::vec3 position, Size3<float> size);
	
	static Block& windowed(glm::vec3 position, Size3<float> size);
	static Block& base(glm::vec3 position, Size3<float> size);

	void metricsOnTop(float padding, glm::vec3&, Size3<float>&);
	void randomPosOnTop(glm::vec3&, const Size3<float>&);

private:
	bool _singleColor{true};
	glm::vec3 _color{0.0, 0.0, 0.0};

	void createWindowTextureCoords();

	void setColor(glm::vec3 c) { _color = c; }

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
	void append(std::vector<Block*>);

	// We only care about width and depth here
	static Building& generate(glm::vec3 position, Size3<float> size);
	static Building& apartments(glm::vec3 position, Size3<float> size);
private:
	std::vector<Block*> _blocks;
};

class TownGrid
{
public:
	TownGrid();

	void upload(GLuint programID);
	void draw();

private:
	Size2<int> _size;
	std::vector<Building*> _buildings;

	GLuint _textureHandle;
	Texture _texture;

	void createWindowTexture();
};

#endif