#pragma once

#include "vk_types.h"
#include <vk_initializers.h>
#include "vk_mesh.h"
#include "vk_shader.h"
#include "vk_mem_alloc.h"
#include "Camera3D.h"
#include "GameObject.h"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include "vk_swapchain.h"
#include "vk_pipelinebuilder.h"
#include "stb_image.h"
#include "SDL.h"
#include "SDL_vulkan.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_vulkan.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <functional>
#include <unordered_map>
#include <vector>
#include <math.h>

#define VK_CHECK(x) {												\
	VkResult err = x;												\
	if (err)														\
	{																\
		std::cout << "Detected Vulkan error: " << err << std::endl;	\
		abort();													\
	}																\
}

constexpr unsigned int FRAME_OVERLAP = 2;

struct UploadContext {
	VkFence _uploadFence;
	VkCommandPool _commandPool;
};

struct GPUObjectData{
	glm::mat4 modelMatrix;
	glm::mat4 rotationMatrix;
	glm::vec4 objectColor;
};

struct LightData{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 strength;
};

struct GPUSceneData {
	glm::vec4 fogColor; // w for exponent
	glm::vec4 fogDistances; // x for min, y for max, zw unused.
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; // w for sun power
	glm::vec4 sunlightColor;
	int numOflights[4];
};

struct GPUCameraData {
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec3 cameraPos;
};

struct Material {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

struct Texture {
	AllocatedImage image;
	VkImageView imageView;
	VkSampler sampler;
	VkDescriptorSet textureSet;
};

struct FrameData {
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	AllocatedBuffer worldBuffer;
	AllocatedBuffer objectBuffer;

	VkDescriptorSet objectDescriptor;
};

class VulkanEngine {

friend class GameObject;
friend class SwapChain;

public:

	SDL_Window* window;
	VkExtent2D winExtent;
	ImGuiIO* io;
	Camera3D* camera;

	uint32_t frameNumber;


	bool isPaused;
	bool tick;

	float dpiScaling = 1.0f;

	VulkanEngine(uint32_t width, uint32_t height);
	~VulkanEngine();

	void get_mesh(std::string meshPath, const char* meshName);
	void get_image(std::string imagePath, const char* imageName, VkFilter filter = VK_FILTER_LINEAR);

	// Create material and add it to the map
	Material* create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);
	Material* get_material(const std::string& name);

	VmaAllocator getAllocator(){return _allocator; }

	// main loop
	bool looper();

private:
	std::vector<GameObject*> gameObjects;
	
	std::unordered_map<std::string, Texture> _loadedTextures;
	std::unordered_map<std::string, Material> _materials;
	std::unordered_map<std::string, Mesh> _meshes;

	std::vector<LightData> lights;

	const int MAX_OBJECTS = 10000;
	const int MAX_LIGHTS = 100;

	std::vector<const char*> _requiredExtensions;
	std::vector<const char*> _deviceExtensions = {
    	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	bool _isInitialized = false;

	SwapChain* _swapChain;

	uint32_t lastSwapchainImageIndex;

	VkSampleCountFlagBits desiredSamples = VK_SAMPLE_COUNT_4_BIT;
	VkSampleCountFlagBits samples;

	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkPhysicalDeviceProperties _GPUProperties;
	VkDevice _device;
	VkSurfaceKHR _surface;

	FrameData _frames[FRAME_OVERLAP];

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;

	VkRenderPass _renderPass;

	VkPipeline _defaultPipeline;

	VkPipelineLayout _defaultPipelineLayout;

	VkDescriptorSetLayout _globalSetLayout;
	VkDescriptorSetLayout _objectSetLayout;
	VkDescriptorSetLayout _singleTextureSetLayout;

	VkDescriptorPool _descriptorPool;
	VkDescriptorPool _imguiPool;

	VkDescriptorSet _globalDescriptor;

	VmaAllocator _allocator;

	GPUSceneData _sceneParameters;
	AllocatedBuffer _worldBuffers;

	UploadContext _uploadContext;

	FrameData& get_current_frame();
	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	size_t pad_uniform_buffer_size(size_t originalSize);
	size_t pad_storage_buffer_size(size_t originalSize);

	Mesh* get_mesh(const std::string& name);

	void draw_objects(VkCommandBuffer cmd);
	VkCommandBuffer beginOneTimeSubmit();
	void endOneTimeSubmit(VkCommandBuffer cmdBuffer);
	void setSamples();

	// initializes everything in the engine
	void init(uint32_t width, uint32_t height);

	// shuts down the engine
	void cleanup();

	// render loop
	void render(ImDrawData* draw_data);
	ImDrawData* imguiLoop();

	void windowResizeEvent();
	int eventHandler();

	void init_instance();
	bool is_device_suitable(VkPhysicalDevice physicalDevice);
	void select_physical_device();
	void create_logical_device();
	void init_vulkan();
	void init_commands();
	void init_default_renderpass();
	void init_sync_structures();
	void init_descriptors();
	void init_pipelines();
	void init_imgui();
	void update_image_descriptors(Texture* tex);

	void upload_mesh(Mesh& mesh);
	bool load_image(std::string fileName, AllocatedImage& outImage);

	VkImageMemoryBarrier insertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);
};
