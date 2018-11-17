#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
varying float intensity;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec4 color;
	if (intensity > 13)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 12)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 10)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);

	gl_FragColor = color;
}