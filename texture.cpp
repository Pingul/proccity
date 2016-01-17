#include "texture.hpp"
#include <algorithm>
#include <iomanip>

void Texture::setSize(Size2<int> size)
{	
	pixels.resize(3*size.width*size.height);
	_size = size;
}

void Texture::crop(Size2<int> size)
{
	std::vector<GLfloat> old = pixels;
	std::vector<GLfloat> newPixels(3*size.width*size.height, 0.0);
	Size2<int> oldSize = _size;
	_size = size;

	for (size_t i = 0; i < std::min(oldSize.height, size.height); i++)
	{
		for (size_t j = 0; j < std::min(oldSize.width, size.width); j++)
		{
			size_t oldIndex{oldSize.width*i + j};
			size_t newIndex{size.width*i + j};
			newPixels[3*newIndex] = pixels[3*oldIndex];
			newPixels[3*newIndex + 1] = pixels[3*oldIndex + 1];
			newPixels[3*newIndex + 2] = pixels[3*oldIndex + 2];
		}
	}
	pixels = newPixels;
}
