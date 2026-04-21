#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>

namespace Meridian::Renderer
{
enum class QueueDomain
{
    Graphics = 0,
    Compute = 1,
    Transfer = 2,
    Present = 3
};

enum class DescriptorType
{
    UniformBuffer,
    StorageBuffer,
    SampledImage,
    Sampler
};

struct DescriptorBinding
{
    int Set = 0;
    int Binding = 0;
    DescriptorType Type = DescriptorType::UniformBuffer;
    int Count = 1;

    std::string Signature() const;
};

struct DescriptorAllocation
{
    int PageIndex = 0;
    int Offset = 0;
    int Count = 0;
};

struct ShaderReflectionRecord
{
    std::string ShaderName;
    std::string Stage;
    std::vector<DescriptorBinding> Bindings;
};

struct BytecodeBlob
{
    std::string ShaderName;
    std::string Hash;
    std::vector<std::uint8_t> Bytes;
};

struct EffectNode
{
    std::string Name;
    QueueDomain Domain = QueueDomain::Graphics;
    std::string ShaderName;
    std::vector<std::string> Reads;
    std::vector<std::string> Writes;
    std::vector<DescriptorBinding> Bindings;
};

struct ResourceEdge
{
    std::string FromNode;
    std::string ToNode;
    std::string ResourceName;
};

struct RenderPassSpec
{
    std::string Name;
    QueueDomain Domain = QueueDomain::Graphics;
    std::vector<std::string> Reads;
    std::vector<std::string> Writes;
    std::vector<std::string> AttachmentFormats;
    bool TransientCandidate = false;
    std::string PsoKey;
};

struct RenderPassObject
{
    std::string Name;
    QueueDomain Domain = QueueDomain::Graphics;
    std::vector<std::string> AttachmentFormats;
    std::string PsoKey;
    std::string CompatibilityKey;
};

struct PsoObject
{
    std::string Name;
    std::string Signature;
    std::string CompatibilityKey;
};

struct QueueBatch
{
    QueueDomain Domain = QueueDomain::Graphics;
    std::vector<std::string> PassNames;
};

struct ResidencyMigration
{
    std::string ResourceName;
    QueueDomain From = QueueDomain::Graphics;
    QueueDomain To = QueueDomain::Graphics;
};

struct AliasGroup
{
    int GroupId = 0;
    std::vector<std::string> Resources;
};

struct PreparedFrame
{
    std::vector<EffectNode> EffectNodes;
    std::vector<ResourceEdge> ResourceEdges;
    std::vector<RenderPassSpec> LoweredPasses;
    std::vector<RenderPassSpec> MergedPasses;
    std::vector<RenderPassObject> PassObjects;
    std::vector<DescriptorAllocation> DescriptorAllocations;
    std::vector<QueueBatch> QueueBatches;
    std::vector<ResidencyMigration> ResidencyMigrations;
    std::vector<AliasGroup> AliasGroups;
};

struct ExecuteStats
{
    int AcquireOps = 0;
    int PresentOps = 0;
    int QueueSubmitLists = 0;
    int Commands = 0;
};

inline constexpr const char* ToString(QueueDomain domain) noexcept
{
    switch (domain)
    {
    case QueueDomain::Graphics: return "graphics";
    case QueueDomain::Compute: return "compute";
    case QueueDomain::Transfer: return "transfer";
    case QueueDomain::Present: return "present";
    }
    return "unknown";
}
}
