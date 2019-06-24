#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
varying float intensity;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDir;

uniform float scaleX;
uniform float scaleY;
uniform float scaleZ;


void main()
{
	intensity = dot (lightDir, mat3(transpose(inverse(model))) * aNormal);
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4( vec3((aPos.x * scaleX), (aPos.y * scaleY), (aPos.z * scaleZ)), 1.0);
}