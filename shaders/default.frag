#version 450

//shader input
layout (location = 0) in vec3 normal;
layout (location = 1) in vec4 objectColor;
layout (location = 2) in vec2 texCoord;

//output write
layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  SceneData{   
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

layout(set = 2, binding = 0) uniform sampler2D tex1;

void main() 
{	
	float diff = max(dot(vec4(normal, 1.0f), sceneData.sunlightDirection), 0.0);
	vec4 diffuseColor = diff * objectColor;
	vec3 texColor = texture(tex1, texCoord).xyz;
	//outFragColor = vec4(vec3(diffuse), 1.0f);
	outFragColor = vec4(texColor * diffuseColor.xyz, 1.0f);
}