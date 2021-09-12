#include "vulkan/vulkan.h"
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {

public:
    SwapChain(void* engine);

    VkExtent2D extent;
    VkSwapchainKHR swapChain;
    VkFormat swapchainImageFormat;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
    
    std::vector<VkFramebuffer> framebuffers;

	VkImageView depthImageView;
	AllocatedImage depthImage;
	VkFormat depthFormat;

    void create();
    void destroy();
    void creationLoop();

    void createFrameBuffers(); // need to be created after renderpass, hence seperate

private:
    void* engine;

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void createImageResources();
    void createDepthResources();
};
