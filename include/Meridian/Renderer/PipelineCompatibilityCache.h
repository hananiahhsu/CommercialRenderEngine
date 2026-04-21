#pragma once
#include "Meridian/Renderer/Common.h"
#include <unordered_map>
namespace Meridian::Renderer
{
class PipelineCompatibilityCache
{
public:
    int InsertOrHit(const PsoObject& object);
    int GetSize() const noexcept;
    int GetHits() const noexcept;

private:
    std::unordered_map<std::string, int> m_cache;
    int m_hits = 0;
};
}
