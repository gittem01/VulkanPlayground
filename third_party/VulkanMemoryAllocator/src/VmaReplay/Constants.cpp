//
// Copyright (c) 2017-2021 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "Common.h"
#include "Constants.h"

const int RESULT_EXCEPTION          = -1000;
const int RESULT_ERROR_COMMAND_LINE = -1;
const int RESULT_ERROR_SOURCE_FILE  = -2;
const int RESULT_ERROR_FORMAT       = -3;
const int RESULT_ERROR_VULKAN       = -4;

const char* VMA_FUNCTION_NAMES[] = {
    "vmaCreatePool",
    "vmaDestroyPool",
    "vmaSetAllocationUserData",
    "vmaCreateBuffer",
    "vmaDestroyBuffer",
    "vmaCreateImage",
    "vmaDestroyImage",
    "vmaFreeMemory",
    "vmaFreeMemoryPages",
    "vmaCreateLostAllocation",
    "vmaAllocateMemory",
    "vmaAllocateMemoryPages",
    "vmaAllocateMemoryForBuffer",
    "vmaAllocateMemoryForImage",
    "vmaMapMemory",
    "vmaUnmapMemory",
    "vmaFlushAllocation",
    "vmaInvalidateAllocation",
    "vmaTouchAllocation",
    "vmaGetAllocationInfo",
    "vmaMakePoolAllocationsLost",
    "vmaResizeAllocation",
    "vmaDefragmentationBegin",
    "vmaDefragmentationEnd",
    "vmaSetPoolName",
};
static_assert(
    _countof(VMA_FUNCTION_NAMES) == (size_t)VMA_FUNCTION::Count,
    "VMA_FUNCTION_NAMES array doesn't match VMA_FUNCTION enum.");

const char* VMA_POOL_CREATE_FLAG_NAMES[] = {
    "VMA_POOL_CREATE_IGNORE_BUFFER_IMAGE_GRANULARITY_BIT",
    "VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT",
    "VMA_POOL_CREATE_BUDDY_ALGORITHM_BIT",
};
const uint32_t VMA_POOL_CREATE_FLAG_VALUES[] = {
    VMA_POOL_CREATE_IGNORE_BUFFER_IMAGE_GRANULARITY_BIT,
    VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT,
    VMA_POOL_CREATE_BUDDY_ALGORITHM_BIT,
};
const size_t VMA_POOL_CREATE_FLAG_COUNT = _countof(VMA_POOL_CREATE_FLAG_NAMES);
static_assert(
    _countof(VMA_POOL_CREATE_FLAG_NAMES) == _countof(VMA_POOL_CREATE_FLAG_VALUES),
    "VMA_POOL_CREATE_FLAG_NAMES array doesn't match VMA_POOL_CREATE_FLAG_VALUES.");

const char* VK_BUFFER_CREATE_FLAG_NAMES[] = {
    "VK_BUFFER_CREATE_SPARSE_BINDING_BIT",
    "VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT",
    "VK_BUFFER_CREATE_SPARSE_ALIASED_BIT",
    "VK_BUFFER_CREATE_PROTECTED_BIT",
};
const uint32_t VK_BUFFER_CREATE_FLAG_VALUES[] = {
    VK_BUFFER_CREATE_SPARSE_BINDING_BIT,
    VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT,
    VK_BUFFER_CREATE_SPARSE_ALIASED_BIT,
    VK_BUFFER_CREATE_PROTECTED_BIT,
};
const size_t VK_BUFFER_CREATE_FLAG_COUNT = _countof(VK_BUFFER_CREATE_FLAG_NAMES);
static_assert(
    _countof(VK_BUFFER_CREATE_FLAG_NAMES) == _countof(VK_BUFFER_CREATE_FLAG_VALUES),
    "VK_BUFFER_CREATE_FLAG_NAMES array doesn't match VK_BUFFER_CREATE_FLAG_VALUES.");

const char* VK_BUFFER_USAGE_FLAG_NAMES[] = {
    "VK_BUFFER_USAGE_TRANSFER_SRC_BIT",
    "VK_BUFFER_USAGE_TRANSFER_DST_BIT",
    "VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT",
    "VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT",
    "VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT",
    "VK_BUFFER_USAGE_STORAGE_BUFFER_BIT",
    "VK_BUFFER_USAGE_INDEX_BUFFER_BIT",
    "VK_BUFFER_USAGE_VERTEX_BUFFER_BIT",
    "VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT",
    "VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT",
    //"VK_BUFFER_USAGE_RAYTRACING_BIT_NVX",
};
const uint32_t VK_BUFFER_USAGE_FLAG_VALUES[] = {
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
    VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
    VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT,
    //VK_BUFFER_USAGE_RAYTRACING_BIT_NVX,
};
const size_t VK_BUFFER_USAGE_FLAG_COUNT = _countof(VK_BUFFER_USAGE_FLAG_NAMES);
static_assert(
    _countof(VK_BUFFER_USAGE_FLAG_NAMES) == _countof(VK_BUFFER_USAGE_FLAG_VALUES),
    "VK_BUFFER_USAGE_FLAG_NAMES array doesn't match VK_BUFFER_USAGE_FLAG_VALUES.");

