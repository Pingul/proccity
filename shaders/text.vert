#version 330

in vec3 in_Position;
in vec2 texCoord;
out vec2 tex;

void main()
{
	tex = texCoord;
	gl_Position = vec4(in_Position, 1.0);
}