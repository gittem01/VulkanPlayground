#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "vk_engine.h"
#include <string>

#define ENABLE_VALIDATION 1

#define VK_CHECK(x) {												\
	VkResult err = x;												\
	if (err)														\
	{																\
		std::cout << "Detected Vulkan error: " << err << std::endl;	\
		abort();													\
	}																\
}

VulkanEngine::VulkanEngine(uint32_t width, uint32_t height) {
	init(width, height);

	int numOfLights = 5;
	lights.resize(numOfLights);
	srand(999);
	for (int i = 0; i < numOfLights; i++){
		lights.at(i).position = glm::vec4(rand() % 50 - 25, rand() % 10, rand() % 50 - 25, 1.0f);
		lights.at(i).strength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		int randNum = rand() % 3;
		if (randNum == 0)
			lights.at(i).color = glm::vec4(0, 0, 1, 1.0f);
		else if (randNum == 1)
			lights.at(i).color = glm::vec4(1, 0, 0, 1.0f);
		else if (randNum == 2)
			lights.at(i).color = glm::vec4(1, 1, 0, 1.0f);
	}
}

VulkanEngine::~VulkanEngine() {
	cleanup();
}

bool VulkanEngine::looper() {
	frameNumber += 1;

	int res = eventHandler();
	if (!res) return 1;	

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// game update start

	camera->update();

	// game update end

	ImDrawData* draw_data = NULL;
	draw_data = imguiLoop();

	render(draw_data);

	return false;
}

ImDrawData* VulkanEngine::imguiLoop() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("imgui window");
	camera->isAnyWindowHovered |= ImGui::IsWindowHovered();
	
	if (io->DeltaTime > 0.0f) {
		ImGui::Text("FPS: %.1f", io->Framerate);
	}
	else {
		ImGui::Text("FPS: 0.0");
	}
	ImGui::Text("Delta Time: %.3fms", io->DeltaTime * 1000);

	ImGui::Checkbox("camera keyboard movement smoothness", &camera->enableKeyPosSmth);
	ImGui::Checkbox("camera rotation smoothness", &camera->enableRotSmth);
	ImGui::Checkbox("camera wheel movement smoothness", &camera->enableWheelPosSmth);
	ImGui::Checkbox("camera zoom smoothness", &camera->enableZoomSmth);

	ImGui::PushItemWidth(150);
	for (int i = 0; i < NUM_VALUES; i++) {
		std::string buttonId = "RESET##" + std::to_string(i);
		if (ImGui::Button(buttonId.c_str())) {
			*camera->values[i] = *camera->baseValues[i];
		}
		ImGui::SameLine();
		ImGui::SliderFloat(camera->strings[i], camera->values[i], 1.0f, 100.0f, "%.1f", 1.0f);
	}
	
	ImGui::PushItemWidth(120);
	if (ImGui::BeginCombo("##combo", camera->cameraStrings[camera->cameraType])) {
		for (int n = 0; n < sizeof(camera->cameraStrings) / sizeof(camera->cameraStrings[0]); n++) {
			bool is_selected = (int)camera->cameraType == n;
			if (ImGui::Selectable(camera->cameraStrings[n], is_selected))
				camera->cameraType = (CameraTypes)n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();

		camera->isAnyWindowHovered = true;
	}

	ImGui::End();

	ImGui::Render();

	return ImGui::GetDrawData();
}

int VulkanEngine::eventHandler() {
	SDL_Event cEvent; // current event
	while (SDL_PollEvent(&cEvent)) {
		// On some cases (ex macos) vulkan may think window is fine even though
		// window is resized. This fixes that issue
		if (cEvent.type == SDL_WINDOWEVENT && cEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
			windowResizeEvent();
		}
		else if (cEvent.type == SDL_QUIT) return 0;
		ImGui_ImplSDL2_ProcessEvent(&cEvent);
	}

	return 1;
}

