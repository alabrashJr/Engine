#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 color;

void main()
{
    FragColor = texture(texture1, TexCoord);

	FragColor = vec4(color, 1);
}