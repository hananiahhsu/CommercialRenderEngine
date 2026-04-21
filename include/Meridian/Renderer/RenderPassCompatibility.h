#pragma once
#include "Meridian/Renderer/Common.h"
#include <unordered_map>

namespace Meridian::Renderer
{
class RenderPassCompatibility
{
public:
    std::unordered_map<std::string, int> GroupByCompatibility(const std::vector<RenderPassSpec>& passes) const;
};
}
