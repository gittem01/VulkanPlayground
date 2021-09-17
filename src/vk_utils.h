#pragma once

#include "vk_types.h"
#include <string>

namespace vkutil {
	bool load_image(void* engine, std::string fileName, AllocatedImage& outImage);
	VkImageMemoryBarrier& insertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);
}
