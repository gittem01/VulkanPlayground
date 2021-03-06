#pragma once

#include <vk_types.h>

namespace vkinit {

	VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

	VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule);

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info(VkPrimitiveTopology topology);

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info(VkPolygonMode polygonMode);

	VkPipelineMultisampleStateCreateInfo multisampling_state_create_info(VkSampleCountFlagBits samples);

	VkPipelineColorBlendAttachmentState color_blend_attachment_state();

	VkPipelineLayoutCreateInfo pipeline_layout_create_info();

	VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent, VkSampleCountFlagBits samples);

	VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

	VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

	VkFenceCreateInfo fence_create_info(VkFenceCreateFlagBits flags);

	VkSemaphoreCreateInfo semaphore_create_info();

	VkDescriptorSetLayoutBinding descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding);

	VkWriteDescriptorSet write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo , uint32_t binding);

	VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags);

	VkSubmitInfo submit_info(VkCommandBuffer* cmd);

	VkSamplerCreateInfo sampler_create_info(VkFilter filters, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

	VkWriteDescriptorSet write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding);

}

