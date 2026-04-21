#pragma once
#include "Meridian/Renderer/Common.h"
#include "Meridian/Renderer/PipelineCompatibilityCache.h"
#include "Meridian/Renderer/PsoObjectCompatibilityCache.h"

namespace Meridian::Renderer
{
struct PipelineCompileResult
{
    std::vector<PsoObject> Objects;
    int CompatibilityGroups = 0;
    int CacheSize = 0;
    int CacheHits = 0;
    int ObjectCacheSize = 0;
    int ObjectCacheHits = 0;
};

class PipelineCompiler
{
public:
    PipelineCompileResult Compile(const std::vector<RenderPassObject>& passObjects) const;
};
}
