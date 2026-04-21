#include "Meridian/Renderer/PipelineCompatibilityCache.h"

namespace Meridian::Renderer
{
int PipelineCompatibilityCache::InsertOrHit(const PsoObject& object)
{
    auto [it, inserted] = m_cache.emplace(object.Signature, static_cast<int>(m_cache.size()));
    if (!inserted)
    {
        ++m_hits;
    }
    return it->second;
}

int PipelineCompatibilityCache::GetSize() const noexcept
{
    return static_cast<int>(m_cache.size());
}

int PipelineCompatibilityCache::GetHits() const noexcept
{
    return m_hits;
}
}
