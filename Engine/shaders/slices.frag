#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 objCoord;

uniform sampler2D texture_diffuse1;

void main()
{
		vec4 tex = texture(texture_diffuse1, TexCoord);


		//vec4 col1 = vec4(tex.r, tex.g, tex.b, 1.0);
		//vec4 col2 = vec4(newCoord.x, newCoord.y, 0.0, 1.0);
		//vec4 col3 = vec4(objCoord.x, objCoord.y, objCoord.z, 1.0);
		vec4 col4 = vec4(tex.r, tex.r, tex.r, 1.0);
		
		if (tex.r > 0.5) // Draw bright areas of image
		{
			FragColor = col4;
		}
		else
		{
			discard; // do not draw anything
		}
}