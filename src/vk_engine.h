#pragma once

#include "vk_types.h"
#include <vk_initializers.h>
#include "vk_mesh.h"
#include "vk_shader.h"
#include "vk_mem_alloc.h"
#include "VkBootstrap.h"
#include "Camera3D.h"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include "vk_swapchain.h"
#include "vk_pipelinebuilder.h"
#include "vk_utils.h"
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <functional>
#include <unordered_map>
#include <math.h>

constexpr unsigned int FRAME_OVERLAP = 2;

struct UploadContext {
	VkFence _uploadFence;
	VkCommandPool _commandPool;
};

struct GPUObjectData{
	glm::mat4 modelMatrix;
	glm::vec4 objectColor;
};

struct GPUSceneData {
	glm::vec4 fogColor; // w is for exponent
	glm::vec4 fogDistances; //x for min, y for max, zw unused.
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; //w for sun power
	glm::vec4 sunlightColor;
};

struct GPUCameraData {
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
};

struct MeshPushConstants {
	glm::vec4 data;
	glm::mat4 render_matrix;
};

struct Material {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSet textureSet{ VK_NULL_HANDLE };
};

struct Texture {
	AllocatedImage image;
	VkImageView imageView;
	VkSampler sampler;
};

struct RenderObject {
	Mesh* mesh;

	Material* material;

	glm::mat4 transformMatrix;
	glm::vec4 color;
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
public:
	std::vector<RenderObject> _renderables;
	
	std::unordered_map<std::string, Texture> _loadedTextures;
	std::unordered_map<std::string, Material> _materials;
	std::unordered_map<std::string, Mesh> _meshes;
	
	SwapChain* _swapChain;

	bool _isInitialized = false;
	int _frameNumber = 0;

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

	VkPipeline _meshPipeline;

	VkPipelineLayout _meshPipelineLayout;

	VkDescriptorSetLayout _globalSetLayout;
	VkDescriptorSetLayout _objectSetLayout;
	VkDescriptorSetLayout _singleTextureSetLayout;

	VkDescriptorPool _descriptorPool;

	VkDescriptorSet _globalDescriptor;

	VmaAllocator _allocator;

	GPUSceneData _sceneParameters;
	AllocatedBuffer _worldBuffers;

	UploadContext _uploadContext;

	VkExtent2D _windowExtent = { 1200 , 700 };

	struct SDL_Window* _window{ NULL };

	WindowHandler* wHandler;
	Camera3D* camera;

	// Create material and add it to the map
	Material* create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);
	Material* get_material(const std::string& name);
	Mesh* get_mesh(const std::string& name);
	void draw_objects(VkCommandBuffer cmd, RenderObject* first, int count);

	FrameData& get_current_frame();
	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	size_t pad_uniform_buffer_size(size_t originalSize);
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
	void setSamples();

	// Initializes everything in the engine
	void init();

	// Shuts down the engine
	void cleanup();

	// Draw loop
	void draw();

	// Run main loop
	void run();

private:
	void windowResizeEvent();

	void init_vulkan();
	void init_commands();
	void init_default_renderpass();
	void init_sync_structures();
	void init_descriptors();
	void init_pipelines();
	void load_meshes();
	void load_images();
	void init_scene();

	void init_mesh_descriptors();

	void upload_mesh(Mesh& mesh);
};
