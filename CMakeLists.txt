cmake_minimum_required (VERSION 3.8)

project (VulkanPlayground)

set (CMAKE_CXX_STANDARD 17)

if (UNIX)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
endif (UNIX)


add_subdirectory("third_party/SDL")


include_directories("third_party/SDL/include")
include_directories("third_party/stb-image")
include_directories("third_party/tiny-obj-loader")
include_directories("third_party/glm")
include_directories("third_party/vulkan/include")
include_directories("third_party/vk-bootstrap/src")
include_directories("third_party/VulkanMemoryAllocator/include")
include_directories("third_party/imgui")
include_directories("third_party/bullet3/src")

include_directories("include")


add_subdirectory("third_party/vk-bootstrap")
add_subdirectory("third_party/VulkanMemoryAllocator")
add_subdirectory("third_party/bullet3")
add_subdirectory("third_party/imgui")


link_directories("third_party/vulkan/lib")


file(GLOB SRCS src/*.cpp)
file(GLOB INCS include/*.h)
set(IMGUI_FILES		"third_party/imgui/backends/imgui_impl_vulkan.cpp"	"third_party/imgui/backends/imgui_impl_vulkan.h"
					"third_party/imgui/backends/imgui_impl_sdl.cpp"		"third_party/imgui/backends/imgui_impl_sdl.h")

add_executable(VulkanPlayground ${SRCS} ${INCS} ${IMGUI_FILES})

add_library	(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS})

target_link_libraries(VulkanPlayground SDL2main)
target_link_libraries(VulkanPlayground SDL2-static)
target_link_libraries(VulkanPlayground VulkanMemoryAllocator)
target_link_libraries(VulkanPlayground vk-bootstrap)
target_link_libraries(VulkanPlayground imgui)
target_link_libraries(VulkanPlayground  Bullet3Common BulletSoftBody BulletDynamics BulletCollision BulletInverseDynamics LinearMath)

if (UNIX)
	target_link_libraries(VulkanPlayground vulkanUnx)
elseif(WIN32)
	target_link_libraries(VulkanPlayground vulkanWin)
endif()