const char* VK_SHARING_MODE_NAMES[] = {
    "VK_SHARING_MODE_EXCLUSIVE",
    "VK_SHARING_MODE_CONCURRENT",
};
const size_t VK_SHARING_MODE_COUNT = _countof(VK_SHARING_MODE_NAMES);

const char* VK_IMAGE_CREATE_FLAG_NAMES[] = {
    "VK_IMAGE_CREATE_SPARSE_BINDING_BIT",
    "VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT",
    "VK_IMAGE_CREATE_SPARSE_ALIASED_BIT",
    "VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT",
    "VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT",
    "VK_IMAGE_CREATE_ALIAS_BIT",
    "VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT",
    "VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT",
    "VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT",
    "VK_IMAGE_CREATE_EXTENDED_USAGE_BIT",
    "VK_IMAGE_CREATE_PROTECTED_BIT",
    "VK_IMAGE_CREATE_DISJOINT_BIT",
    //"VK_IMAGE_CREATE_CORNER_SAMPLED_BIT_NV",
    "VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT",
};
const uint32_t VK_IMAGE_CREATE_FLAG_VALUES[] = {
    VK_IMAGE_CREATE_SPARSE_BINDING_BIT,
    VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT,
    VK_IMAGE_CREATE_SPARSE_ALIASED_BIT,
    VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
    VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
    VK_IMAGE_CREATE_ALIAS_BIT,
    VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT,
    VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT,
    VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT,
    VK_IMAGE_CREATE_EXTENDED_USAGE_BIT,
    VK_IMAGE_CREATE_PROTECTED_BIT,
    VK_IMAGE_CREATE_DISJOINT_BIT,
    //VK_IMAGE_CREATE_CORNER_SAMPLED_BIT_NV,
    VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT,
};
const size_t VK_IMAGE_CREATE_FLAG_COUNT = _countof(VK_IMAGE_CREATE_FLAG_NAMES);
static_assert(
    _countof(VK_IMAGE_CREATE_FLAG_NAMES) == _countof(VK_IMAGE_CREATE_FLAG_VALUES),
    "VK_IMAGE_CREATE_FLAG_NAMES array doesn't match VK_IMAGE_CREATE_FLAG_VALUES.");

const char* VK_IMAGE_TYPE_NAMES[] = {
    "VK_IMAGE_TYPE_1D",
    "VK_IMAGE_TYPE_2D",
    "VK_IMAGE_TYPE_3D",
};
const size_t VK_IMAGE_TYPE_COUNT = _countof(VK_IMAGE_TYPE_NAMES);

