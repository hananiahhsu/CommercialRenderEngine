#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class TransientAliasingPlanner
{
public:
    std::vector<AliasGroup> Plan(const std::vector<RenderPassSpec>& passes) const;
};
}
