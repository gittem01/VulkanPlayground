#include "vk_engine.h"

bool vkutil::load_image(void* engine, std::string fileName, AllocatedImage& outImage)
{
	VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

	int width, height, nChannels;
	stbi_uc* pixels = stbi_load(fileName.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);

	if (!pixels) {
		std::cout << "Failed to load texture file: " << fileName << std::endl;
		return false;
	}

	VkDeviceSize imageSize = width * height * (uint64_t)4;

	VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

	AllocatedBuffer stagingBuffer = vulkanEngine->create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(vulkanEngine->_allocator, stagingBuffer._allocation, &data);

	memcpy(data, (void*)pixels, static_cast<size_t>(imageSize));

	vmaUnmapMemory(vulkanEngine->_allocator, stagingBuffer._allocation);
	stbi_image_free(pixels);

	VkExtent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(width);
	imageExtent.height = static_cast<uint32_t>(height);
	imageExtent.depth = 1;

	VkImageCreateInfo dimg_info = vkinit::image_create_info(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, 
		imageExtent, VK_SAMPLE_COUNT_1_BIT);

	AllocatedImage newImage;

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	vmaCreateImage(vulkanEngine->_allocator, &dimg_info, &dimg_allocinfo, &newImage._image, &newImage._allocation, NULL);

	vulkanEngine->immediate_submit([&](VkCommandBuffer cmd) {
		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image = newImage._image;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
			NULL, 0, NULL, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = imageExtent;

		vkCmdCopyBufferToImage(cmd, stagingBuffer._buffer, newImage._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
			NULL, 0, NULL, 1, &imageBarrier_toReadable);
	});

	vmaDestroyBuffer(vulkanEngine->_allocator, stagingBuffer._buffer, stagingBuffer._allocation);

	std::cout << "Texture file: " << fileName << " loaded successfully " << std::endl;

	outImage = newImage;

	return true;
}