const char* VK_FORMAT_NAMES[] = {
    "VK_FORMAT_UNDEFINED",
    "VK_FORMAT_R4G4_UNORM_PACK8",
    "VK_FORMAT_R4G4B4A4_UNORM_PACK16",
    "VK_FORMAT_B4G4R4A4_UNORM_PACK16",
    "VK_FORMAT_R5G6B5_UNORM_PACK16",
    "VK_FORMAT_B5G6R5_UNORM_PACK16",
    "VK_FORMAT_R5G5B5A1_UNORM_PACK16",
    "VK_FORMAT_B5G5R5A1_UNORM_PACK16",
    "VK_FORMAT_A1R5G5B5_UNORM_PACK16",
    "VK_FORMAT_R8_UNORM",
    "VK_FORMAT_R8_SNORM",
    "VK_FORMAT_R8_USCALED",
    "VK_FORMAT_R8_SSCALED",
    "VK_FORMAT_R8_UINT",
    "VK_FORMAT_R8_SINT",
    "VK_FORMAT_R8_SRGB",
    "VK_FORMAT_R8G8_UNORM",
    "VK_FORMAT_R8G8_SNORM",
    "VK_FORMAT_R8G8_USCALED",
    "VK_FORMAT_R8G8_SSCALED",
    "VK_FORMAT_R8G8_UINT",
    "VK_FORMAT_R8G8_SINT",
    "VK_FORMAT_R8G8_SRGB",
    "VK_FORMAT_R8G8B8_UNORM",
    "VK_FORMAT_R8G8B8_SNORM",
    "VK_FORMAT_R8G8B8_USCALED",
    "VK_FORMAT_R8G8B8_SSCALED",
    "VK_FORMAT_R8G8B8_UINT",
    "VK_FORMAT_R8G8B8_SINT",
    "VK_FORMAT_R8G8B8_SRGB",
    "VK_FORMAT_B8G8R8_UNORM",
    "VK_FORMAT_B8G8R8_SNORM",
    "VK_FORMAT_B8G8R8_USCALED",
    "VK_FORMAT_B8G8R8_SSCALED",
    "VK_FORMAT_B8G8R8_UINT",
    "VK_FORMAT_B8G8R8_SINT",
    "VK_FORMAT_B8G8R8_SRGB",
    "VK_FORMAT_R8G8B8A8_UNORM",
    "VK_FORMAT_R8G8B8A8_SNORM",
    "VK_FORMAT_R8G8B8A8_USCALED",
    "VK_FORMAT_R8G8B8A8_SSCALED",
    "VK_FORMAT_R8G8B8A8_UINT",
    "VK_FORMAT_R8G8B8A8_SINT",
    "VK_FORMAT_R8G8B8A8_SRGB",
    "VK_FORMAT_B8G8R8A8_UNORM",
    "VK_FORMAT_B8G8R8A8_SNORM",
    "VK_FORMAT_B8G8R8A8_USCALED",
    "VK_FORMAT_B8G8R8A8_SSCALED",
    "VK_FORMAT_B8G8R8A8_UINT",
    "VK_FORMAT_B8G8R8A8_SINT",
    "VK_FORMAT_B8G8R8A8_SRGB",
    "VK_FORMAT_A8B8G8R8_UNORM_PACK32",
    "VK_FORMAT_A8B8G8R8_SNORM_PACK32",
    "VK_FORMAT_A8B8G8R8_USCALED_PACK32",
    "VK_FORMAT_A8B8G8R8_SSCALED_PACK32",
    "VK_FORMAT_A8B8G8R8_UINT_PACK32",
    "VK_FORMAT_A8B8G8R8_SINT_PACK32",
    "VK_FORMAT_A8B8G8R8_SRGB_PACK32",
    "VK_FORMAT_A2R10G10B10_UNORM_PACK32",
    "VK_FORMAT_A2R10G10B10_SNORM_PACK32",
    "VK_FORMAT_A2R10G10B10_USCALED_PACK32",
    "VK_FORMAT_A2R10G10B10_SSCALED_PACK32",
    "VK_FORMAT_A2R10G10B10_UINT_PACK32",
    "VK_FORMAT_A2R10G10B10_SINT_PACK32",
    "VK_FORMAT_A2B10G10R10_UNORM_PACK32",
    "VK_FORMAT_A2B10G10R10_SNORM_PACK32",
    "VK_FORMAT_A2B10G10R10_USCALED_PACK32",
    "VK_FORMAT_A2B10G10R10_SSCALED_PACK32",
    "VK_FORMAT_A2B10G10R10_UINT_PACK32",
    "VK_FORMAT_A2B10G10R10_SINT_PACK32",
    "VK_FORMAT_R16_UNORM",
    "VK_FORMAT_R16_SNORM",
    "VK_FORMAT_R16_USCALED",
    "VK_FORMAT_R16_SSCALED",
    "VK_FORMAT_R16_UINT",
    "VK_FORMAT_R16_SINT",
    "VK_FORMAT_R16_SFLOAT",
    "VK_FORMAT_R16G16_UNORM",
    "VK_FORMAT_R16G16_SNORM",
    "VK_FORMAT_R16G16_USCALED",
    "VK_FORMAT_R16G16_SSCALED",
    "VK_FORMAT_R16G16_UINT",
    "VK_FORMAT_R16G16_SINT",
    "VK_FORMAT_R16G16_SFLOAT",
    "VK_FORMAT_R16G16B16_UNORM",
    "VK_FORMAT_R16G16B16_SNORM",
    "VK_FORMAT_R16G16B16_USCALED",
    "VK_FORMAT_R16G16B16_SSCALED",
    "VK_FORMAT_R16G16B16_UINT",
    "VK_FORMAT_R16G16B16_SINT",
    "VK_FORMAT_R16G16B16_SFLOAT",
    "VK_FORMAT_R16G16B16A16_UNORM",
    "VK_FORMAT_R16G16B16A16_SNORM",
    "VK_FORMAT_R16G16B16A16_USCALED",
    "VK_FORMAT_R16G16B16A16_SSCALED",
    "VK_FORMAT_R16G16B16A16_UINT",
    "VK_FORMAT_R16G16B16A16_SINT",
    "VK_FORMAT_R16G16B16A16_SFLOAT",
    "VK_FORMAT_R32_UINT",
    "VK_FORMAT_R32_SINT",
    "VK_FORMAT_R32_SFLOAT",
    "VK_FORMAT_R32G32_UINT",
    "VK_FORMAT_R32G32_SINT",
    "VK_FORMAT_R32G32_SFLOAT",
    "VK_FORMAT_R32G32B32_UINT",
    "VK_FORMAT_R32G32B32_SINT",
    "VK_FORMAT_R32G32B32_SFLOAT",
    "VK_FORMAT_R32G32B32A32_UINT",
    "VK_FORMAT_R32G32B32A32_SINT",
    "VK_FORMAT_R32G32B32A32_SFLOAT",
    "VK_FORMAT_R64_UINT",
    "VK_FORMAT_R64_SINT",
    "VK_FORMAT_R64_SFLOAT",
    "VK_FORMAT_R64G64_UINT",
    "VK_FORMAT_R64G64_SINT",
    "VK_FORMAT_R64G64_SFLOAT",
    "VK_FORMAT_R64G64B64_UINT",
    "VK_FORMAT_R64G64B64_SINT",
    "VK_FORMAT_R64G64B64_SFLOAT",
    "VK_FORMAT_R64G64B64A64_UINT",
    "VK_FORMAT_R64G64B64A64_SINT",
    "VK_FORMAT_R64G64B64A64_SFLOAT",
    "VK_FORMAT_B10G11R11_UFLOAT_PACK32",
    "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32",
    "VK_FORMAT_D16_UNORM",
    "VK_FORMAT_X8_D24_UNORM_PACK32",
    "VK_FORMAT_D32_SFLOAT",
    "VK_FORMAT_S8_UINT",
    "VK_FORMAT_D16_UNORM_S8_UINT",
    "VK_FORMAT_D24_UNORM_S8_UINT",
    "VK_FORMAT_D32_SFLOAT_S8_UINT",
    "VK_FORMAT_BC1_RGB_UNORM_BLOCK",
    "VK_FORMAT_BC1_RGB_SRGB_BLOCK",
    "VK_FORMAT_BC1_RGBA_UNORM_BLOCK",
    "VK_FORMAT_BC1_RGBA_SRGB_BLOCK",
    "VK_FORMAT_BC2_UNORM_BLOCK",
    "VK_FORMAT_BC2_SRGB_BLOCK",
    "VK_FORMAT_BC3_UNORM_BLOCK",
    "VK_FORMAT_BC3_SRGB_BLOCK",
    "VK_FORMAT_BC4_UNORM_BLOCK",
    "VK_FORMAT_BC4_SNORM_BLOCK",
    "VK_FORMAT_BC5_UNORM_BLOCK",
    "VK_FORMAT_BC5_SNORM_BLOCK",
    "VK_FORMAT_BC6H_UFLOAT_BLOCK",
    "VK_FORMAT_BC6H_SFLOAT_BLOCK",
    "VK_FORMAT_BC7_UNORM_BLOCK",
    "VK_FORMAT_BC7_SRGB_BLOCK",
    "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK",
    "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK",
    "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK",
    "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK",
    "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK",
    "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK",
    "VK_FORMAT_EAC_R11_UNORM_BLOCK",
    "VK_FORMAT_EAC_R11_SNORM_BLOCK",
    "VK_FORMAT_EAC_R11G11_UNORM_BLOCK",
    "VK_FORMAT_EAC_R11G11_SNORM_BLOCK",
    "VK_FORMAT_ASTC_4x4_UNORM_BLOCK",
    "VK_FORMAT_ASTC_4x4_SRGB_BLOCK",
    "VK_FORMAT_ASTC_5x4_UNORM_BLOCK",
    "VK_FORMAT_ASTC_5x4_SRGB_BLOCK",
    "VK_FORMAT_ASTC_5x5_UNORM_BLOCK",
    "VK_FORMAT_ASTC_5x5_SRGB_BLOCK",
    "VK_FORMAT_ASTC_6x5_UNORM_BLOCK",
    "VK_FORMAT_ASTC_6x5_SRGB_BLOCK",
    "VK_FORMAT_ASTC_6x6_UNORM_BLOCK",
    "VK_FORMAT_ASTC_6x6_SRGB_BLOCK",
    "VK_FORMAT_ASTC_8x5_UNORM_BLOCK",
    "VK_FORMAT_ASTC_8x5_SRGB_BLOCK",
    "VK_FORMAT_ASTC_8x6_UNORM_BLOCK",
    "VK_FORMAT_ASTC_8x6_SRGB_BLOCK",
    "VK_FORMAT_ASTC_8x8_UNORM_BLOCK",
    "VK_FORMAT_ASTC_8x8_SRGB_BLOCK",
    "VK_FORMAT_ASTC_10x5_UNORM_BLOCK",
    "VK_FORMAT_ASTC_10x5_SRGB_BLOCK",
    "VK_FORMAT_ASTC_10x6_UNORM_BLOCK",
    "VK_FORMAT_ASTC_10x6_SRGB_BLOCK",
    "VK_FORMAT_ASTC_10x8_UNORM_BLOCK",
    "VK_FORMAT_ASTC_10x8_SRGB_BLOCK",
    "VK_FORMAT_ASTC_10x10_UNORM_BLOCK",
    "VK_FORMAT_ASTC_10x10_SRGB_BLOCK",
    "VK_FORMAT_ASTC_12x10_UNORM_BLOCK",
    "VK_FORMAT_ASTC_12x10_SRGB_BLOCK",
    "VK_FORMAT_ASTC_12x12_UNORM_BLOCK",
    "VK_FORMAT_ASTC_12x12_SRGB_BLOCK",
    "VK_FORMAT_G8B8G8R8_422_UNORM",
    "VK_FORMAT_B8G8R8G8_422_UNORM",
    "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM",
    "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM",
    "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM",
    "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM",
    "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM",
    "VK_FORMAT_R10X6_UNORM_PACK16",
    "VK_FORMAT_R10X6G10X6_UNORM_2PACK16",
    "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16",
    "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16",
    "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16",
    "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16",
    "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16",
    "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16",
    "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16",
    "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16",
    "VK_FORMAT_R12X4_UNORM_PACK16",
    "VK_FORMAT_R12X4G12X4_UNORM_2PACK16",
    "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16",
    "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16",
    "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16",
    "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16",
    "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16",
    "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16",
    "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16",
    "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16",
    "VK_FORMAT_G16B16G16R16_422_UNORM",
    "VK_FORMAT_B16G16R16G16_422_UNORM",
    "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM",
    "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM",
    "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM",
    "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM",
    "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM",
    "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG",
    "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG",
    "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG",
    "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG",
    "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG",
    "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG",
    "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG",
    "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG",
};
const uint32_t VK_FORMAT_VALUES[] = {
    VK_FORMAT_UNDEFINED,
    VK_FORMAT_R4G4_UNORM_PACK8,
    VK_FORMAT_R4G4B4A4_UNORM_PACK16,
    VK_FORMAT_B4G4R4A4_UNORM_PACK16,
    VK_FORMAT_R5G6B5_UNORM_PACK16,
    VK_FORMAT_B5G6R5_UNORM_PACK16,
    VK_FORMAT_R5G5B5A1_UNORM_PACK16,
    VK_FORMAT_B5G5R5A1_UNORM_PACK16,
    VK_FORMAT_A1R5G5B5_UNORM_PACK16,
    VK_FORMAT_R8_UNORM,
    VK_FORMAT_R8_SNORM,
    VK_FORMAT_R8_USCALED,
    VK_FORMAT_R8_SSCALED,
    VK_FORMAT_R8_UINT,
    VK_FORMAT_R8_SINT,
    VK_FORMAT_R8_SRGB,
    VK_FORMAT_R8G8_UNORM,
    VK_FORMAT_R8G8_SNORM,
    VK_FORMAT_R8G8_USCALED,
    VK_FORMAT_R8G8_SSCALED,
    VK_FORMAT_R8G8_UINT,
    VK_FORMAT_R8G8_SINT,
    VK_FORMAT_R8G8_SRGB,
    VK_FORMAT_R8G8B8_UNORM,
    VK_FORMAT_R8G8B8_SNORM,
    VK_FORMAT_R8G8B8_USCALED,
    VK_FORMAT_R8G8B8_SSCALED,
    VK_FORMAT_R8G8B8_UINT,
    VK_FORMAT_R8G8B8_SINT,
    VK_FORMAT_R8G8B8_SRGB,
    VK_FORMAT_B8G8R8_UNORM,
    VK_FORMAT_B8G8R8_SNORM,
    VK_FORMAT_B8G8R8_USCALED,
    VK_FORMAT_B8G8R8_SSCALED,
    VK_FORMAT_B8G8R8_UINT,
    VK_FORMAT_B8G8R8_SINT,
    VK_FORMAT_B8G8R8_SRGB,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SNORM,
    VK_FORMAT_R8G8B8A8_USCALED,
    VK_FORMAT_R8G8B8A8_SSCALED,
    VK_FORMAT_R8G8B8A8_UINT,
    VK_FORMAT_R8G8B8A8_SINT,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_B8G8R8A8_UNORM,
    VK_FORMAT_B8G8R8A8_SNORM,
    VK_FORMAT_B8G8R8A8_USCALED,
    VK_FORMAT_B8G8R8A8_SSCALED,
    VK_FORMAT_B8G8R8A8_UINT,
    VK_FORMAT_B8G8R8A8_SINT,
    VK_FORMAT_B8G8R8A8_SRGB,
    VK_FORMAT_A8B8G8R8_UNORM_PACK32,
    VK_FORMAT_A8B8G8R8_SNORM_PACK32,
    VK_FORMAT_A8B8G8R8_USCALED_PACK32,
    VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
    VK_FORMAT_A8B8G8R8_UINT_PACK32,
    VK_FORMAT_A8B8G8R8_SINT_PACK32,
    VK_FORMAT_A8B8G8R8_SRGB_PACK32,
    VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    VK_FORMAT_A2R10G10B10_SNORM_PACK32,
    VK_FORMAT_A2R10G10B10_USCALED_PACK32,
    VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
    VK_FORMAT_A2R10G10B10_UINT_PACK32,
    VK_FORMAT_A2R10G10B10_SINT_PACK32,
    VK_FORMAT_A2B10G10R10_UNORM_PACK32,
    VK_FORMAT_A2B10G10R10_SNORM_PACK32,
    VK_FORMAT_A2B10G10R10_USCALED_PACK32,
    VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
    VK_FORMAT_A2B10G10R10_UINT_PACK32,
    VK_FORMAT_A2B10G10R10_SINT_PACK32,
    VK_FORMAT_R16_UNORM,
    VK_FORMAT_R16_SNORM,
    VK_FORMAT_R16_USCALED,
    VK_FORMAT_R16_SSCALED,
    VK_FORMAT_R16_UINT,
    VK_FORMAT_R16_SINT,
    VK_FORMAT_R16_SFLOAT,
    VK_FORMAT_R16G16_UNORM,
    VK_FORMAT_R16G16_SNORM,
    VK_FORMAT_R16G16_USCALED,
    VK_FORMAT_R16G16_SSCALED,
    VK_FORMAT_R16G16_UINT,
    VK_FORMAT_R16G16_SINT,
    VK_FORMAT_R16G16_SFLOAT,
    VK_FORMAT_R16G16B16_UNORM,
    VK_FORMAT_R16G16B16_SNORM,
    VK_FORMAT_R16G16B16_USCALED,
    VK_FORMAT_R16G16B16_SSCALED,
    VK_FORMAT_R16G16B16_UINT,
    VK_FORMAT_R16G16B16_SINT,
    VK_FORMAT_R16G16B16_SFLOAT,
    VK_FORMAT_R16G16B16A16_UNORM,
    VK_FORMAT_R16G16B16A16_SNORM,
    VK_FORMAT_R16G16B16A16_USCALED,
    VK_FORMAT_R16G16B16A16_SSCALED,
    VK_FORMAT_R16G16B16A16_UINT,
    VK_FORMAT_R16G16B16A16_SINT,
    VK_FORMAT_R16G16B16A16_SFLOAT,
    VK_FORMAT_R32_UINT,
    VK_FORMAT_R32_SINT,
    VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R32G32_UINT,
    VK_FORMAT_R32G32_SINT,
    VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32B32_UINT,
    VK_FORMAT_R32G32B32_SINT,
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32A32_UINT,
    VK_FORMAT_R32G32B32A32_SINT,
    VK_FORMAT_R32G32B32A32_SFLOAT,
    VK_FORMAT_R64_UINT,
    VK_FORMAT_R64_SINT,
    VK_FORMAT_R64_SFLOAT,
    VK_FORMAT_R64G64_UINT,
    VK_FORMAT_R64G64_SINT,
    VK_FORMAT_R64G64_SFLOAT,
    VK_FORMAT_R64G64B64_UINT,
    VK_FORMAT_R64G64B64_SINT,
    VK_FORMAT_R64G64B64_SFLOAT,
    VK_FORMAT_R64G64B64A64_UINT,
    VK_FORMAT_R64G64B64A64_SINT,
    VK_FORMAT_R64G64B64A64_SFLOAT,
    VK_FORMAT_B10G11R11_UFLOAT_PACK32,
    VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
    VK_FORMAT_D16_UNORM,
    VK_FORMAT_X8_D24_UNORM_PACK32,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_BC1_RGB_UNORM_BLOCK,
    VK_FORMAT_BC1_RGB_SRGB_BLOCK,
    VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
    VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
    VK_FORMAT_BC2_UNORM_BLOCK,
    VK_FORMAT_BC2_SRGB_BLOCK,
    VK_FORMAT_BC3_UNORM_BLOCK,
    VK_FORMAT_BC3_SRGB_BLOCK,
    VK_FORMAT_BC4_UNORM_BLOCK,
    VK_FORMAT_BC4_SNORM_BLOCK,
    VK_FORMAT_BC5_UNORM_BLOCK,
    VK_FORMAT_BC5_SNORM_BLOCK,
    VK_FORMAT_BC6H_UFLOAT_BLOCK,
    VK_FORMAT_BC6H_SFLOAT_BLOCK,
    VK_FORMAT_BC7_UNORM_BLOCK,
    VK_FORMAT_BC7_SRGB_BLOCK,
    VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
    VK_FORMAT_EAC_R11_UNORM_BLOCK,
    VK_FORMAT_EAC_R11_SNORM_BLOCK,
    VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
    VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
    VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
    VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
    VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
    VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
    VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
    VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
    VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
    VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
    VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
    VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
    VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
    VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
    VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
    VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
    VK_FORMAT_G8B8G8R8_422_UNORM,
    VK_FORMAT_B8G8R8G8_422_UNORM,
    VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
    VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
    VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
    VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
    VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
    VK_FORMAT_R10X6_UNORM_PACK16,
    VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
    VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
    VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
    VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
    VK_FORMAT_R12X4_UNORM_PACK16,
    VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
    VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
    VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
    VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
    VK_FORMAT_G16B16G16R16_422_UNORM,
    VK_FORMAT_B16G16R16G16_422_UNORM,
    VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
    VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
    VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
    VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
    VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
    VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
    VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
    VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
    VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
    VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
    VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
    VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
    VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,
};
const size_t VK_FORMAT_COUNT = _countof(VK_FORMAT_NAMES);
static_assert(
    _countof(VK_FORMAT_NAMES) == _countof(VK_FORMAT_VALUES),
    "VK_FORMAT_NAMES array doesn't match VK_FORMAT_VALUES.");

