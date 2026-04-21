#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class RenderPassMergePlanner
{
public:
    std::vector<RenderPassSpec> Merge(const std::vector<RenderPassSpec>& passes) const;
};
}
