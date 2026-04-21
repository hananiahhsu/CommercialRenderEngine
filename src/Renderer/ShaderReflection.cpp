#include "Meridian/Renderer/ShaderReflection.h"

namespace Meridian::Renderer
{
std::vector<ShaderReflectionRecord> ShaderReflection::ReflectDefaultShaderSet() const
{
    return {
        {"gbuffer_vs", "vertex", {{0,0,DescriptorType::UniformBuffer,1},{0,1,DescriptorType::StorageBuffer,1}}},
        {"gbuffer_fs", "fragment", {{1,0,DescriptorType::SampledImage,4},{1,1,DescriptorType::Sampler,1}}},
        {"lighting_cs", "compute", {{0,0,DescriptorType::StorageBuffer,1},{0,1,DescriptorType::SampledImage,3}}},
        {"post_bloom_cs", "compute", {{0,0,DescriptorType::SampledImage,2}}},
        {"post_tonemap_fs", "fragment", {{0,0,DescriptorType::SampledImage,1}}},
        {"shadow_vs", "vertex", {{0,0,DescriptorType::UniformBuffer,1}}},
        {"hair_cs", "compute", {{0,0,DescriptorType::StorageBuffer,2}}},
        {"water_cs", "compute", {{0,0,DescriptorType::StorageBuffer,2}}}
    };
}
}