const char* VK_SAMPLE_COUNT_NAMES[] = {
    "VK_SAMPLE_COUNT_1_BIT",
    "VK_SAMPLE_COUNT_2_BIT",
    "VK_SAMPLE_COUNT_4_BIT",
    "VK_SAMPLE_COUNT_8_BIT",
    "VK_SAMPLE_COUNT_16_BIT",
    "VK_SAMPLE_COUNT_32_BIT",
    "VK_SAMPLE_COUNT_64_BIT",
};
const uint32_t VK_SAMPLE_COUNT_VALUES[] = {
    VK_SAMPLE_COUNT_1_BIT,
    VK_SAMPLE_COUNT_2_BIT,
    VK_SAMPLE_COUNT_4_BIT,
    VK_SAMPLE_COUNT_8_BIT,
    VK_SAMPLE_COUNT_16_BIT,
    VK_SAMPLE_COUNT_32_BIT,
    VK_SAMPLE_COUNT_64_BIT,
};
const size_t VK_SAMPLE_COUNT_COUNT = _countof(VK_SAMPLE_COUNT_NAMES);
static_assert(
    _countof(VK_SAMPLE_COUNT_NAMES) == _countof(VK_SAMPLE_COUNT_VALUES),
    "VK_SAMPLE_COUNT_NAMES array doesn't match VK_SAMPLE_COUNT_VALUES.");

