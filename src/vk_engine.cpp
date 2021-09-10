#define VMA_IMPLEMENTATION

#include "vk_engine.h"
#include <string>

#ifndef NDEBUG
	#define IS_DEBUG 1
#else
	#define IS_DEBUG 0
#endif // NDEBUG

#define VK_CHECK(x){												\
	VkResult err = x;												\
	if (err)														\
	{																\
		std::cout <<"Detected Vulkan error: " << err << std::endl;	\
		abort();													\
	}																\
}

float getRand01(){
	return ((double)rand()) / RAND_MAX;
}

void VulkanEngine::run()
{
	bool done = false;

	while (!done)
	{
		if (!painter->looper()) done = true;
		camera->update();
		draw();
	}
}

void VulkanEngine::init()
{
	painter = new WindowHandler(_windowExtent.width, _windowExtent.height);
	camera = new Camera3D(glm::vec3(0.0f, 0.0f, 10.0f), painter);

	_window = painter->window;

	init_vulkan();

	this->_swapChain = new SwapChain((void*)this);

	init_default_renderpass();

	_swapChain->createFrameBuffers();
	
	init_commands();
		
	init_sync_structures();
	
	init_descriptors();
	
	init_pipelines();

	load_meshes();
	
	init_scene();

	_isInitialized = true;
}

void VulkanEngine::windowResizeEvent(){

	vkDeviceWaitIdle(_device);
	vkDestroyCommandPool(_device, _uploadContext._commandPool, nullptr);
	for (int i = 0; i < FRAME_OVERLAP; i++) {
		vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
	}

	vkDestroyPipeline(_device, _meshPipeline, nullptr);
	vkDestroyPipelineLayout(_device, _meshPipelineLayout, nullptr);
	vkDestroyRenderPass(_device, _renderPass, nullptr);

	for (int i=0; i<FRAME_OVERLAP; i++){
		vmaDestroyBuffer(_allocator, _frames[i].objectBuffer._buffer , _frames[i].objectBuffer._allocation);
	}

	vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(_device, _globalSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(_device, _objectSetLayout, nullptr);

	vmaDestroyBuffer(_allocator, _worldBuffers._buffer , _worldBuffers._allocation);

	_swapChain->destroy();
	_swapChain->create();
	
	init_commands();
	init_default_renderpass();
	_swapChain->createFrameBuffers();
	init_descriptors();
	init_pipelines();
}

void VulkanEngine::cleanup(){
	if (_isInitialized) {
		vkDeviceWaitIdle(_device);
		
		for (auto iter = _meshes.begin(); iter != _meshes.end(); iter++){
			Mesh m = iter->second;
			vmaDestroyBuffer(_allocator, m._vertexBuffer._buffer, m._vertexBuffer._allocation);
		}

		vkDestroyPipeline(_device, _meshPipeline, nullptr);
		vkDestroyPipelineLayout(_device, _meshPipelineLayout, nullptr);

		for (int i=0; i<FRAME_OVERLAP; i++){
			vmaDestroyBuffer(_allocator, _frames[i].objectBuffer._buffer , _frames[i].objectBuffer._allocation);
		}

		vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(_device, _globalSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(_device, _objectSetLayout, nullptr);

		vmaDestroyBuffer(_allocator, _worldBuffers._buffer , _worldBuffers._allocation);

		vkDestroyFence(_device, _uploadContext._uploadFence, nullptr);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
			vkDestroySemaphore(_device, _frames[i]._presentSemaphore, nullptr);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
		}

		_swapChain->destroy();

		vkDestroyRenderPass(_device, _renderPass, nullptr);

		vkDestroyCommandPool(_device, _uploadContext._commandPool, nullptr);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
		}


		vmaDestroyAllocator(_allocator);

#if IS_DEBUG
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
#endif

		vkDestroySurfaceKHR(_instance, _surface, nullptr);

		vkDestroyDevice(_device, nullptr);
		vkDestroyInstance(_instance, nullptr);

		SDL_DestroyWindow(_window);
	}
}

