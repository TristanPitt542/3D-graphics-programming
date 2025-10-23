#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;
uniform vec3 light_position;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_colour;



out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_coord;
out vec3 varying_gouraud;
out vec3 varying_colour;

void main(void)
{	
	varying_coord = vertex_texcoord;

	varying_colour = vertex_colour;

	varying_position = (model_xform * vec4(vertex_position, 1.0)).xyz;
	varying_normal = (model_xform * vec4(vertex_normal, 0.0)).xyz;

	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
}