const char* VK_IMAGE_TILING_NAMES[] = {
    "VK_IMAGE_TILING_OPTIMAL",
    "VK_IMAGE_TILING_LINEAR",
};
const size_t VK_IMAGE_TILING_COUNT = _countof(VK_IMAGE_TILING_NAMES);

const char* VK_IMAGE_USAGE_FLAG_NAMES[] = {
    "VK_IMAGE_USAGE_TRANSFER_SRC_BIT",
    "VK_IMAGE_USAGE_TRANSFER_DST_BIT",
    "VK_IMAGE_USAGE_SAMPLED_BIT",
    "VK_IMAGE_USAGE_STORAGE_BIT",
    "VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT",
    "VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT",
    "VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT",
    "VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT",
    //"VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV",
};
const uint32_t VK_IMAGE_USAGE_FLAG_VALUES[] = {
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_IMAGE_USAGE_STORAGE_BIT,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
    VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
    //VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV,
};
const size_t VK_IMAGE_USAGE_FLAG_COUNT = _countof(VK_IMAGE_USAGE_FLAG_NAMES);
static_assert(
    _countof(VK_IMAGE_USAGE_FLAG_NAMES) == _countof(VK_IMAGE_USAGE_FLAG_VALUES),
    "VK_IMAGE_USAGE_FLAG_NAMES array doesn't match VK_IMAGE_USAGE_FLAG_VALUES.");

