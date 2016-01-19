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
	static float nextNormal(float a = 0.0, float b = 1.0) { return std::normal_distribution<float>(a, b)((_shared->_generator)); }
	static int nextInt(int a = 0, int b = 100) { return std::uniform_real_distribution<int>(a, b)((_shared->_generator)); }

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

void Block::createWindowTexture()
{
	// std::default_random_engine generator;
	// std::uniform_real_distribution<float> distribution{0.0, 1.0};

	// Evenly distribute as many as possible through each side
	const int pixelsPerFloat{100};
	const Size2<int> windowsPerFloat{10, 8};
	const Size2<int> windowSize{3, 2};

	_texture.setSize(Size2<int>{
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

						size_t index{_texture.index(pixel.x, pixel.y)};
						_texture.pixels[3*index] = color;
						_texture.pixels[3*index + 1] = color;
						_texture.pixels[3*index + 2] = color;
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

std::vector<Block*> Block::roof(glm::vec3 position, Size3<float> size)
{
	std::vector<Block*> blocks;
	glm::vec3 color{0.1, 0.1, 0.1};

	Block& b = *(new Block(position, {size.width, Rand::next(0.05, 0.25), size.depth}));
	b.createSingleColorTexture(color);
	b.createSingleColorTextureCoords();
	blocks.push_back(&b);
	
	Block* top = &b;
	for (int i = 0; i < Rand::nextInt(0, 2); i++)
	{
		float c = Rand::next(0.05, 0.15);
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

Block& Block::office(glm::vec3 position, Size3<float> size)
{
	Block& b = *(new Block(position, size));
	b.createWindowTextureCoords();
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
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(programID, "windowTexture"), 0); // combines with GL_TEXTURE0 in draw
}

void Block::draw()
{
	glBindVertexArray(_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glDrawElements(GL_TRIANGLES, _elements.size(), GL_UNSIGNED_INT, 0);
}

Building::Building(glm::vec3 position, Size3<float> size)
: Drawable(position, size)
{
}

Building& Building::generate(glm::vec3 position, Size3<float> size)
{
	Building& b = *(new Building(position, size));
	Size3<float> basesize, ofsize;
	basesize = {size.width, 0.1, size.depth};
	glm::vec3 baseOffset(0.05, 0.1, 0.05);
	ofsize = {size.width - 2*baseOffset.x, 3.0, size.depth - 2*baseOffset.z};
	
	b.append(Block::base(glm::vec3(0.0, 0.0, 0.0), basesize));
	b.append(Block::office(baseOffset, ofsize));
	for (auto& block : Block::roof(baseOffset + glm::vec3(0.0, ofsize.height, 0.0), ofsize))
		b.append(*block);
	return b;
}

void Building::append(Block& block)
{
	// box containing all blocks
	// at the moment the box is kind fucked up
	_blocks.push_back(&block);
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
		block->draw();
	}
}

Building::~Building()
{
	for (auto& block : _blocks)
		delete block;
}