void VulkanEngine::init(uint32_t width, uint32_t height) {
	winExtent.width = width;
	winExtent.height = height;

	frameNumber = 0;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	window = SDL_CreateWindow(
		"Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		winExtent.width, winExtent.height,
		window_flags
	);

	int x1, y1, x2, y2;
	SDL_GetWindowSize(window, &x1, &y1);
	SDL_GL_GetDrawableSize(window, &x2, &y2);

	dpiScaling = x2 / x1;

	camera = new Camera3D(glm::vec3(-10.0f, 10.0f, 10.0f), (void*)this);
	camera->rot.x = glm::pi<float>() / 4.0f;
	camera->rot.y = glm::pi<float>() / 4.0f;

	init_vulkan();

	setSamples();

	_swapChain = new SwapChain((void*)this);

	init_default_renderpass();

	_swapChain->createFrameBuffers();

	init_commands();

	init_sync_structures();

	init_descriptors();

	init_pipelines();

	init_imgui();

	_isInitialized = true;
}

void VulkanEngine::windowResizeEvent() {

	vkDeviceWaitIdle(_device);

	vkDestroyCommandPool(_device, _uploadContext._commandPool, NULL);
	for (int i = 0; i < FRAME_OVERLAP; i++) {
		vkDestroyCommandPool(_device, _frames[i]._commandPool, NULL);
	}

	vkDestroyPipeline(_device, _defaultPipeline, NULL);
	vkDestroyPipelineLayout(_device, _defaultPipelineLayout, NULL);
	vkDestroyRenderPass(_device, _renderPass, NULL);

	for (int i=0; i<FRAME_OVERLAP; i++){
		vmaDestroyBuffer(_allocator, _frames[i].objectBuffer._buffer , _frames[i].objectBuffer._allocation);
	}

	vkDestroyDescriptorPool(_device, _descriptorPool, NULL);
	vkDestroyDescriptorSetLayout(_device, _globalSetLayout, NULL);
	vkDestroyDescriptorSetLayout(_device, _objectSetLayout, NULL);
	vkDestroyDescriptorSetLayout(_device, _singleTextureSetLayout, NULL);

	vmaDestroyBuffer(_allocator, _worldBuffers._buffer , _worldBuffers._allocation);

	_swapChain->destroy();
	_swapChain->creationLoop();

	init_commands();
	init_default_renderpass();
	_swapChain->createFrameBuffers();
	init_descriptors();
	init_pipelines();
	for (auto iter = _loadedTextures.begin(); iter != _loadedTextures.end(); iter++) {
		update_image_descriptors(&iter->second);
	}

	vkDeviceWaitIdle(_device);
}

void VulkanEngine::cleanup() {
	if (_isInitialized) {
		vkDeviceWaitIdle(_device);

		for (auto iter = _meshes.begin(); iter != _meshes.end(); iter++){
			Mesh m = iter->second;
			vmaDestroyBuffer(_allocator, m._vertexBuffer._buffer, m._vertexBuffer._allocation);
		}

		for (auto iter = _loadedTextures.begin(); iter != _loadedTextures.end(); iter++) {
			Texture t = iter->second;
			vkDestroyImageView(_device, t.imageView, NULL);
			vkDestroySampler(_device, t.sampler, NULL);
			vmaDestroyImage(_allocator, t.image._image, t.image._allocation);
		}

		vkDestroyPipeline(_device, _defaultPipeline, NULL);
		vkDestroyPipelineLayout(_device, _defaultPipelineLayout, NULL);

		for (int i=0; i<FRAME_OVERLAP; i++){
			vmaDestroyBuffer(_allocator, _frames[i].objectBuffer._buffer , _frames[i].objectBuffer._allocation);
		}

		vkDestroyDescriptorPool(_device, _imguiPool, NULL);
		vkDestroyDescriptorPool(_device, _descriptorPool, NULL);
		vkDestroyDescriptorSetLayout(_device, _globalSetLayout, NULL);
		vkDestroyDescriptorSetLayout(_device, _objectSetLayout, NULL);
		vkDestroyDescriptorSetLayout(_device, _singleTextureSetLayout, NULL);

		vmaDestroyBuffer(_allocator, _worldBuffers._buffer, _worldBuffers._allocation);

		vkDestroyFence(_device, _uploadContext._uploadFence, NULL);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyFence(_device, _frames[i]._renderFence, NULL);
			vkDestroySemaphore(_device, _frames[i]._presentSemaphore, NULL);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, NULL);
		}

		_swapChain->destroy();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		vkDestroyRenderPass(_device, _renderPass, NULL);

		vkDestroyCommandPool(_device, _uploadContext._commandPool, NULL);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyCommandPool(_device, _frames[i]._commandPool, NULL);
		}

		vmaDestroyAllocator(_allocator);

