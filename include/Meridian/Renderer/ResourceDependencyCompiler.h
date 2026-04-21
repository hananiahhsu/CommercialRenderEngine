#pragma once
#include "Meridian/Renderer/Common.h"

namespace Meridian::Renderer
{
class ResourceDependencyCompiler
{
public:
    std::vector<ResourceEdge> Compile(const std::vector<EffectNode>& nodes) const;
};
}
