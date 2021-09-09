#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>

class VulkanShader {
public:
	VulkanShader(VkDevice device, std::string fileNames[], size_t count);
	~VulkanShader();
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	std::vector<VkShaderModule> shaderModules;
	VkDevice device;

private:
	VkShaderModule load_shader_module(VkDevice device, const char* filePath);
};