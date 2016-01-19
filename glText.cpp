#include "glText.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <iomanip>


// class Font 
// {
// public:
// 	Font(const char* path);



// 	static const char* FONT_KEYBOARD;
// 	static const Font& keyboard() { return *_keyboard; }
// private:
// 	FT_Library _library;
// 	FT_Face _face;

// 	static Font* _keyboard;
// };

// const char* Font::FONT_KEYBOARD = "/System/Library/Fonts/Keyboard.ttf";
// Font* Font::_keyboard = new Font(Font::FONT_KEYBOARD);

// Font::Font(const char* path)
// {
// 	int error = FT_Init_FreeType(&_library);
// 	error = FT_New_Face(_library, path, 0, &_face);
// 	error = FT_Set_Char_Size(_face, 0, 16*64, 500, 500);

// 	if (error)
// 		std::cout << "error while initing font" << std::endl;

// }

Textbox::Textbox(glm::vec3 position, Size2<float> size)
: Drawable(position, Size3<float>{size.width, size.height, 0.0})
{
	_vertices = {
		_position.x, _position.y, position.z,
		_position.x + _size.width, _position.y, _position.z,
		_position.x, _position.y + _size.height, _position.z,
		_position.x + _size.width, _position.y + _size.height, _position.z
	};

	_texCoords = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};

	_elements = {
		0, 1, 3,
		0, 2, 3
	};
}

void Textbox::render()
{
	FT_Library library;
	FT_Face face;
	int error = FT_Init_FreeType(&library);
	error = FT_New_Face(library, "resources/carbon.ttf", 0, &face);
	error = FT_Set_Char_Size(face, 0, 16*64, 500, 500);
	
	int min_width{0};
	Size2<int> textureSize{0, 0};
	_texture.setSize(textureSize);

	int pen_x{0};
	FT_GlyphSlot slot{face->glyph};
	for (auto& c : _content)
	{
		error = FT_Load_Char(face, c, FT_LOAD_RENDER);
		min_width += slot->bitmap.rows;
		if (face->size->metrics.y_ppem > textureSize.height)
		{
			textureSize.height = face->size->metrics.y_ppem;
			textureSize.width = (int)textureSize.height*_content.size();
			_texture.crop(textureSize);
		}
		for (int i = 0; i < slot->bitmap.rows; i++)
		{
			for (int j = 0; j < slot->bitmap.width; j++)
			{
				unsigned char color{slot->bitmap.buffer[i*slot->bitmap.width + j]};
				int paddingBot{5};
				size_t index{_texture.index(j + pen_x, slot->bitmap_top - i + paddingBot)};
				float v{((float)color)/255.f};
				_texture.pixels[3*index] = v;
				_texture.pixels[3*index + 1] = v;
				_texture.pixels[3*index + 2] = v;
			}
		}

		pen_x += slot->advance.x >> 6;
	}
	textureSize.width = min_width;
	_texture.crop(textureSize);
	// _texture = texture;
		
	FT_Done_FreeType(library);
}

void Textbox::uploadTexture()
{
	glBindVertexArray(_VAO);
	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texture.size().width, _texture.size().height, 0, GL_RGB, GL_FLOAT, _texture.pixels.data());
}

void Textbox::setText(std::string text)
{
	_content = text;
	render();
	if (_loaded)
		uploadTexture();
}


void Textbox::upload(GLuint programID)
{
	render();
	Drawable::upload(programID);
	
	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(_program, "textTexture"), 1);
	_loaded = true;
}

#include "glUtilities.hpp"

void Textbox::draw()
{
	glBindVertexArray(_VAO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _textureHandle);
	glDrawElements(GL_TRIANGLES, _elements.size(), GL_UNSIGNED_INT, 0);
}