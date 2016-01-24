#ifndef _TEXTURE_
#define _TEXTURE_

#include <GL/glew.h>
#include <vector>
#include "geometryUtils.hpp"

class Texture
{
public:
	Texture(Size2<int> s) { setSize(s); }
	Texture() : _size{0, 0} {}
	Texture(const Texture&) = default;
	~Texture() = default;

	std::vector<GLfloat> pixels;
	// index from upper left corner
	size_t index(size_t x, size_t y) { return _size.width*y + x; }

	Size2<int> size() { return _size; }
	// Resizes. Does not care about preserving the texture
	void setSize(Size2<int> size);

	// Preserves the texture and resizes
	void crop(Size2<int> size);
private:
	Size2<int> _size;
};


#endif