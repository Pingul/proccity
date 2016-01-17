#include <GL/glew.h>
#include "buildings.hpp"
#include "glUtilities.hpp"

#include <exception>
#include <iostream>
#include <iomanip>
#include <random>

class Cube
{
public:
	Cube(Size3<float> s);
	Size3<float> size;

	std::vector<GLfloat> vertices;
	// std::vector<GLfloat> _texCoords;
	std::vector<GLfloat> elements;
};

Cube::Cube(Size3<float> s)
: size{s}
{
	if (size.width < 0.0 || size.height < 0.0 || size.depth < 0.0)
		return;

	vertices = 
	{
		0.0, 0.0, 0.0,  size.width, 0.0, 0.0,  size.width, 0.0, size.depth,  0.0, 0.0, size.depth, // bottom
		0.0, size.height, 0.0,  size.width, size.height, 0.0,  size.width, size.height, size.depth,  0.0, size.height, size.depth, // top
		0.0, 0.0, 0.0,  size.width, 0.0, 0.0,  size.width, size.height, 0.0,  0.0, size.height, 0.0, // front
		size.width, 0.0, 0.0,  size.width, 0.0, size.depth,  size.width, size.height,  size.depth, size.width, size.height, 0.0, // right
		0.0, 0.0, size.depth,  size.width, 0.0, size.depth,  size.width, size.height, size.depth,  0.0, size.height, size.depth, // back
		0.0, 0.0, 0.0,  0.0, 0.0, size.depth,  0.0, size.height,  size.depth, 0.0, size.height, 0.0, // left
	};

	elements =
	{
		0, 2, 1, 0, 3, 2,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23,
	};
}

void Building::append(const Cube& cube, glm::vec3 at)
{
	int nbrVertices{(int)_vertices.size()};
	for (int i = 0; i < cube.vertices.size(); i += 3)
	{
		_vertices.push_back(cube.vertices[i]);
		_vertices.push_back(cube.vertices[i + 1]);
		_vertices.push_back(cube.vertices[i + 2]);
	}
	for (auto& e : cube.elements)
	{
		_elements.push_back(nbrVertices + e);
	}
}

std::vector<GLfloat> Building::createTextureCoords()
{
	float h{1.f/100.f};
	float hh{99.f/100.f};
	float w{_size.width/(2*_size.width + 2*_size.depth)};
	float ww{(_size.width + _size.depth)/(2*_size.width + 2*_size.depth)};
	float www{(2*_size.width + _size.depth)/(2*_size.width + 2*_size.depth)};

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

Texture Building::createWindowTexture()
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution{0.0, 1.0};

	// Evenly distribute as many as possible through each side
	const int pixelsPerFloat{100};
	const Size2<int> windowsPerFloat{10, 8};
	const Size2<int> windowSize{3, 2};

	Texture tex{Size2<int>{
		(int)(pixelsPerFloat*(_size.width*2 + _size.depth*2)), 
		(int)(pixelsPerFloat*_size.height + 2)
	}};

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
				GLfloat color{distribution(generator)};
				for (int i = -windowSize.width; i < windowSize.width; i++)
				{
					for (int j = -windowSize.height; j < windowSize.height; j++)
					{
						glm::ivec2 pixel = {
							(int)round(windowOffsets.x*x) + i + sideOffset,
							(int)round(windowOffsets.y*y) + j,
						};

						size_t index{tex.index(pixel.x, pixel.y)};
						tex.pixels[3*index] = color;
						tex.pixels[3*index + 1] = color;
						tex.pixels[3*index + 2] = color;
					}
				}
			}
		}
		sideOffset += cubeSide.x;
	}
	return tex;
}

Building::Building(glm::vec3 position, Size3<float> size)
: Drawable(position, size)
{
	// _vertices = createVertices(_size.width, _size.height, _size.depth);	
	// _elements = createIndices();
	Cube c{size};
	append(c, glm::vec3{0.0, 0.0, 0.0});
	Cube c2{Size3<float>{1.0, 1.0, 1.0}};
	append(c2, glm::vec3{1.0, 0.0, 0.0});
	_texCoords = createTextureCoords();
	_texture = createWindowTexture();
}

void Building::upload(GLuint programID)
{
	Drawable::upload(programID);
	
	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(_program, "windowTexture"), 0); // combines with GL_TEXTURE0 in draw
}

void Building::draw()
{
	uploadPerspective();
	glBindVertexArray(_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glDrawElements(GL_TRIANGLES, _elements.size(), GL_UNSIGNED_INT, 0);
}