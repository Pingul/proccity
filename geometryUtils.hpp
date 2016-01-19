#ifndef _GEOMETRY_UTILITIES
#define _GEOMETRY_UTILITIES

#include <glm/glm.hpp>
#include <iostream>

template <class T>
class Size2 
{
public:
	Size2() = default;
	Size2(const Size2<T>&) = default;
	Size2(T w, T h)
		: width{w}, height{h} {}

	T width;
	T height;

	Size2<T> operator+(Size2<T>& rhs) const { return Size2<T>{width + rhs.width, height + rhs.height}; };
};

template<class T>
inline std::ostream& operator<<(std::ostream& os, const Size2<T> s)
{
	os << "w: " << s.width << ", h: " << s.height;
	return os;
}

template <class T>
class Size3
{
public:
	Size3() = default;
	Size3(const Size3<T>&) = default;
	Size3(T w, T h, T d)
		: width{w}, height{h}, depth{d} {}

	T width;
	T height;
	T depth;

	Size3<T> mask(float x, float y, float z) const { return Size3<T>{width*x, height*y, depth*z}; }
};

template<class T>
inline glm::vec3 operator+(glm::vec3 v, Size3<T> s)
{
	return glm::vec3{v.x + (float)s.width, v.y + (float)s.height, v.z + (float)s.depth};
}

template<class T>
inline glm::vec3 operator+(Size3<T> s, glm::vec3 v)
{
	return v + s;
}

template<class T>
inline std::ostream& operator<<(std::ostream& os, const Size3<T> s)
{
	os << "w: " << s.width << ", h: " << s.height << ", d: " << s.depth;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3 vec)
{
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec2 vec)
{
	os << "{" << vec.x << ", " << vec.y << "}";
	return os;
}


#endif