#if ENABLE_VALIDATION
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
#endif

		vkDestroySurfaceKHR(_instance, _surface, NULL);

		vkDestroyDevice(_device, NULL);
		vkDestroyInstance(_instance, NULL);

		SDL_DestroyWindow(window);
	}
}

FrameData& VulkanEngine::get_current_frame() { return _frames[frameNumber % FRAME_OVERLAP]; }


void VulkanEngine::render(ImDrawData* draw_data) {
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, UINT64_MAX));

	VkResult result;
	result = vkAcquireNextImageKHR(_device, _swapChain->swapChain, UINT64_MAX,
		get_current_frame()._presentSemaphore, NULL, &lastSwapchainImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		windowResizeEvent();
		return;
	}

	VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));
	VK_CHECK(vkResetCommandBuffer(get_current_frame()._mainCommandBuffer, 0));

	VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = {};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.pNext = NULL;

	cmdBeginInfo.pInheritanceInfo = NULL;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.f;

	VkRenderPassBeginInfo rpInfo = {};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.pNext = NULL;

	rpInfo.renderPass = _renderPass;
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = winExtent;
	rpInfo.framebuffer = _swapChain->framebuffers[lastSwapchainImageIndex];

	rpInfo.clearValueCount = 2;
	VkClearValue clearValues[] = { clearValue, depthClear };
	rpInfo.pClearValues = &clearValues[0];

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	draw_objects(cmd);

	if (draw_data) ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);

	vkCmdEndRenderPass(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));
	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = NULL;

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
	presentInfo.pNext = NULL;
	presentInfo.pSwapchains = &_swapChain->swapChain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &lastSwapchainImageIndex;

	result = vkQueuePresentKHR(_graphicsQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		windowResizeEvent();
	}
}

void VulkanEngine::draw_objects(VkCommandBuffer cmd) {
	GPUCameraData camData;
	camData.proj = camera->pers;
	camData.view = camera->view;
	camData.viewproj = camera->pers * camera->view;
	camData.cameraPos = camera->pos;

	uint32_t fullSize = pad_uniform_buffer_size(sizeof(GPUCameraData)) + pad_uniform_buffer_size(sizeof(GPUSceneData));
	char* data;
	vmaMapMemory(_allocator, _worldBuffers._allocation, (void**)&data);
	int frameIndex = frameNumber % FRAME_OVERLAP;
	data += fullSize * frameIndex;
	memcpy(data, &camData, sizeof(GPUCameraData));

	_sceneParameters.sunlightDirection = { 1.0f, 0.5f, 0.0f, 1.0f };
	_sceneParameters.sunlightColor = { 1.0f, 1.0f, 1.0f, 0.02f };
	_sceneParameters.ambientColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	_sceneParameters.fogColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	_sceneParameters.numOflights[0] = lights.size();

	data += pad_uniform_buffer_size(sizeof(GPUCameraData));
	memcpy(data, &_sceneParameters, sizeof(GPUSceneData));
	vmaUnmapMemory(_allocator, _worldBuffers._allocation);

	GPUObjectData* objectData;
	char* baseObjectData;
	vmaMapMemory(_allocator, get_current_frame().objectBuffer._allocation, (void**)&objectData);

	baseObjectData = (char*)objectData;

	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects.at(i)->reCalculateObjectMatrix();
		RenderObject* object = gameObjects.at(i)->renderObject;
		objectData[i].modelMatrix = gameObjects.at(i)->objectMatrix;
		objectData[i].rotationMatrix = gameObjects.at(i)->rotationMatrix;
		objectData[i].objectColor = object->color;
	}

	LightData* lightData = (LightData*)(baseObjectData + pad_storage_buffer_size(sizeof(GPUObjectData)) * MAX_OBJECTS);

	for (int i = 0; i < lights.size(); i++)
	{
		float angle = atan2(lights[i].position.x, lights[i].position.z);
		float length = glm::length(glm::vec2(lights[i].position.x, lights[i].position.z));
		lights[i].position.x = length * sin(angle + io->DeltaTime);
		lights[i].position.z = length * cos(angle + io->DeltaTime);
		lightData[i].position = lights[i].position;
		lightData[i].color = lights[i].color;
		lightData[i].strength = lights[i].strength;
	}

	vmaUnmapMemory(_allocator, get_current_frame().objectBuffer._allocation);

	std::string lastMesh = std::string();
	std::string lastMaterial = std::string();
	std::string lastTexture = std::string();
	for (int i = 0; i < gameObjects.size(); i++)
	{
		RenderObject* object = gameObjects.at(i)->renderObject;
		Mesh& mesh = _meshes[object->meshName];

		if (object->materialName != lastMaterial || lastTexture != object->textureName) {
			Material& material = _materials[object->materialName];
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
			lastMaterial = object->materialName;
			lastTexture = object->textureName;

			uint32_t uniform_offset1 = fullSize * frameIndex;
			uint32_t uniform_offset2 = uniform_offset1 + pad_uniform_buffer_size(sizeof(GPUCameraData));
			uint32_t uniform_offsets[] = { uniform_offset1, uniform_offset2 };

			uint32_t obj_uniform_offset1 = 0;
			uint32_t obj_uniform_offset2 = pad_storage_buffer_size(sizeof(GPUObjectData)) * MAX_OBJECTS;
			uint32_t obj_uniform_offsets[] = { obj_uniform_offset1, obj_uniform_offset2 };

			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout,
				0, 1, &_globalDescriptor,								2, uniform_offsets);

			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout,
				1, 1, &get_current_frame().objectDescriptor,			2, obj_uniform_offsets);

			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout,
				2, 1, &_loadedTextures[object->textureName].textureSet,	0, NULL);
		}

		if (object->meshName != lastMesh) {
			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(cmd, 0, 1, &mesh._vertexBuffer._buffer, &offset);
			lastMesh = object->meshName;
		}
		vkCmdDraw(cmd, mesh._vertices.size(), 1, 0, i);
	}
}

