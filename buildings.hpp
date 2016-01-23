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
	void draw(bool FULL_RES);

	void drawMode(bool fullRes) { _drawFullRes = fullRes || !_textureLowResInited; }

	static std::vector<Block*> roof(glm::vec3 position, Size3<float> size);
	
	static Block& windowed(glm::vec3 position, Size3<float> size);
	static Block& base(glm::vec3 position, Size3<float> size);

	void metricsOnTop(float padding, glm::vec3&, Size3<float>&);
	void randomPosOnTop(glm::vec3&, const Size3<float>&);

private:
	std::vector<GLfloat> createTextureCoords(float h, float hh, float w, float ww, float www);

	bool _textureLowResInited{false};
	bool _drawFullRes{true};
	GLuint _textureHandle_LOWRES;
	Texture _texture_LOWRES;

	void createWindowTexture(bool FULL_RES = true);
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

	void drawMode(bool fullRes);

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

	void updateResolution(Camera&);

private:
	Size2<int> _size;
	std::vector<Building*> _buildings;
};

#endif