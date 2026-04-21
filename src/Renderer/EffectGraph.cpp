#include "Meridian/Renderer/EffectGraph.h"

namespace Meridian::Renderer
{
std::vector<EffectNode> EffectGraph::BuildDefaultGraph() const
{
    return {
        {"shadow", QueueDomain::Graphics, "shadow_vs", {}, {"shadowMap"}, {{0,0,DescriptorType::UniformBuffer,1}}},
        {"gbuffer", QueueDomain::Graphics, "gbuffer_fs", {"shadowMap"}, {"gbufferA","gbufferB","depth"}, {{1,0,DescriptorType::SampledImage,4}}},
        {"hair", QueueDomain::Compute, "hair_cs", {"depth"}, {"hairBuffer"}, {{0,0,DescriptorType::StorageBuffer,2}}},
        {"water", QueueDomain::Compute, "water_cs", {"depth"}, {"waterBuffer"}, {{0,0,DescriptorType::StorageBuffer,2}}},
        {"lighting", QueueDomain::Compute, "lighting_cs", {"gbufferA","gbufferB","depth","shadowMap","hairBuffer","waterBuffer"}, {"litScene"}, {{0,0,DescriptorType::StorageBuffer,1}}},
        {"bloom", QueueDomain::Compute, "post_bloom_cs", {"litScene"}, {"bloomScene"}, {{0,0,DescriptorType::SampledImage,2}}},
        {"tonemap", QueueDomain::Graphics, "post_tonemap_fs", {"bloomScene"}, {"backbuffer"}, {{0,0,DescriptorType::SampledImage,1}}}
    };
}
}