void VulkanEngine::draw()
{
	// wait until the GPU has finished rendering the last frame. Timeout of 1 second

	uint32_t swapchainImageIndex;
	VkResult result = vkAcquireNextImageKHR(_device, _swapChain->swapChain, UINT64_MAX, get_current_frame()._presentSemaphore, nullptr, &swapchainImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		windowResizeEvent();
		return;
	}
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, UINT64_MAX));
	VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));
	VK_CHECK(vkResetCommandBuffer(get_current_frame()._mainCommandBuffer, 0));

	VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = {};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.pNext = nullptr;

	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.f;

	VkRenderPassBeginInfo rpInfo = {};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.pNext = nullptr;

	rpInfo.renderPass = _renderPass;
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = _swapChain->extent;
	rpInfo.framebuffer = _swapChain->framebuffers[swapchainImageIndex];

	rpInfo.clearValueCount = 2;

	VkClearValue clearValues[] = { clearValue, depthClear };

	rpInfo.pClearValues = &clearValues[0];

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	draw_objects(cmd, _renderables.data(), _renderables.size());

	vkCmdEndRenderPass(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));
	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &get_current_frame()._presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &get_current_frame()._renderSemaphore;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &_swapChain->swapChain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	result = vkQueuePresentKHR(_graphicsQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		windowResizeEvent();
	}

	_frameNumber++;
}

FrameData& VulkanEngine::get_current_frame()
{
	return _frames[_frameNumber % FRAME_OVERLAP];
}

void VulkanEngine::draw_objects(VkCommandBuffer cmd, RenderObject* first, int count)
{
	GPUCameraData camData;
	camData.proj = camera->pers;
	camData.view = camera->view;
	camData.viewproj = camera->pers * camera->view;

	uint32_t fullSize = pad_uniform_buffer_size(sizeof(GPUCameraData)) + pad_uniform_buffer_size(sizeof(GPUSceneData));
	char* data;
	vmaMapMemory(_allocator, _worldBuffers._allocation, (void**)&data);
	int frameIndex = _frameNumber % FRAME_OVERLAP;
	data += fullSize * frameIndex;
	memcpy(data, &camData, sizeof(GPUCameraData));
	vmaUnmapMemory(_allocator, _worldBuffers._allocation);

	float framed = (_frameNumber / 120.f);
	_sceneParameters.ambientColor = { sin(framed), 0, cos(framed), 1 };
	_sceneParameters.sunlightDirection = glm::vec4(0.0f, 1.0f, 0.0f, 0.1f);

	char* sceneData;
	vmaMapMemory(_allocator, _worldBuffers._allocation , (void**)&sceneData);
	frameIndex = _frameNumber % FRAME_OVERLAP;
	sceneData += 	fullSize * frameIndex + 
					pad_uniform_buffer_size(sizeof(GPUCameraData));
	memcpy(sceneData, &_sceneParameters, sizeof(GPUSceneData));
	vmaUnmapMemory(_allocator, _worldBuffers._allocation);

	void* objectData;
	vmaMapMemory(_allocator, get_current_frame().objectBuffer._allocation, &objectData);

	GPUObjectData* objectSSBO = (GPUObjectData*)objectData;
	
	for (int i = 0; i < count; i++)
	{
		RenderObject& object = first[i];
		objectSSBO[i].modelMatrix = object.transformMatrix;
		objectSSBO[i].objectColor = object.color;
	}

	vmaUnmapMemory(_allocator, get_current_frame().objectBuffer._allocation);

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;
	for (int i = 0; i < count; i++)
	{
		RenderObject& object = first[i];

		if (object.material != lastMaterial) {

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
			lastMaterial = object.material;

			uint32_t uniform_offset1 = fullSize * frameIndex;
			uint32_t uniform_offset2 = uniform_offset1 + pad_uniform_buffer_size(sizeof(GPUCameraData));
			uint32_t uniform_offsets[] = { uniform_offset1, uniform_offset2 };

			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 
				0, 1, &_globalDescriptor, 2, uniform_offsets);

			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout,
				1, 1, &get_current_frame().objectDescriptor, 0, nullptr);
		}

		if (object.mesh != lastMesh) {
			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->_vertexBuffer._buffer, &offset);
			lastMesh = object.mesh;
		}
		vkCmdDraw(cmd, object.mesh->_vertices.size(), 1, 0, i);
	}
}

