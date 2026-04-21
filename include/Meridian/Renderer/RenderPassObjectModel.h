#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class RenderPassObjectModel
{
public:
    std::vector<RenderPassObject> BuildObjects(const std::vector<RenderPassSpec>& passes) const;
};
}
