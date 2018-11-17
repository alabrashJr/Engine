#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec4 color = vec4(1.0,1.0,1.0,1.0);
	gl_FragColor = color;
}