void VulkanEngine::init_vulkan() {
	vkb::InstanceBuilder builder;

	auto inst_ret = builder
		.set_app_name("AppX")
		.request_validation_layers(ENABLE_VALIDATION)
#ifdef __APPLE__
		.enable_extension("VK_MVK_macos_surface")
#endif
		.require_api_version(1, 2, 0);

#if ENABLE_VALIDATION
	printf("Validation layers are enabled\n\n");
	inst_ret = inst_ret.use_default_debug_messenger();
#else
	printf("Validation layers are disabled\n\n");
#endif

	vkb::Instance vkb_inst = inst_ret.build().value();

	_instance = vkb_inst.instance;

#if ENABLE_VALIDATION
	_debug_messenger = vkb_inst.debug_messenger;
	inst_ret = inst_ret.use_default_debug_messenger();
#else
	_debug_messenger = NULL;
#endif
	
	SDL_Vulkan_CreateSurface(window, _instance, &_surface);

	VkPhysicalDeviceFeatures features = {};
#ifndef __APPLE__
	features.wideLines = VK_TRUE;
	features.fillModeNonSolid = VK_TRUE;
#endif
	VkPhysicalDeviceVulkan11Features features11 = {};
	features11.shaderDrawParameters = VK_TRUE;

	vkb::PhysicalDeviceSelector selector = vkb::PhysicalDeviceSelector(vkb_inst);
	selector = selector.set_minimum_version(1, 0)
		.set_required_features(features)
		.set_required_features_11(features11);
	
	selector = selector.set_surface(_surface);

	vkb::PhysicalDevice physicalDevice = selector.select().value();

	// .prefer_gpu_device_type(vkb::PreferredDeviceType::integrated)

	vkb::DeviceBuilder deviceBuilder = vkb::DeviceBuilder(physicalDevice);

	vkb::Device vkbDevice = deviceBuilder.build().value();

	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;

	vkGetPhysicalDeviceProperties(_chosenGPU, &_GPUProperties);
	std::cout << "Selected GPU: " << _GPUProperties.deviceName << "\n\n";

	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = _chosenGPU;
	allocatorInfo.device = _device;
	allocatorInfo.instance = _instance;
	vmaCreateAllocator(&allocatorInfo, &_allocator);
}

