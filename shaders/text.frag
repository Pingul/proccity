#version 330

in vec2 tex;
out vec4 out_color;

uniform sampler2D textTexture;

void main()
{
	vec4 color = texture(textTexture, tex);  
	if (color.r == 0)
		out_color = vec4(0.0, 0.0, 0.0, 0.0);
	else
		out_color = vec4(color.r, color.r, color.r, 1.0);
}