const char* VK_IMAGE_LAYOUT_NAMES[] = {
    "VK_IMAGE_LAYOUT_UNDEFINED",
    "VK_IMAGE_LAYOUT_GENERAL",
    "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL",
    "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL",
    "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL",
    "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL",
    "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL",
    "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL",
    "VK_IMAGE_LAYOUT_PREINITIALIZED",
    "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL",
    "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL",
    "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR",
    "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR",
    //"VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV",
};
const uint32_t VK_IMAGE_LAYOUT_VALUES[] = {
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_GENERAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_PREINITIALIZED,
    VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
    //VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV,
};
const size_t VK_IMAGE_LAYOUT_COUNT = _countof(VK_IMAGE_LAYOUT_NAMES);
static_assert(
    _countof(VK_IMAGE_LAYOUT_NAMES) == _countof(VK_IMAGE_LAYOUT_VALUES),
    "VK_IMAGE_LAYOUT_NAMES array doesn't match VK_IMAGE_LAYOUT_VALUES.");

const char* VMA_ALLOCATION_CREATE_FLAG_NAMES[] = {
    "VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT",
    "VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT",
    "VMA_ALLOCATION_CREATE_MAPPED_BIT",
    "VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT",
    "VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT",
    "VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT",
    "VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT",
    "VMA_ALLOCATION_CREATE_DONT_BIND_BIT",
    "VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT",
    "VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT",
    "VMA_ALLOCATION_CREATE_STRATEGY_WORST_FIT_BIT",
    "VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT",
};
const uint32_t VMA_ALLOCATION_CREATE_FLAG_VALUES[] = {
    VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
    VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT,
    VMA_ALLOCATION_CREATE_MAPPED_BIT,
    VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT,
    VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT,
    VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT,
    VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT,
    VMA_ALLOCATION_CREATE_DONT_BIND_BIT,
    VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT,
    VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT,
    VMA_ALLOCATION_CREATE_STRATEGY_WORST_FIT_BIT,
    VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT,
};
const size_t VMA_ALLOCATION_CREATE_FLAG_COUNT = _countof(VMA_ALLOCATION_CREATE_FLAG_NAMES);
static_assert(
    _countof(VMA_ALLOCATION_CREATE_FLAG_NAMES) == _countof(VMA_ALLOCATION_CREATE_FLAG_VALUES),
    "VMA_ALLOCATION_CREATE_FLAG_NAMES array doesn't match VMA_ALLOCATION_CREATE_FLAG_VALUES.");