void VulkanEngine::init_commands() {
	VkCommandPoolCreateInfo uploadCommandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily);
	VK_CHECK(vkCreateCommandPool(_device, &uploadCommandPoolInfo, NULL, &_uploadContext._commandPool));

	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, NULL, &_frames[i]._commandPool));

		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));
	}
}

void VulkanEngine::init_default_renderpass() {
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = _swapChain->swapchainImageFormat;
	color_attachment.samples = samples;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	if (samples > VK_SAMPLE_COUNT_1_BIT) {
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	else {
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_attachment = {};
	depth_attachment.flags = 0;
	depth_attachment.format = _swapChain->depthFormat;
	depth_attachment.samples = samples;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// may not need to be created but still easier to read this way
	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = _swapChain->swapchainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_resolve_ref{};
	color_attachment_resolve_ref.attachment = 2;
	color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;
	if (samples > VK_SAMPLE_COUNT_1_BIT) subpass.pResolveAttachments = &color_attachment_resolve_ref;

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	std::vector<VkAttachmentDescription> attachments;
	if (samples > VK_SAMPLE_COUNT_1_BIT) attachments = { color_attachment, depth_attachment, colorAttachmentResolve };
	else attachments = { color_attachment, depth_attachment };

	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = attachments.size();
	render_pass_info.pAttachments = attachments.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;

	VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, NULL, &_renderPass));
}

void VulkanEngine::init_sync_structures() {
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, NULL, &_frames[i]._renderFence));

		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, NULL, &_frames[i]._presentSemaphore));
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, NULL, &_frames[i]._renderSemaphore));
	}

	VkFenceCreateInfo uploadFenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_FLAG_BITS_MAX_ENUM);
	VK_CHECK(vkCreateFence(_device, &uploadFenceCreateInfo, NULL, &_uploadContext._uploadFence));
}

void VulkanEngine::init_descriptors() {
	const size_t worldParamBufferSize = FRAME_OVERLAP *
		(pad_uniform_buffer_size(sizeof(GPUCameraData)) + pad_uniform_buffer_size(sizeof(GPUSceneData)));

	_worldBuffers = create_buffer(worldParamBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VMA_MEMORY_USAGE_CPU_TO_GPU);

	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 10 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 },
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(_device, &pool_info, NULL, &_descriptorPool);

	// binding for camera data at 0
	VkDescriptorSetLayoutBinding cameraBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		VK_SHADER_STAGE_VERTEX_BIT, 0);

	// binding for scene data at 1
	VkDescriptorSetLayoutBinding sceneBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);

	VkDescriptorSetLayoutCreateInfo set1Info = {};
	set1Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	set1Info.pNext = NULL;
	set1Info.bindingCount = 2;
	set1Info.flags = 0;
	VkDescriptorSetLayoutBinding bindings[] = { cameraBind, sceneBind };
	set1Info.pBindings = bindings;

	vkCreateDescriptorSetLayout(_device, &set1Info, NULL, &_globalSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.pNext = NULL;
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
		_globalDescriptor, &sceneInfo,	1);

	VkWriteDescriptorSet sets[] = { cameraWrite, sceneWrite };

	vkUpdateDescriptorSets(_device, 2, sets, 0, NULL);

	{ // set 2
		VkDescriptorSetLayoutBinding objectBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0);

		VkDescriptorSetLayoutBinding lightBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 
			VK_SHADER_STAGE_FRAGMENT_BIT, 1);

		VkDescriptorSetLayoutBinding binds[2] = {objectBind, lightBind};

		VkDescriptorSetLayoutCreateInfo set2info = {};
		set2info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		set2info.flags = 0;
		set2info.bindingCount = 2;
		set2info.pBindings = binds;
		vkCreateDescriptorSetLayout(_device, &set2info, NULL, &_objectSetLayout);
	}

	{ // set 3
		VkDescriptorSetLayoutBinding textureBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_FRAGMENT_BIT, 0);

		VkDescriptorSetLayoutCreateInfo set3info = {};
		set3info.bindingCount = 1;
		set3info.flags = 0;
		set3info.pNext = NULL;
		set3info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		set3info.pBindings = &textureBind;
		vkCreateDescriptorSetLayout(_device, &set3info, NULL, &_singleTextureSetLayout);
	}

	for (int i = 0; i < FRAME_OVERLAP; i++){
		_frames[i].objectBuffer = create_buffer(pad_storage_buffer_size(sizeof(GPUObjectData)) * MAX_OBJECTS +
												pad_storage_buffer_size(sizeof(LightData)) * MAX_LIGHTS,
												VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
												VMA_MEMORY_USAGE_CPU_TO_GPU);

		VkDescriptorSetAllocateInfo objectSetAlloc = {};
		objectSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		objectSetAlloc.descriptorPool = _descriptorPool;
		objectSetAlloc.descriptorSetCount = 1;
		objectSetAlloc.pSetLayouts = &_objectSetLayout;

		vkAllocateDescriptorSets(_device, &objectSetAlloc, &_frames[i].objectDescriptor);

		VkDescriptorBufferInfo objectBufferInfo;
		objectBufferInfo.buffer = _frames[i].objectBuffer._buffer;
		objectBufferInfo.offset = 0;
		objectBufferInfo.range = sizeof(GPUObjectData) * MAX_OBJECTS;

		VkDescriptorBufferInfo lightBufferInfo;
		lightBufferInfo.buffer = _frames[i].objectBuffer._buffer;
		lightBufferInfo.offset = 0;
		lightBufferInfo.range = sizeof(LightData) * MAX_LIGHTS;

		VkWriteDescriptorSet objectWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
			_frames[i].objectDescriptor, &objectBufferInfo, 0);

		VkWriteDescriptorSet lightWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
			_frames[i].objectDescriptor, &lightBufferInfo, 1);

		VkWriteDescriptorSet sets[] = { objectWrite, lightWrite };

		vkUpdateDescriptorSets(_device, 2, sets, 0, NULL);
	}
}

