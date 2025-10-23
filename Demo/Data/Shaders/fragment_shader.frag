#version 330

uniform sampler2D sampler_tex;

uniform vec4 diffuse_colour;

in vec3 varying_normal;
in vec2 varying_coord;
in vec3 varying_colour;

out vec4 fragment_colour;

void main(void)
{
	vec3 diffuseColour = texture(sampler_tex, varying_coord).rgb;

	fragment_colour = vec4(diffuseColour, 1.0);
	//fragment_colour = vec4(varying_colour, 1.0);
}