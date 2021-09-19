cmake_minimum_required (VERSION 3.8)

project (VulkanPlayground)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)

add_subdirectory ("third_party/SDL")
include_directories("third_party/SDL/include")


include_directories("third_party/stb-image")
include_directories("third_party/tiny-obj-loader")
include_directories("third_party/glm")
include_directories("third_party/vulkan/include")
include_directories("third_party/vk-bootstrap/src")
include_directories("third_party/VulkanMemoryAllocator/include")
include_directories("src")

add_subdirectory("third_party/vk-bootstrap")
add_subdirectory("third_party/VulkanMemoryAllocator")

link_directories("third_party/vulkan/lib")

add_executable	(VulkanPlayground "src/main.cpp"
								"src/vk_engine.cpp" "src/vk_engine.h"
								"src/vk_initializers.cpp" "src/vk_initializers.h"
								"src/vk_mesh.h" "src/vk_mesh.cpp"
								"src/vk_types.h"
								"src/vk_shader.h" "src/vk_shader.cpp"
								"src/Camera3D.h" "src/Camera3D.cpp"
								"src/WindowHandler.h" "src/WindowHandler.cpp"
								"src/vk_swapchain.h" "src/vk_swapchain.cpp"
								"src/vk_pipelinebuilder.h"
								"src/vk_utils.h" "src/vk_utils.cpp")

target_link_libraries(VulkanPlayground SDL2main)
target_link_libraries(VulkanPlayground SDL2-static)
target_link_libraries(VulkanPlayground VulkanMemoryAllocator)
target_link_libraries(VulkanPlayground vk-bootstrap)


if (UNIX)
	target_link_libraries(VulkanPlayground vulkanUnx)
elseif(WIN32)
	target_link_libraries(VulkanPlayground vulkanWin)
endif()