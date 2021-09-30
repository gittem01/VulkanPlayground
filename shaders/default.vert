#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec4 objectColor;
layout (location = 2) out vec2 texCoord;
layout (location = 3) out vec3 fragWorldPos;
layout (location = 4) out vec3 cameraPos;

layout(set = 0, binding = 0) uniform CameraData{
    mat4 view;
    mat4 proj;
	mat4 viewproj;
	vec3 cameraPos;
} cameraData;

struct ObjectData{
	mat4 model;
	mat4 rotationMatrix;
	vec4 color;
};

// all object matrices
layout(std140, set = 1, binding = 0) readonly buffer ObjectBuffer {

	ObjectData objects[];
} objectBuffer;

void main() {
	mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].model;
	mat4 rotationMatrix = objectBuffer.objects[gl_BaseInstance].rotationMatrix;
	mat4 transformMatrix = cameraData.viewproj * modelMatrix;
	objectColor = objectBuffer.objects[gl_BaseInstance].color;
	normal = vec3(rotationMatrix * vec4(vNormal, 1));
	texCoord = vTexCoord;
	fragWorldPos = vec3(modelMatrix * vec4(vPosition, 1.0f));
	cameraPos = cameraData.cameraPos;
	gl_Position = transformMatrix * vec4(vPosition, 1.0f);
}