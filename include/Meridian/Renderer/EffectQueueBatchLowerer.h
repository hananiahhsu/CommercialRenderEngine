#pragma once
#include "Meridian/Renderer/Common.h"

namespace Meridian::Renderer
{
class EffectQueueBatchLowerer
{
public:
    std::vector<QueueBatch> Lower(const std::vector<EffectNode>& nodes) const;
};
}
