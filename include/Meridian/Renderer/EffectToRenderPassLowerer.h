#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class EffectToRenderPassLowerer
{
public:
    std::vector<RenderPassSpec> Lower(const std::vector<EffectNode>& nodes) const;
};
}