void VulkanEngine::init_vulkan()
{
	vkb::InstanceBuilder builder;

	auto inst_ret = builder.set_app_name("AppX")
		.request_validation_layers(IS_DEBUG)
		.require_api_version(1, 1, 0);

#if IS_DEBUG
	printf("Validation layers are enabled\n\n");
	inst_ret = inst_ret.use_default_debug_messenger().set_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
#else
	printf("Validation layers are disabled\n\n");
#endif
	
	vkb::Instance vkb_inst = inst_ret.build().value();

	_instance = vkb_inst.instance;

#if IS_DEBUG
	_debug_messenger = vkb_inst.debug_messenger;
	inst_ret = inst_ret.use_default_debug_messenger();
#else
	_debug_messenger = nullptr;
#endif
	
	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);
		
	VkPhysicalDeviceFeatures features = {};
	features.fillModeNonSolid = VK_TRUE;
	features.wideLines = VK_TRUE;
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 1) // 1.1 for compatibility. Will be changed to latest later
		.set_surface(_surface)
		.set_required_features(features)
		.select()
		.value();

	// .prefer_gpu_device_type(vkb::PreferredDeviceType::integrated)

	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	
	vkb::Device vkbDevice = deviceBuilder.build().value();
	
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;

	vkGetPhysicalDeviceProperties(_chosenGPU, &_GPUProperties);
	std::cout << "Selected GPU: " << _GPUProperties.deviceName << "\n\n";
	std::cout << "The GPU has a minimum buffer alignment of " << 
	_GPUProperties.limits.minUniformBufferOffsetAlignment << "\n\n";
	
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
	
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = _chosenGPU;
	allocatorInfo.device = _device;
	allocatorInfo.instance = _instance;
	vmaCreateAllocator(&allocatorInfo, &_allocator);
}

void VulkanEngine::init_commands()
{
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, 
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	VkCommandPoolCreateInfo uploadCommandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily);
	VK_CHECK(vkCreateCommandPool(_device, &uploadCommandPoolInfo, nullptr, &_uploadContext._commandPool));

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));
	}
}

void VulkanEngine::init_default_renderpass()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = _swapChain->swapchainImageFormat;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_attachment = {};
	depth_attachment.flags = 0;
	depth_attachment.format = _swapChain->depthFormat;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };

	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 2;
	render_pass_info.pAttachments = &attachments[0];
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;

	VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass));
}

void VulkanEngine::init_sync_structures()
{
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();
	
	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_frames[i]._renderFence));

		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._presentSemaphore));
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));
	}

	VkFenceCreateInfo uploadFenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_FLAG_BITS_MAX_ENUM);
	VK_CHECK(vkCreateFence(_device, &uploadFenceCreateInfo, nullptr, &_uploadContext._uploadFence));
}

