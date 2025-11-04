#include "VulkanGraphicResourceManager.h"
namespace vulkan {

	static VkImageUsageFlags vulkan_get_image_usage(const TextureCreation& creation) {
		const bool is_render_target = (creation._flags & TextureFlags::RenderTarget_mask) == TextureFlags::RenderTarget_mask;
		const bool is_compute_used = (creation._flags & TextureFlags::Compute_mask) == TextureFlags::Compute_mask;
		const bool is_shading_rate_texture = (creation._flags & TextureFlags::ShadingRate_mask) == TextureFlags::ShadingRate_mask;

		// Default to always readable from shader.
		VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;

		usage |= is_compute_used ? VK_IMAGE_USAGE_STORAGE_BIT : 0;

		usage |= is_shading_rate_texture ? VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR : 0;

		if (TextureFormat::has_depth_or_stencil(creation._format)) {
			// Depth/Stencil textures are normally textures you render into.
			usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // TODO

		}
		else {
			usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // TODO
			usage |= is_render_target ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;
		}

		return usage;
	}
	static void VulkanCreateTexture(VulkanGraphicResourceManager& resouce_manager, const TextureCreation& creation, TextureHandle handle, VulkanTexture* texture) {
		bool isCubemap = false;
		uint32_t layerCount = creation._array_layer_count;
		if (creation._type == TextureType::TextureCube || creation._type == TextureType::Texture_Cube_Array) {
			isCubemap = true;
		}
		const bool is_sparse_texture = (creation._flags & TextureFlags::Sparse_mask) == TextureFlags::Sparse_mask;

		texture->width = creation._width;
		texture->height = creation._height;
		texture->depth = creation._depth;
		texture->mip_base_level = 0;
		texture->array_base_layer = 0;
		texture->array_layer_count = layerCount;
		texture->mip_level_count = creation._mip_level_count;
		texture->type = creation._type;
		texture->name = creation._name;
		texture->vk_usage = vulkan_get_image_usage(creation);
		texture->vk_format = creation._format;
		texture->sampler = nullptr;
		texture->flags = creation._flags;
		texture->parent_txture = Invalid_Texture;
		texture->handle = handle;
		texture->sparse = is_sparse_texture;
		texture->alias_texture = Invalid_Texture;

		
	}
}