#include <GL/glew.h>
#include "buildings.hpp"
#include "glUtilities.hpp"

#include <exception>
#include <iostream>
#include <iomanip>
#include <random>

class Rand
{
public:
	Rand() = default;

	static float next(float a = 0.0, float b = 1.0) { return std::uniform_real_distribution<float>(a, b)((_shared->_generator)); }
	static float nextNormal(float a = 0.0, float b = 1.0) { return std::exponential_distribution<double>(3.5)((_shared->_generator)); }
	static int nextInt(int a = 0, int b = 100) { return std::uniform_int_distribution<int>(a, b)((_shared->_generator)); }

private:
	std::default_random_engine _generator;
	static Rand* _shared;
};
Rand* Rand::_shared = new Rand();

void Block::createVertices()
{
	_vertices = 
	{
		0.0, 0.0, 0.0,  _size.width, 0.0, 0.0,  _size.width, 0.0, _size.depth,  0.0, 0.0, _size.depth, // bottom
		0.0, _size.height, 0.0,  _size.width, _size.height, 0.0,  _size.width, _size.height, _size.depth,  0.0, _size.height, _size.depth, // top
		0.0, 0.0, 0.0,  _size.width, 0.0, 0.0,  _size.width, _size.height, 0.0,  0.0, _size.height, 0.0, // front
		_size.width, 0.0, 0.0,  _size.width, 0.0, _size.depth,  _size.width, _size.height,  _size.depth, _size.width, _size.height, 0.0, // right
		0.0, 0.0, _size.depth,  _size.width, 0.0, _size.depth,  _size.width, _size.height, _size.depth,  0.0, _size.height, _size.depth, // back
		0.0, 0.0, 0.0,  0.0, 0.0, _size.depth,  0.0, _size.height,  _size.depth, 0.0, _size.height, 0.0, // left
	};
}

void Block::createElements()
{
	_elements =
	{
		0, 2, 1, 0, 3, 2,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23,
	};
}

/*
	Orientation
	1	 --
		|  |
	hh	 -- -- -- --
		|  |  |  |  |
	h	 -- -- -- --
		|  |
	0	 --
		0  w  ww www 1
*/

std::vector<GLfloat> Block::createTextureCoords(float h, float hh, float w, float ww, float www)
{
	std::vector<GLfloat> coords = 
	{
		0.0, h,  w, h,  w, 0.0,  0.0, 0.0,
		0.0, hh,  w, hh,  w, 1.0,  0.0, 1.0,
		0.0, h,  w, h,  w, hh,  0.0, hh, 
		w, h,  ww, h,  ww, hh,  w, hh,
		ww, h,  www, h,  www, hh,  ww, hh,
		www, h,  1.0, h,  1.0, hh,  www, hh,
	};
	return coords;
}

void Block::createWindowTextureCoords()
{
	float h{1.f/100.f};
	float hh{99.f/100.f};
	float w{_size.width/(2*_size.width + 2*_size.depth)};
	float ww{(_size.width + _size.depth)/(2*_size.width + 2*_size.depth)};
	float www{(2*_size.width + _size.depth)/(2*_size.width + 2*_size.depth)};
	_texCoords = createTextureCoords(h, hh, w, ww, www);
}

void Block::createWindowTexture(bool FULL_RES)
{
	// std::default_random_engine generator;
	// std::uniform_real_distribution<float> distribution{0.0, 1.0};

	// Evenly distribute as many as possible through each side
	int pixelsPerFloat{100};
	Size2<int> windowsPerFloat{10, 8};
	Size2<int> windowSize{3, 2};
	Texture* texture{&_texture};

	if (!FULL_RES)
	{
		pixelsPerFloat = 50;
		windowsPerFloat = {5, 4};
		windowSize = {3, 2};
		texture = &_texture_LOWRES;
		_textureLowResInited = true;
		// std::cout << "lowres" << std::endl;
	}

	texture->setSize(Size2<int>{
		(int)(pixelsPerFloat*(_size.width*2 + _size.depth*2)), 
		(int)(pixelsPerFloat*_size.height + 2)
	});

	int sideOffset = 0;
	for (int side = 1; side < 5; side++)
	{
		float width = side % 2 == 1 ? _size.width : _size.depth;
		glm::vec2 nbrWindows = {
			floor(width*windowsPerFloat.width),
			floor(_size.height*windowsPerFloat.height)
		};

		glm::vec2 cubeSide{width*pixelsPerFloat, _size.height*pixelsPerFloat};
		glm::vec2 windowOffsets{cubeSide.x/nbrWindows.x, cubeSide.y/nbrWindows.y};

		for (int x = 1; x < nbrWindows.x; x++)
		{
			for (int y = 1; y < nbrWindows.y; y++)
			{
				GLfloat color{Rand::nextNormal()};
				for (int i = -windowSize.width; i < windowSize.width; i++)
				{
					for (int j = -windowSize.height; j < windowSize.height; j++)
					{
						glm::ivec2 pixel = {
							(int)round(windowOffsets.x*x) + i + sideOffset,
							(int)round(windowOffsets.y*y) + j,
						};

						size_t index{texture->index(pixel.x, pixel.y)};
						texture->pixels[3*index] = color;
						texture->pixels[3*index + 1] = color;
						texture->pixels[3*index + 2] = color;
					}
				}
			}
		}
		sideOffset += cubeSide.x;
	}
}

