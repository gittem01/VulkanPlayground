#version 460
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec4 objectColor;

layout(set = 0, binding = 0) uniform CameraData{
    mat4 view;
    mat4 proj;
	mat4 viewproj;
} cameraData;

struct ObjectData{
	mat4 model;
	vec4 color;
};

// all object matrices
layout(std140, set = 1, binding = 0) readonly buffer ObjectBuffer {

	ObjectData objects[];
} objectBuffer;

void main()
{
	mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].model;
	mat4 transformMatrix = (cameraData.viewproj * modelMatrix);
	objectColor = objectBuffer.objects[gl_BaseInstance].color;
	gl_Position = transformMatrix * vec4(vPosition, 1.0f);
	normal = vNormal;
}