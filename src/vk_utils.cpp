#include "vk_engine.h"

bool VulkanEngine::load_image(std::string fileName, AllocatedImage& outImage)
{
	int width, height, nChannels;
	stbi_uc* pixels = stbi_load(fileName.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);

	if (!pixels) {
		std::cout << "Failed to load texture file: " << fileName << std::endl;
		return false;
	}

	VkDeviceSize imageSize = width * height * (uint64_t)4;

	VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

	AllocatedBuffer stagingBuffer = create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(getAllocator(), stagingBuffer._allocation, &data);

	memcpy(data, (void*)pixels, static_cast<size_t>(imageSize));

	vmaUnmapMemory(getAllocator(), stagingBuffer._allocation);
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

	vmaCreateImage(getAllocator(), &dimg_info, &dimg_allocinfo, &newImage._image, &newImage._allocation, NULL);
	
	// one time submit start
	VkCommandBuffer commandBuffer = beginOneTimeSubmit();
	VkImageMemoryBarrier imageBarrier_toTransfer = insertImageMemoryBarrier(commandBuffer, newImage._image, 0,
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
	);

	VkBufferImageCopy copyRegion = {};
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;

	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;
	copyRegion.imageExtent = imageExtent;

	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer._buffer, newImage._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

	VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

	imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
		NULL, 0, NULL, 1, &imageBarrier_toReadable);
	endOneTimeSubmit(commandBuffer);
	// one time submit end

	vmaDestroyBuffer(getAllocator(), stagingBuffer._buffer, stagingBuffer._allocation);

	std::cout << "Texture file: " << fileName << " loaded successfully " << std::endl;

	outImage = newImage;

	return true;
}

VkImageMemoryBarrier VulkanEngine::insertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange)
{
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	vkCmdPipelineBarrier(cmdbuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	return imageMemoryBarrier;
}

VkCommandBuffer VulkanEngine::beginOneTimeSubmit() {
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_uploadContext._commandPool, 1);
	VkCommandBuffer cmdBuffer;
	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &cmdBuffer));

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo));

	return cmdBuffer;
}

void VulkanEngine::endOneTimeSubmit(VkCommandBuffer cmdBuffer) {
	VK_CHECK(vkEndCommandBuffer(cmdBuffer));

	VkSubmitInfo submit = vkinit::submit_info(&cmdBuffer);
	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _uploadContext._uploadFence));

	vkWaitForFences(_device, 1, &_uploadContext._uploadFence, true, UINT64_MAX);
	vkResetFences(_device, 1, &_uploadContext._uploadFence);
	vkResetCommandPool(_device, _uploadContext._commandPool, 0);
}


void VulkanEngine::upload_mesh(Mesh& mesh) {
	const size_t bufferSize = mesh._vertices.size() * sizeof(Vertex);
	// allocate staging buffer
	VkBufferCreateInfo stagingBufferInfo = {};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.pNext = NULL;

	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	AllocatedBuffer stagingBuffer;

	// allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &stagingBufferInfo, &vmaallocInfo,
		&stagingBuffer._buffer, &stagingBuffer._allocation, NULL)
	);

	void* data;
	vmaMapMemory(_allocator, stagingBuffer._allocation, &data);

	memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(_allocator, stagingBuffer._allocation);

	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = NULL;
	vertexBufferInfo.size = bufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	// allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &vertexBufferInfo, &vmaallocInfo,
		&mesh._vertexBuffer._buffer, &mesh._vertexBuffer._allocation, NULL)
	);

	VkCommandBuffer commandBuffer = beginOneTimeSubmit();
	VkBufferCopy copy;
	copy.dstOffset = 0;
	copy.srcOffset = 0;
	copy.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, stagingBuffer._buffer, mesh._vertexBuffer._buffer, 1, &copy);
	endOneTimeSubmit(commandBuffer);

	vmaDestroyBuffer(_allocator, stagingBuffer._buffer, stagingBuffer._allocation);
}
