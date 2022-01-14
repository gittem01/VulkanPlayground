#version 450

layout (location = 0) in vec3 normal;
layout (location = 1) in vec4 objectColor;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 fragWorldPos;
layout (location = 4) in vec3 cameraPos;

layout (location = 0) out vec4 outFragColor;


struct LightData{
	vec4 position;
	vec4 color;
	vec4 strength; // use x only
};

layout(set = 0, binding = 1) uniform  SceneData{   
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; // x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; // w for sun power
	vec4 sunlightColor;
	int numOfLights[4]; // use x only
} sceneData;

layout(set = 2, binding = 0) uniform sampler2D tex1;

// all object matrices
layout(std140, set = 1, binding = 1) readonly buffer LightBuffer {
	LightData lights[];
} lightBuffer;

float ambientRatio = 0.01f;
float brightness = 0.50f;

void main() 
{	
	vec3 nNormal = normalize(normal);
	float diff = max(dot(vec4(normal, 1.0f), sceneData.sunlightDirection), 0.0);
	vec3 diffuse = vec3(diff * objectColor * (1 * brightness - ambientRatio));
	vec3 ambient = ambientRatio * objectColor.xyz * sceneData.ambientColor.xyz * brightness;
	vec3 texColor = texture(tex1, texCoord).xyz;

	vec3 viewDir = normalize(cameraPos - fragWorldPos);
	vec3 reflectDir = normalize(reflect(-sceneData.sunlightDirection.xyz, nNormal));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);
	vec3 specular = spec * sceneData.sunlightColor.xyz * sceneData.sunlightColor[3] * 100;

	vec3 finalColor = vec3(0, 0, 0);
	for (int i = 0; i < sceneData.numOfLights[0]; i++){
		vec3 lightDirection = fragWorldPos - lightBuffer.lights[i].position.xyz;
		float dist = length(lightDirection);
		vec3 reflectDir = normalize(reflect(lightDirection, nNormal));
		float pointColor = pow(max(dot(normal, reflectDir), 0.0), 2);
		finalColor += pointColor * lightBuffer.lights[i].color.xyz * lightBuffer.lights[i].strength.x * 100.0f / pow(dist, 2);
	}

	outFragColor = vec4((finalColor + ambient) * texColor, 1.0f);
}