const char* VMA_MEMORY_USAGE_NAMES[] = {
    "VMA_MEMORY_USAGE_UNKNOWN",
    "VMA_MEMORY_USAGE_GPU_ONLY",
    "VMA_MEMORY_USAGE_CPU_ONLY",
    "VMA_MEMORY_USAGE_CPU_TO_GPU",
    "VMA_MEMORY_USAGE_GPU_TO_CPU",
    "VMA_MEMORY_USAGE_CPU_COPY",
    "VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED",
};
const size_t VMA_MEMORY_USAGE_COUNT = _countof(VMA_MEMORY_USAGE_NAMES);

const char* VK_MEMORY_PROPERTY_FLAG_NAMES[] = {
    "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT",
    "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT",
    "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT",
    "VK_MEMORY_PROPERTY_HOST_CACHED_BIT",
    "VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT",
    "VK_MEMORY_PROPERTY_PROTECTED_BIT",
};
const uint32_t VK_MEMORY_PROPERTY_FLAG_VALUES[] = {
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
    VK_MEMORY_PROPERTY_PROTECTED_BIT,
};
const size_t VK_MEMORY_PROPERTY_FLAG_COUNT = _countof(VK_MEMORY_PROPERTY_FLAG_NAMES);
static_assert(
    _countof(VK_MEMORY_PROPERTY_FLAG_NAMES) == _countof(VK_MEMORY_PROPERTY_FLAG_VALUES),
    "VK_MEMORY_PROPERTY_FLAG_NAMES array doesn't match VK_MEMORY_PROPERTY_FLAG_VALUES.");
