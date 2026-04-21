#pragma once
#include "Meridian/Renderer/Common.h"

namespace Meridian::Renderer
{
class PipelineObjectModel
{
public:
    std::vector<PsoObject> Build(const std::vector<RenderPassObject>& passObjects) const;
};
}
