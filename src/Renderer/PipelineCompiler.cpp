#include "Meridian/Renderer/PipelineCompiler.h"
#include "Meridian/Renderer/PipelineObjectModel.h"
#include "Meridian/Renderer/PsoCompatibilityGrouper.h"

namespace Meridian::Renderer
{
PipelineCompileResult PipelineCompiler::Compile(const std::vector<RenderPassObject>& passObjects) const
{
    PipelineObjectModel builder;
    PsoCompatibilityGrouper grouper;
    PipelineCompatibilityCache cache;
    PsoObjectCompatibilityCache objectCache;

    PipelineCompileResult result;
    result.Objects = builder.Build(passObjects);
    auto groups = grouper.Group(result.Objects);
    result.CompatibilityGroups = static_cast<int>(groups.size());

    for (const PsoObject& object : result.Objects)
    {
        cache.InsertOrHit(object);
        objectCache.InsertOrHit(object);
        // hit paths
        cache.InsertOrHit(object);
        objectCache.InsertOrHit(object);
    }

    result.CacheSize = cache.GetSize();
    result.CacheHits = cache.GetHits();
    result.ObjectCacheSize = objectCache.GetCacheSize();
    result.ObjectCacheHits = objectCache.GetCacheHits();
    return result;
}
}
