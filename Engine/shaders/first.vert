#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 objCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 objectCoord;

void main()
{
	objCoord = objectCoord;
	//vec2 newCoord;
	// diveded by 2.0 for uppper left corner of and fasjdflasjdlkfj
	//newCoord.x = aTexCoord.x + (objectCoord.x / 2.0);
	//newCoord.y = aTexCoord.y + (objectCoord.z / 2.0);
	//newCoord = aTexCoord;

	// uniform for division scale
	// it equals to number of squares along a side
	//newCoord *= (1.0 / 32.0);

    gl_Position = projection * view * (model) * vec4((aPos * 50.0), 1.0);
    //TexCoord = newCoord;
    TexCoord = TexCoord;

}