size_t VulkanEngine::pad_uniform_buffer_size(size_t originalSize) {
	// calculate required alignment based on minimum device offset alignment
	size_t minUboAlignment = _GPUProperties.limits.minUniformBufferOffsetAlignment;
	size_t alignedSize = originalSize;
	if (minUboAlignment > 0) {
		alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return alignedSize;
}

size_t VulkanEngine::pad_storage_buffer_size(size_t originalSize) {
	// calculate required alignment based on minimum device offset alignment
	size_t minStorageAlignment = _GPUProperties.limits.minStorageBufferOffsetAlignment;
	size_t alignedSize = originalSize;
	if (minStorageAlignment > 0) {
		alignedSize = (alignedSize + minStorageAlignment - 1) & ~(minStorageAlignment - 1);
	}
	return alignedSize;
}

void VulkanEngine::init_pipelines() {

	PipelineBuilder pipelineBuilder;

	std::vector<std::string> fileNames =	{	
												std::string("../../shaders/default.vert.spv"),
												std::string("../../shaders/default.frag.spv")
											};

	VulkanShader vulkanShader = VulkanShader(_device, fileNames);
	pipelineBuilder._shaderStages = vulkanShader.shaderStages;

	pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

	VertexInputDescription vertexDescription = Vertex::get_vertex_description();
	VkPipelineVertexInputStateCreateInfo info;

	pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();
	pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();

	pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();
	pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();

	pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	// build viewport and scissor from the swapchain extents
	pipelineBuilder._viewport.x = 0.0f;
	pipelineBuilder._viewport.y = 0.0f;
	pipelineBuilder._viewport.width = (float)winExtent.width;
	pipelineBuilder._viewport.height = (float)winExtent.height;
	pipelineBuilder._viewport.minDepth = 0.0f;
	pipelineBuilder._viewport.maxDepth = 1.0f;

	pipelineBuilder._scissor.offset = { 0, 0 };
	pipelineBuilder._scissor.extent = winExtent;

	pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

	pipelineBuilder._multisampling = vkinit::multisampling_state_create_info(samples);

	pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

	pipelineBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
	
	VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();
	
	VkDescriptorSetLayout setLayouts[] = { _globalSetLayout, _objectSetLayout, _singleTextureSetLayout };
	mesh_pipeline_layout_info.setLayoutCount = sizeof(setLayouts) / sizeof(setLayouts[0]);
	mesh_pipeline_layout_info.pSetLayouts = setLayouts;

	VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, NULL, &_defaultPipelineLayout));
	
	pipelineBuilder._pipelineLayout = _defaultPipelineLayout;
	_defaultPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);
	
	create_material(_defaultPipeline, _defaultPipelineLayout, "defaultMaterial");
}

