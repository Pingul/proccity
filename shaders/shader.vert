#version 330

uniform mat4 MTW;
uniform mat4 WTV;
uniform mat4 proj;

in vec3 in_Position;
in vec2 texCoord;
out vec2 tex;

void main()
{
	tex = texCoord;
	// pos = vec3(in_Position.z, in_Position.z, in_Position.z);
	gl_Position = proj*WTV*MTW*vec4(in_Position, 1.0);
}