#pragma once
#include "Meridian/Renderer/Common.h"

namespace Meridian::Renderer
{
class RenderPreparationPipeline
{
public:
    PreparedFrame BuildPreparedFrame() const;
};
}
