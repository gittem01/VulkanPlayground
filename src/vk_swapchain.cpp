#include "vk_engine.h"
#include <algorithm>

SwapChain::SwapChain(void* engine){

    this->engine = engine;

    create();
}

void SwapChain::create(){
    VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vulkanEngine->_chosenGPU);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    swapchainImageFormat = surfaceFormat.format;

    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vulkanEngine->winExtent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = vulkanEngine->_surface;
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageExtent = vulkanEngine->winExtent;
    
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(vulkanEngine->_device, &swapChainCreateInfo, NULL, &swapChain) != VK_SUCCESS) {
        printf("SwapChain creation error\n"); 
    }

    createImageResources();
    if (vulkanEngine->samples > VK_SAMPLE_COUNT_1_BIT) createColorResources();
    createDepthResources();
}

void SwapChain::creationLoop() {
    VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    for (;;) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vulkanEngine->_chosenGPU);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        if (extent.width > 0 && extent.height > 0) {
            create();
            return;
        }
        else {
            SDL_PumpEvents();
        }
    }
}

void SwapChain::destroy(){
    VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    for (int i = 0; i < swapchainImageViews.size(); i++) {
        vkDestroyFramebuffer(vulkanEngine->_device, framebuffers[i], NULL);
        vkDestroyImageView(vulkanEngine->_device, swapchainImageViews[i], NULL);
    }


    vkDestroySwapchainKHR(vulkanEngine->_device, swapChain, NULL);
    vkDestroyImageView(vulkanEngine->_device, depthImageView, NULL);
    vmaDestroyImage(vulkanEngine->_allocator, depthImage._image, depthImage._allocation);
    if (vulkanEngine->samples > VK_SAMPLE_COUNT_1_BIT) {
        vkDestroyImageView(vulkanEngine->_device, colorImageView, NULL);
        vmaDestroyImage(vulkanEngine->_allocator, colorImage._image, colorImage._allocation);
    }
}

void SwapChain::createImageResources() {
    VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(vulkanEngine->_device, swapChain, &imageCount, NULL);
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

        if (vkCreateImageView(vulkanEngine->_device, &createInfo, NULL, &swapchainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void SwapChain::createColorResources() {
    VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    VkExtent3D colorImageExtent = {
        vulkanEngine->winExtent.width,
        vulkanEngine->winExtent.height,
        1
    };

    VkImageCreateInfo cimg_info;
    cimg_info = vkinit::image_create_info(swapchainImageFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, colorImageExtent, vulkanEngine->samples);

    VmaAllocationCreateInfo cimg_allocinfo = {};
    cimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    cimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vmaCreateImage(vulkanEngine->_allocator, &cimg_info, &cimg_allocinfo, &colorImage._image, &colorImage._allocation, NULL);

    VkImageViewCreateInfo cview_info = vkinit::imageview_create_info(swapchainImageFormat, colorImage._image, VK_IMAGE_ASPECT_COLOR_BIT);

    vkCreateImageView(vulkanEngine->_device, &cview_info, NULL, &colorImageView);
}

void SwapChain::createDepthResources(){
    VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

	VkExtent3D depthImageExtent = {
        vulkanEngine->winExtent.width,
        vulkanEngine->winExtent.height,
		1
	};

	depthFormat = VK_FORMAT_D32_SFLOAT;

	VkImageCreateInfo dimg_info = vkinit::image_create_info(depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        depthImageExtent, vulkanEngine->samples);

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vmaCreateImage(vulkanEngine->_allocator, &dimg_info, &dimg_allocinfo, &depthImage._image, &depthImage._allocation, NULL);

	VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(depthFormat, depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

	vkCreateImageView(vulkanEngine->_device, &dview_info, NULL, &depthImageView);
}

void SwapChain::createFrameBuffers(){
    VulkanEngine* vulkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    //create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fb_info = {};
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = NULL;

	fb_info.renderPass = vulkanEngine->_renderPass;
	fb_info.width = vulkanEngine->winExtent.width;
	fb_info.height = vulkanEngine->winExtent.height;
	fb_info.layers = 1;

    uint32_t swapchain_imagecount;
    swapchain_imagecount = swapchainImages.size();
	framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++) {

		std::vector<VkImageView> attachments;
        if (vulkanEngine->samples > VK_SAMPLE_COUNT_1_BIT) attachments = { colorImageView, depthImageView, swapchainImageViews[i] };
        else attachments = { swapchainImageViews[i], depthImageView };
        
		fb_info.pAttachments = attachments.data();
		fb_info.attachmentCount = attachments.size();

		vkCreateFramebuffer(vulkanEngine->_device, &fb_info, NULL, &framebuffers[i]);
	}
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice device) {
    VulkanEngine* vullkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vullkanEngine->_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vullkanEngine->_surface, &formatCount, NULL);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vullkanEngine->_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vullkanEngine->_surface, &presentModeCount, NULL);

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
    // masking is fine since fifo relaxed is not gonna be used
    uint32_t mask = 0;
    for (const auto& availablePresentMode : availablePresentModes) {
        mask |= availablePresentMode;
    }

    if (mask & VK_PRESENT_MODE_MAILBOX_KHR) return VK_PRESENT_MODE_MAILBOX_KHR;
    else if (mask & VK_PRESENT_MODE_FIFO_KHR) return VK_PRESENT_MODE_FIFO_KHR;
    else return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    VulkanEngine* vullkanEngine = reinterpret_cast<VulkanEngine*>(engine);

    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } 
    else {
        int width, height;
        SDL_Vulkan_GetDrawableSize(vullkanEngine->window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}