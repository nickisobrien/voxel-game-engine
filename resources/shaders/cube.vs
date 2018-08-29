#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in float aTorchLight;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoord;
out vec3 Norm;
out float TorchLight;

void main()
{
	TorchLight = aTorchLight;
	TexCoord = aTexCoord;
	Norm = aNorm;
	gl_Position = projection * view * transform * vec4(aPos, 1.0f);
}