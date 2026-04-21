#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class EffectGraph
{
public:
    std::vector<EffectNode> BuildDefaultGraph() const;
};
}
