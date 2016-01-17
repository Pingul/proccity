#version 150

in vec2 tex;
out vec4 out_color;

uniform sampler2D windowTexture;

void main()
{
	out_color = texture(windowTexture, tex);
	// out_color = vec4(tex.x, tex.x, tex.x, 1.0);
}