void Block::createSingleColorTexture(glm::vec3 color)
{
	Size2<int> size{4, 3};
	_texture.setSize(size);
	for (int j = 0; j < size.height; j++)
		for (int i = 0; i < size.width; i++)
		{
			size_t index = _texture.index(i, j);
			_texture.pixels[3*index] = color.x;
			_texture.pixels[3*index + 1] = color.y;
			_texture.pixels[3*index + 2] = color.z;
		}
}

void Block::createSingleColorTextureCoords()
{
	float h{1.0/3.0};
	float hh{2.0/30};
	float w{1.0/4.0};
	float ww{2.0/4.0};
	float www{3.0/4.0};
	_texCoords = createTextureCoords(h, hh, w, ww, www);
}


Block::Block(glm::vec3 position, Size3<float> size)
: Drawable(position, size)
{
	createVertices();
	createElements();
	// createWindowTextureCoords();
	// createWindowTexture();
}

void Block::metricsOnTop(float padding, glm::vec3& at, Size3<float>& size)
{
	size.width = _size.width - 2*padding;
	size.depth = _size.depth - 2*padding;
	if (size.width < 0.0) 
		size.width = 0.0;
	if (size.depth < 0.0) 
		size.depth = 0.0;
	at = _position + glm::vec3(padding, _size.height, padding);
}

void Block::randomPosOnTop(glm::vec3& v, const Size3<float>& otherSize)
{
	float x = Rand::next(0.0, _size.width - otherSize.width);
	float z = Rand::next(0.0, _size.depth - otherSize.depth);
	v.x = _position.x + x;
	v.y = _position.y;
	v.z = _position.z + z;
}

std::vector<Block*> Block::roof(glm::vec3 position, Size3<float> size)
{
	std::vector<Block*> blocks;
	glm::vec3 color{0.05, 0.05, 0.05};

	Block& b = *(new Block(position, {size.width, Rand::next(0.05, 0.25), size.depth}));
	b.createSingleColorTexture(color);
	b.createSingleColorTextureCoords();
	blocks.push_back(&b);
	
	int r = Rand::nextInt(0, 2);
	Block* top = &b;
	for (int i = 0; i < r; i++)
	{
		float c = Rand::next(0.01, 0.1);
		color = {c, c, c};
		glm::vec3 pos;
		Size3<float> s;
		top->metricsOnTop(Rand::next(0.01, 0.1), pos, s);
		s.height = Rand::next(0.05, top->dim().height);
		top = new Block(pos, s);
		top->createSingleColorTexture(color);
		top->createSingleColorTextureCoords();
		blocks.push_back(top);
	}


	return blocks;
}

Block& Block::windowed(glm::vec3 position, Size3<float> size)
{
	Block& b = *(new Block(position, size));
	b.createWindowTextureCoords();
	b.createWindowTexture(false);
	b.createWindowTexture();
	return b;
}

Block& Block::base(glm::vec3 position, Size3<float> size)
{
	Block& b = *(new Block(position, size));
	b.createSingleColorTexture();
	b.createSingleColorTextureCoords();
	return b;
}


void Block::upload(GLuint programID)
{
	Drawable::upload(programID);	

	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glGenerateMipmap(GL_TEXTURE_2D); // This makes everything significantly slower

	if (_textureLowResInited)
	{
		glGenTextures(1, &_textureHandle_LOWRES);
		glBindTexture(GL_TEXTURE_2D, _textureHandle_LOWRES);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texture_LOWRES.size().width, _texture_LOWRES.size().height, 0, GL_RGB, GL_FLOAT, _texture_LOWRES.pixels.data());
	}

	_texture.setSize({0, 0});
	_texture_LOWRES.setSize({0, 0});

	glUniform1i(glGetUniformLocation(programID, "windowTexture"), 0); // combines with GL_TEXTURE0 in draw
}

