#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
varying float intensity;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec4 color = vec4(0.2,0.2,0.2,1.0);
	gl_FragColor = color;
}