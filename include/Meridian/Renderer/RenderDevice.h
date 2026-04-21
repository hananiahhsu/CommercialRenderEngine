#pragma once
#include "Meridian/Renderer/Common.h"
#include <vector>

namespace Meridian::Renderer
{
class RenderDevice
{
public:
    ExecuteStats Execute(const std::vector<QueueBatch>& batches) const;
};
}