void VulkanEngine::init_descriptors()
{
	const size_t worldParamBufferSize = FRAME_OVERLAP * 
		(pad_uniform_buffer_size(sizeof(GPUCameraData)) + pad_uniform_buffer_size(sizeof(GPUSceneData)));

	_worldBuffers = create_buffer(worldParamBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
		VMA_MEMORY_USAGE_CPU_TO_GPU);

	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(_device, &pool_info, nullptr, &_descriptorPool);

	// binding for camera data at 0
	VkDescriptorSetLayoutBinding cameraBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 
		VK_SHADER_STAGE_VERTEX_BIT, 0);

	// binding for scene data at 1
	VkDescriptorSetLayoutBinding sceneBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);

	VkDescriptorSetLayoutCreateInfo setInfo = {};
	setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setInfo.pNext = nullptr;
	setInfo.bindingCount = 2;
	setInfo.flags = 0;
	VkDescriptorSetLayoutBinding bindings[] = { cameraBind, sceneBind };
	setInfo.pBindings = bindings;

	vkCreateDescriptorSetLayout(_device, &setInfo, nullptr, &_globalSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.pNext = nullptr;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &_globalSetLayout;

	vkAllocateDescriptorSets(_device, &allocInfo, &_globalDescriptor);

	VkDescriptorBufferInfo cameraInfo;
	cameraInfo.buffer = _worldBuffers._buffer;
	cameraInfo.offset = 0;
	cameraInfo.range = sizeof(GPUCameraData);

	VkDescriptorBufferInfo sceneInfo;
	sceneInfo.buffer = _worldBuffers._buffer;
	sceneInfo.offset = 0;
	sceneInfo.range = sizeof(GPUSceneData);

	VkWriteDescriptorSet cameraWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 
		_globalDescriptor, &cameraInfo, 0);
	
	VkWriteDescriptorSet sceneWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 
		_globalDescriptor, &sceneInfo, 1);

	VkWriteDescriptorSet sets[] = { cameraWrite, sceneWrite };

	vkUpdateDescriptorSets(_device, 2, sets, 0, nullptr);

	VkDescriptorSetLayoutBinding objectBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	VkDescriptorSetLayoutCreateInfo set2info = {};
	set2info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	set2info.pNext = nullptr;
	set2info.bindingCount = 1;
	set2info.flags = 0;
	set2info.pBindings = &objectBind;

	vkCreateDescriptorSetLayout(_device, &set2info, nullptr, &_objectSetLayout);

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		const int MAX_OBJECTS = 10000;
		_frames[i].objectBuffer = create_buffer((sizeof(GPUObjectData)) * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU);

		VkDescriptorSetAllocateInfo objectSetAlloc = {};
		objectSetAlloc.pNext = nullptr;
		objectSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		objectSetAlloc.descriptorPool = _descriptorPool;
		objectSetAlloc.descriptorSetCount = 1;
		objectSetAlloc.pSetLayouts = &_objectSetLayout;

		vkAllocateDescriptorSets(_device, &objectSetAlloc, &_frames[i].objectDescriptor);

		VkDescriptorBufferInfo objectBufferInfo;
		objectBufferInfo.buffer = _frames[i].objectBuffer._buffer;
		objectBufferInfo.offset = 0;
		objectBufferInfo.range = sizeof(GPUObjectData) * MAX_OBJECTS;

		VkWriteDescriptorSet objectWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
			_frames[i].objectDescriptor, &objectBufferInfo, 0);

		vkUpdateDescriptorSets(_device, 1, &objectWrite, 0, nullptr);
	}
}

size_t VulkanEngine::pad_uniform_buffer_size(size_t originalSize)
{
	// calculate required alignment based on minimum device offset alignment
	size_t minUboAlignment = _GPUProperties.limits.minUniformBufferOffsetAlignment;
	size_t alignedSize = originalSize;
	if (minUboAlignment > 0) {
		alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return alignedSize;
}

void VulkanEngine::init_pipelines() {
	PipelineBuilder pipelineBuilder;
	
	std::string fileNames[] = { std::string(BASE_DIR) + std::string("/shaders/triangle.vert.spv"), 
								std::string(BASE_DIR) + std::string("/shaders/triangle.frag.spv") };
	
	VulkanShader vulkanShader = VulkanShader(_device, fileNames, 2);
	pipelineBuilder._shaderStages = vulkanShader.shaderStages;

	VertexInputDescription vertexDescription = Vertex::get_vertex_description();
	
	pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

	pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();
	
	pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();

	pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	// build viewport and scissor from the swapchain extents
	pipelineBuilder._viewport.x = 0.0f;
	pipelineBuilder._viewport.y = 0.0f;
	pipelineBuilder._viewport.width = (float)_swapChain->extent.width;
	pipelineBuilder._viewport.height = (float)_swapChain->extent.height;
	pipelineBuilder._viewport.minDepth = 0.0f;
	pipelineBuilder._viewport.maxDepth = 1.0f;

	pipelineBuilder._scissor.offset = { 0, 0 };
	pipelineBuilder._scissor.extent = _swapChain->extent;

	pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);
	
	pipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

	pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

	pipelineBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

	VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();

	VkDescriptorSetLayout setLayouts[] = { _globalSetLayout, _objectSetLayout };
	mesh_pipeline_layout_info.setLayoutCount = 2;
	mesh_pipeline_layout_info.pSetLayouts = setLayouts;

	VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &_meshPipelineLayout));

	pipelineBuilder._pipelineLayout = _meshPipelineLayout;
	_meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

	create_material(_meshPipeline, _meshPipelineLayout, "defaultmesh");
}

