#version 450

layout (location = 0) in vec3 normal;
layout (location = 1) in vec4 objectColor;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 fragWorldPos;
layout (location = 4) in vec3 cameraPos;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  SceneData{   
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

layout(set = 2, binding = 0) uniform sampler2D tex1;

float ambientRatio = 0.20f;
float specularStrength = 10.0f;

void main() 
{	
	float diff = max(dot(vec4(normal, 1.0f), sceneData.sunlightDirection), 0.0);
	vec3 diffuse = vec3(diff * objectColor * (1 - ambientRatio));
	vec3 ambient = ambientRatio * objectColor.xyz;
	vec3 texColor = texture(tex1, texCoord).xyz;

	vec3 viewDir = normalize(cameraPos - fragWorldPos);
	vec3 reflectDir = reflect(-vec3(sceneData.sunlightDirection), normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * vec3(1, 1, 1);  

	//outFragColor = vec4(vec3(diffuse), 1.0f);
	outFragColor = vec4((diffuse + ambient + specular) * texColor, 1.0f);
}