void VulkanEngine::get_mesh(std::string meshPath, const char* meshName) {
	Mesh m;

	std::string s = std::string(meshPath);
	m.load_from_obj(s);

	upload_mesh(m);

	_meshes[meshName] = m;
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

void VulkanEngine::get_image(std::string imagePath, const char* imageName, VkFilter filter) {
	Texture tex; // texture handling will be done in a separate class in the future

	vkutil::load_image(this, imagePath, tex.image);

	VkImageViewCreateInfo imageinfo = vkinit::imageview_create_info(VK_FORMAT_R8G8B8A8_SRGB, tex.image._image, VK_IMAGE_ASPECT_COLOR_BIT);
	vkCreateImageView(_device, &imageinfo, NULL, &tex.imageView);

	VkSamplerCreateInfo samplerInfo = vkinit::sampler_create_info(filter);
	vkCreateSampler(_device, &samplerInfo, NULL, &tex.sampler);

	update_image_descriptors(&tex);

	_loadedTextures[imageName] = tex;
}

void VulkanEngine::update_image_descriptors(Texture* tex) {
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.pNext = NULL;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &_singleTextureSetLayout;

	vkAllocateDescriptorSets(_device, &allocInfo, &tex->textureSet);

	VkDescriptorImageInfo imageBufferInfo;
	imageBufferInfo.imageView = tex->imageView;
	imageBufferInfo.sampler = tex->sampler;
	imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet texture = vkinit::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		tex->textureSet, &imageBufferInfo, 0);

	vkUpdateDescriptorSets(_device, 1, &texture, 0, NULL);
}

AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = NULL;

	bufferInfo.size = allocSize;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;

	AllocatedBuffer newBuffer;
	VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo, &newBuffer._buffer, &newBuffer._allocation, NULL));

	return newBuffer;
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

void VulkanEngine::init_imgui() {
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &_imguiPool));

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForVulkan(window);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _instance;
	init_info.PhysicalDevice = _chosenGPU;
	init_info.Device = _device;
	init_info.Queue = _graphicsQueue;
	init_info.QueueFamily = _graphicsQueueFamily;
	init_info.DescriptorPool = _imguiPool;
	init_info.MinImageCount = _swapChain->swapchainImages.size();
	init_info.ImageCount = _swapChain->swapchainImages.size();
	init_info.MSAASamples = samples;
	ImGui_ImplVulkan_Init(&init_info, _renderPass);

	VkCommandBuffer commandBuffer = beginOneTimeSubmit();
	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
	endOneTimeSubmit(commandBuffer);
	
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	io = &ImGui::GetIO();

	ImGuiStyle* style = &ImGui::GetStyle();
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0.95);
}

Material* VulkanEngine::create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name) {
	Material mat;
	mat.pipeline = pipeline;
	mat.pipelineLayout = layout;
	_materials[name] = mat;
	return &_materials[name];
}

Material* VulkanEngine::get_material(const std::string& name) {
	auto it = _materials.find(name);
	if (it == _materials.end()) {
		return NULL;
	}
	else {
		return &(*it).second;
	}
}

Mesh* VulkanEngine::get_mesh(const std::string& name) {
	auto it = _meshes.find(name);
	if (it == _meshes.end()) {
		return NULL;
	}
	else {
		return &(*it).second;
	}
}

void VulkanEngine::setSamples() {
	VkSampleCountFlags counts = _GPUProperties.limits.framebufferColorSampleCounts & _GPUProperties.limits.framebufferDepthSampleCounts;

	for (uint32_t flags = desiredSamples; flags >= VK_SAMPLE_COUNT_1_BIT; flags >>= 1) {
		if (flags & counts) {
			samples = (VkSampleCountFlagBits)flags;
			break;
		}
	}

	uint32_t maxSampleCount = pow(2, (uint32_t)log2(counts));
	printf("Desired sample count: %d\nApplied sample count: %d\nMax sample count: %d\n\n", desiredSamples, samples, maxSampleCount);
}
