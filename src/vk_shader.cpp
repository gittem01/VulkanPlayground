#include "vk_shader.h"
#include "vk_initializers.h"

VulkanShader::VulkanShader(VkDevice device, std::vector<std::string> fileNames) {
	this->device = device;
	this->shaderStages.resize(fileNames.size());
	
	VkShaderStageFlagBits bits[3] = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT, VK_SHADER_STAGE_GEOMETRY_BIT };

	for (int i = 0; i < fileNames.size(); i++) {
		VkShaderModule shaderModule = load_shader_module(device, fileNames[i].c_str());
		shaderModules.push_back(shaderModule);
		shaderStages[i] = vkinit::pipeline_shader_stage_create_info(bits[i], shaderModule);
	}
}

VulkanShader::~VulkanShader() {
	for (int i = 0; i < shaderModules.size(); i++) {
		vkDestroyShaderModule(device, shaderModules.at(i), NULL);
	}
	shaderModules.clear();
	shaderStages.clear();
}

VkShaderModule VulkanShader::load_shader_module(VkDevice device, const char* filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		return NULL;
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
	file.seekg(0);
	file.read((char*)buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();
	
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
		return NULL;
	}

	return shaderModule;
}