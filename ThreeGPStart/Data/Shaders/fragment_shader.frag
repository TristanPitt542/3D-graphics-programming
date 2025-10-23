#version 330

uniform sampler2D sampler_tex;

uniform vec3 light_positionvec3;
uniform vec3 light_intensity;

uniform int useLighting; // 1 for on, 0 for off
uniform int useTexture; // 1 for on, 0 for off

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_coord;
in vec3 varying_colour;

in vec3 varying_gouraud;

out vec4 fragment_colour;
out vec4 fragment_colour2;

void main(void)
{
	if (useLighting == 1)
	{
		// Lighting calculations
			vec3 diffuseColour = texture(sampler_tex, varying_coord).rgb;

         	vec3 N = normalize(varying_normal);
			vec3 P = varying_position;

			vec3 light_direction = vec3(0.5, -1, -0.3);
			vec3 L = normalize(-light_direction);

			float diffuse_intensity = dot(L,N);
	
			vec3 ambientLightColour = vec3(1,1,1);
			float ambientLightLevel = 0.3;

			vec3 final_colour = ambientLightLevel * ambientLightColour + diffuse_intensity * diffuseColour;
			fragment_colour = vec4(diffuseColour * diffuse_intensity, 1.0);
	}
	else
	{
		if (useTexture == 1)
		{
			// Just use texture color without lighting
			vec3 diffuseColour = texture(sampler_tex, varying_coord).rgb;
			fragment_colour = vec4(diffuseColour, 1.0);
		}
		else
		{
			fragment_colour = vec4(varying_colour, 1.0);
		}
	}
}
