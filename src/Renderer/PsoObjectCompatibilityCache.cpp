#include "Meridian/Renderer/PsoObjectCompatibilityCache.h"

namespace Meridian::Renderer
{
int PsoObjectCompatibilityCache::InsertOrHit(const PsoObject& object)
{
    auto [it, inserted] = m_cache.emplace(object.CompatibilityKey + "|" + object.Name, static_cast<int>(m_cache.size()));
    if (!inserted)
    {
        ++m_hits;
    }
    return it->second;
}

int PsoObjectCompatibilityCache::GetCacheSize() const noexcept
{
    return static_cast<int>(m_cache.size());
}

int PsoObjectCompatibilityCache::GetCacheHits() const noexcept
{
    return m_hits;
}
}
