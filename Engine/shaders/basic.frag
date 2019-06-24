#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
varying float intensity;

uniform sampler2D texture_diffuse1;
uniform vec4 color;

void main()
{  
	gl_FragColor = color;
}