void Block::draw()
{
	draw(true);
}

void Block::draw(bool FULL_RES)
{

	glBindVertexArray(_VAO);
	glActiveTexture(GL_TEXTURE0);
	if (_drawFullRes)
		glBindTexture(GL_TEXTURE_2D, _textureHandle);
	else
		glBindTexture(GL_TEXTURE_2D, _textureHandle_LOWRES);
	glDrawElements(GL_TRIANGLES, _elements.size(), GL_UNSIGNED_INT, 0);
}

Building::Building(glm::vec3 position, Size3<float> size)
: Drawable(position, size)
{
}

void Building::drawMode(bool fullRes)
{
	for (auto& b : _blocks)
		b->drawMode(fullRes);
}

Building& Building::generate(glm::vec3 position, Size3<float> size)
{
	Building& building = *(new Building(position, size));
	Size3<float> basesize{size}, s{0.0, 0.0, 0.0};
	basesize.height = 0.1;
	glm::vec3 v;

	Block& base = Block::base(glm::vec3(0.0, 0.0, 0.0), basesize);
	building.append(base);

	base.metricsOnTop(0.1, v, s);
	s.height = size.height;
	Block& office = Block::windowed(v, s);
	building.append(office);

	office.metricsOnTop(0.0, v, s);
	building.append(Block::roof(v, s));

	return building;
}

Building& Building::apartments(glm::vec3 position, Size3<float> size)
{
	Building& building = *(new Building(position, size));
	Size3<float> basesize{size};
	basesize.height = 0.1;

	Block& base = Block::base(glm::vec3(0.0, 0.0, 0.0), basesize);
	building.append(base);


	glm::vec3 v;
	Size3<float> s{size};
	s.width *= Rand::next(0.4, 0.6);
	s.height *= Rand::next(0.8, 1.0);
	s.depth *= Rand::next(0.4, 0.6);
	base.randomPosOnTop(v, s);
	Block& ap = Block::windowed(v, s);
	building.append(ap);

	glm::vec3 roof_v;
	Size3<float> roof_s{0.0, 0.0, 0.0};
	ap.metricsOnTop(0.0, roof_v, roof_s);
	building.append(Block::roof(roof_v, roof_s));


	int nbrApartments = 2;//{Rand::nextInt(1, 4)};
	for (int i = 0; i < nbrApartments; i++)
	{
		s.width *= Rand::next(0.7, 1.0);
		s.height *= Rand::next(0.6, 0.8);
		s.depth *= Rand::next(0.7, 1.0);
		base.randomPosOnTop(v, s);
		Block& apartm = Block::windowed(v, s);
		building.append(apartm);

		apartm.metricsOnTop(0.0, roof_v, roof_s);
		building.append(Block::roof(roof_v, roof_s));
	}

	return building;

}

void Building::append(Block& block)
{
	// box containing all blocks
	// at the moment the box is kind of fucked up
	_blocks.push_back(&block);
}

void Building::append(std::vector<Block*> blocks)
{
	for (auto& block : blocks)
		append(*block);
}

void Building::upload(GLuint programID)
{
	_program = programID;
	for (auto& block : _blocks)
	{
		block->upload(programID);
	}
}
void Building::draw()
{
	for (auto& block : _blocks)
	{
		glm::mat4 m = transformation()*block->transformation();
		uploadMat4(_program, m, "MTW");
		block->draw(false);
	}
}

Building::~Building()
{
	for (auto& block : _blocks)
		delete block;
}


TownGrid::TownGrid()
: _size{8, 8}
{
	for (int i = 0; i < _size.width; i++)
		for (int j = 0; j < _size.height; j++)
		{
			Size3<float> townBlock{2.0, 5.0, 2.0};
			float x = i*(townBlock.width + 0.5); // potential road included
			float z = j*(townBlock.depth + 0.5);
			glm::vec3 v{x, 0.0, z};
			_buildings.push_back(&Building::apartments(v, townBlock));
		}
}

void TownGrid::upload(GLuint programID)
{
	for (auto& b : _buildings)
		b->upload(programID);
}

void TownGrid::draw()
{
	for (auto& b : _buildings)
		b->draw();
}

void TownGrid::updateResolution(Camera& cam)
{
	for (auto& building : _buildings)
	{
		double dist = glm::length(cam.at() - building->at());
		// std::cout << std::dec << dist << std::endl;
		building->drawMode(dist < 24.0);
	}
}