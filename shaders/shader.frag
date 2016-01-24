#version 150

in vec2 tex;
out vec4 out_color;

uniform bool showColor;
uniform vec3 uniformColor;

uniform sampler2D windowTexture;

void main()
{
	if (showColor)
		out_color = vec4(uniformColor, 1.0);
	else
		out_color = texture(windowTexture, tex);
}