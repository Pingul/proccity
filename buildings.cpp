#include <GL/glew.h>
#include "buildings.hpp"
#include "glUtilities.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <exception>
#include <iostream>
#include <iomanip>
#include <random>

// G for global
Size2<int> G_windowSize{5, 5};
Size2<int> G_windowDist{2, 4};
glm::ivec2 G_nbrWindows{500, 60};
glm::vec2 G_windowsPerFloat{12.0, 9.0};

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

void Block::createWindowTextureCoords()
{
	// This is a very messy algorithm
	_singleColor = false;

	Size2<float> totalTextureSize{
		(float)(G_windowDist.width + G_nbrWindows.x*(G_windowSize.width + G_windowDist.width)),
		(float)(G_windowDist.height + G_nbrWindows.y*(G_windowSize.height + G_windowDist.height))
	};

	Size3<float> windowsPerDim{
		(float)round(_size.width*G_windowsPerFloat.x),
		(float)round(_size.height*G_windowsPerFloat.y),
		(float)round(_size.depth*G_windowsPerFloat.x),
	};

	float hStartAt = Rand::nextInt(0, G_nbrWindows.y);
	float wStartAt = Rand::nextInt(0, G_nbrWindows.x);

	float h = ((float)G_windowSize.height/2.f + (hStartAt*(float)(G_windowSize.height + G_windowDist.height)))/totalTextureSize.height;
	float hh = h + (windowsPerDim.height*(float)(G_windowSize.height + G_windowDist.height))/totalTextureSize.height;
	float w0 = ((float)G_windowSize.width/2.f + (wStartAt*(float)(G_windowSize.width + G_windowDist.width)))/totalTextureSize.width;
	float w = w0 + (windowsPerDim.width*(float)(G_windowSize.width + G_windowDist.width))/totalTextureSize.width;
	float ww = w + (windowsPerDim.depth*(float)(G_windowSize.width + G_windowDist.width))/totalTextureSize.width;
	float www = ww + w - w0;
	float wwww = 2*ww - w0;

	_texCoords = 
	{
		0.0, 0.0,  0.0, 0.0,  0.0, 0.0,  0.0, 0.0,
		1.0, 1.0,  1.0, 1.0,  1.0, 1.0,  1.0, 1.0,
		w0, h,  w, h,  w, hh,  w0, hh, 
		w, h,  ww, h,  ww, hh,  w, hh,
		ww, h,  www, h,  www, hh,  ww, hh,
		www, h,  wwww, h,  wwww, hh,  www, hh,
	};
}

Block::Block(glm::vec3 position, Size3<float> size)
: Drawable(position, size)
{
	createVertices();
	createElements();
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

std::vector<Block*> Block::tiledRoof(glm::vec3 position, Size3<float> size)
{
	std::vector<Block*> blocks;
	glm::vec3 color{0.05, 0.05, 0.05};

	Block& b = *(new Block(position, {size.width, Rand::next(0.05, 0.25), size.depth}));
	b.setColor(color);
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
		top->setColor(color);
		blocks.push_back(top);
	}
	return blocks;
}

Block& Block::roof(glm::vec3 position, Size3<float> size)
{
	size.height = 0.05;
	Block& b = *(new Block(position, size));
	b.setColor({0.05, 0.05, 0.05});
	return b;
}

Block& Block::windowed(glm::vec3 position, Size3<float> size)
{
	Block& b = *(new Block(position, size));
	b.createWindowTextureCoords();
	return b;
}

Block& Block::base(glm::vec3 position, Size3<float> size)
{
	size.height = 0.1;
	Block& b = *(new Block(position, size));
	return b;
}


void Block::upload(GLuint programID)
{
	Drawable::upload(programID);

	glUniform1i(glGetUniformLocation(programID, "windowTexture"), 0); // combines with GL_TEXTURE0 in draw
	logError("after block");
}


void Block::draw()
{
	glBindVertexArray(_VAO);
	// probably more efficient to upload these together with the object, instead of sending loads every draw call
	if (_singleColor)
	{
		glUniform3f(glGetUniformLocation(_program, "uniformColor"), _color.x, _color.y, _color.z);
		glUniform1i(glGetUniformLocation(_program, "showColor"), true);
	}
	else
		glUniform1i(glGetUniformLocation(_program, "showColor"), false);
	glDrawElements(GL_TRIANGLES, _elements.size(), GL_UNSIGNED_INT, 0);
}