void VulkanEngine::load_meshes() // Mesh handling will be done in a Mesh class in the future
{
	Mesh m;
	std::string s = std::string(BASE_DIR) + std::string("/assets/monkey_flat.obj");
	m.load_from_obj(s);

	upload_mesh(m);

	_meshes["mesh"] = m;
}

void VulkanEngine::init_scene()
{
	RenderObject monkey;
	monkey.mesh = get_mesh("mesh");
	monkey.material = get_material("defaultmesh");
	monkey.transformMatrix = glm::mat4{ 1.0f };
	monkey.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	_renderables.push_back(monkey);

	for (int x = -10; x <= 10; x++) {
		for (int y = -10; y <= 10; y++) {

			RenderObject tri;
			tri.mesh = get_mesh("mesh");
			tri.material = get_material("defaultmesh");
			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, 
				glm::vec3((getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, 
				glm::vec3(0.25, 0.25, 0.25));
			tri.transformMatrix = translation * scale;
			tri.color = glm::vec4(getRand01(), getRand01(), getRand01(), 1.0f);
			_renderables.push_back(tri);
		}
	}
}

void VulkanEngine::upload_mesh(Mesh& mesh)
{
	const size_t bufferSize = mesh._vertices.size() * sizeof(Vertex);
	// allocate staging buffer
	VkBufferCreateInfo stagingBufferInfo = {};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.pNext = nullptr;

	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	// let the VMA library know that this data should be on CPU RAM
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	AllocatedBuffer stagingBuffer;

	// allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &stagingBufferInfo, &vmaallocInfo,
		&stagingBuffer._buffer, &stagingBuffer._allocation, nullptr)
	);

	void* data;
	vmaMapMemory(_allocator, stagingBuffer._allocation, &data);

	memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(_allocator, stagingBuffer._allocation);

	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = nullptr;
	vertexBufferInfo.size = bufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	// allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &vertexBufferInfo, &vmaallocInfo,
		&mesh._vertexBuffer._buffer, &mesh._vertexBuffer._allocation, nullptr)
	);

	immediate_submit([=](VkCommandBuffer cmd) {
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = bufferSize;
		vkCmdCopyBuffer(cmd, stagingBuffer._buffer, mesh._vertexBuffer._buffer, 1, &copy);
	});

	vmaDestroyBuffer(_allocator, stagingBuffer._buffer, stagingBuffer._allocation);
}

void VulkanEngine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_uploadContext._commandPool, 1);
    VkCommandBuffer cmd;
	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &cmd));

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	function(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = vkinit::submit_info(&cmd);
	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _uploadContext._uploadFence));

	vkWaitForFences(_device, 1, &_uploadContext._uploadFence, true, 9999999999);
	vkResetFences(_device, 1, &_uploadContext._uploadFence);
	vkResetCommandPool(_device, _uploadContext._commandPool, 0);
}

AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;

	bufferInfo.size = allocSize;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;

	AllocatedBuffer newBuffer;
	
	VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo,
		&newBuffer._buffer,
		&newBuffer._allocation,
		nullptr)
	);

	return newBuffer;
}

Material* VulkanEngine::create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name)
{
	Material mat;
	mat.pipeline = pipeline;
	mat.pipelineLayout = layout;
	_materials[name] = mat;
	return &_materials[name];
}

Material* VulkanEngine::get_material(const std::string& name)
{
	auto it = _materials.find(name);
	if (it == _materials.end()) {
		return NULL;
	}
	else {
		return &(*it).second;
	}
}

Mesh* VulkanEngine::get_mesh(const std::string& name)
{
	auto it = _meshes.find(name);
	if (it == _meshes.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}