#include "vk_engine.h"

SwapChain::SwapChain(void* engine){

    this->engine = engine;
    create();
}

void SwapChain::create(){
    VulkanEngine* vulkanEngine = (VulkanEngine*)engine;

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vulkanEngine->_chosenGPU);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    swapchainImageFormat = surfaceFormat.format;

    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = vulkanEngine->_surface;
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageExtent = extent;
    
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(vulkanEngine->_device, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
        printf("SwapChain creation error\n"); 
    }

    createImageResources();

    createDepthResources();
}

void SwapChain::destroy(){
    VulkanEngine* vulkanEngine = (VulkanEngine*)engine;

    for (int i = 0; i < swapchainImageViews.size(); i++) {
		vkDestroyFramebuffer(vulkanEngine->_device, framebuffers[i], nullptr);
		vkDestroyImageView(vulkanEngine->_device, swapchainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(vulkanEngine->_device, swapChain, nullptr);
	vkDestroyImageView(vulkanEngine->_device, depthImageView, nullptr);
	vmaDestroyImage(vulkanEngine->_allocator, depthImage._image, depthImage._allocation);
}

void SwapChain::createImageResources() {
    VulkanEngine* vulkanEngine = (VulkanEngine*)engine;
    
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(vulkanEngine->_device, swapChain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vulkanEngine->_device, swapChain, &imageCount, swapchainImages.data());

    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vulkanEngine->_device, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void SwapChain::createDepthResources(){
    VulkanEngine* vulkanEngine = (VulkanEngine*)engine;

	VkExtent3D depthImageExtent = {
		extent.width,
		extent.height,
		1
	};

	depthFormat = VK_FORMAT_D32_SFLOAT;

	VkImageCreateInfo dimg_info = vkinit::image_create_info(depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vmaCreateImage(vulkanEngine->_allocator, &dimg_info, &dimg_allocinfo, &depthImage._image, &depthImage._allocation, nullptr);

	VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(depthFormat, depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

	vkCreateImageView(vulkanEngine->_device, &dview_info, nullptr, &depthImageView);
}

void SwapChain::createFrameBuffers(){
    VulkanEngine* vulkanEngine = (VulkanEngine*)engine;

    //create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fb_info = {};
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = nullptr;

	fb_info.renderPass = vulkanEngine->_renderPass;
	fb_info.attachmentCount = 1;
	fb_info.width = extent.width;
	fb_info.height = extent.height;
	fb_info.layers = 1;

	//grab how many images we have in the swapchain
	const uint32_t swapchain_imagecount = swapchainImages.size();
	framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchain_imagecount; i++) {

		VkImageView attachments[2];
		attachments[0] = swapchainImageViews[i];
		attachments[1] = depthImageView;

		fb_info.pAttachments = attachments;
		fb_info.attachmentCount = 2;

		vkCreateFramebuffer(vulkanEngine->_device, &fb_info, nullptr, &framebuffers[i]);
	}
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice device) {
    VulkanEngine* vullkanEngine = (VulkanEngine*)engine;

    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vullkanEngine->_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vullkanEngine->_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vullkanEngine->_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vullkanEngine->_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vullkanEngine->_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    VulkanEngine* vullkanEngine = (VulkanEngine*)engine;

    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } 
    else {
        int width, height;
        SDL_Vulkan_GetDrawableSize(vullkanEngine->_window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}