Building::Building(glm::vec3 position, Size3<float> size)
: Drawable(position, size)
{
}

Building& Building::generate(glm::vec3 position, Size3<float> size)
{
	Building& building = *(new Building(position, size));
	Size3<float> basesize{size}, s{0.0, 0.0, 0.0};
	basesize.height = 0.1;
	glm::vec3 v;

	Block& base = Block::base(glm::vec3(0.0, 0.0, 0.0), basesize);
	// building.append(base);

	base.metricsOnTop(0.1, v, s);
	s.height = size.height;
	Block& office = Block::windowed(v, s);
	building.append(office);

	// office.metricsOnTop(0.0, v, s);
	// building.append(Block::tiledRoof(v, s));

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
	building.append(Block::tiledRoof(roof_v, roof_s));

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
		building.append(Block::tiledRoof(roof_v, roof_s));
	}
	return building;

}

Building& Building::stacked(glm::vec3 position, Size3<float> size)
{
	Building& building = *(new Building(position, size));
	Size3<float> basesize{size};

	Block& base = Block::base(glm::vec3(0.0, 0.0, 0.0), basesize);
	building.append(base);

	Block& top = base;
	glm::vec3 v;
	Size3<float> s;

	int nbrStacks = 3;
	for (int i = 0; i < nbrStacks; i++)
	{
		top.metricsOnTop(0.1, v, s);
		s.height = 1.5;
		Block& stack = Block::windowed(v, s);
		building.append(stack);

		stack.metricsOnTop(0.0, v, s);
		Block& roof = Block::roof(v, s);
		building.append(roof);
		top = roof;
	}

	top.metricsOnTop(0.0, v, s);
	building.append(Block::tiledRoof(v, s));

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
		block->draw();
	}
}

Building::~Building()
{
	for (auto& block : _blocks)
		delete block;
}


TownGrid::TownGrid()
: _size{1, 1}
{
	for (int i = 0; i < _size.width; i++)
		for (int j = 0; j < _size.height; j++)
		{
			Size3<float> townBlock{4.0, Rand::next(2.0, 6.0), 2.0};
			float x = i*(townBlock.width + 0.5); // potential road included
			float z = j*(townBlock.depth + 0.5);
			glm::vec3 v{x, 0.0, z};
			_buildings.push_back(&Building::stacked(v, townBlock));
			// _buildings.push_back(&Building::generate(v, townBlock));
		}

	createWindowTexture();
	logError("town complete");
}

void TownGrid::upload(GLuint programID)
{
	glGenTextures(1, &_textureHandle);
	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texture.size().width, _texture.size().height, 0, GL_RGB, GL_FLOAT, _texture.pixels.data());
	
	for (auto& b : _buildings)
		b->upload(programID);

	logError("town uploaded");
}

void TownGrid::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureHandle);

	for (auto& b : _buildings)
		b->draw();
}

void TownGrid::createWindowTexture()
{
	_texture.setSize(Size2<int>{
		(int)(G_windowDist.width + G_nbrWindows.x*(G_windowSize.width + G_windowDist.width)),
		(int)(G_windowDist.height + G_nbrWindows.y*(G_windowSize.height + G_windowDist.height))
	});

	bool lastLit{false};

	GLfloat color;
	for (int y = 0; y < G_nbrWindows.y; y++)
	{
		for (int x = 0; x < G_nbrWindows.x; x++)
		{
			glm::ivec2 coord{
				G_windowDist.width + x*(G_windowSize.width + G_windowDist.width),
				G_windowDist.height + y*(G_windowSize.height + G_windowDist.height),
			};

			float v{Rand::next()};
			if (lastLit)
				lastLit = v < 0.7 ? true : false;
			else
				lastLit = v < 0.9 ? false : true;
			color = lastLit ? Rand::next(0.5, 1.0) : 0.0;

			for (int i = 0; i < G_windowSize.width; i++)
			{
				for (int j = 0; j < G_windowSize.height; j++)
				{
					glm::ivec2 pixel = {
						coord.x + i,
						coord.y + j,
					};

					size_t index{_texture.index(pixel.x, pixel.y)};
					_texture.pixels[3*index] = color*0.99;
					_texture.pixels[3*index + 1] = color*0.98;
					_texture.pixels[3*index + 2] = color*0.85;
				}
			}
		}
	}
}