#include "Meridian/Renderer/RenderDevice.h"

namespace Meridian::Renderer
{
ExecuteStats RenderDevice::Execute(const std::vector<QueueBatch>& batches) const
{
    ExecuteStats stats;
    stats.AcquireOps = 1;
    stats.PresentOps = 1;
    stats.QueueSubmitLists = static_cast<int>(batches.size());
    stats.Commands = 2; // acquire + present
    for (const QueueBatch& batch : batches)
    {
        stats.Commands += static_cast<int>(batch.PassNames.size()) + 1; // begin queue + one per pass
        (void)batch;
    }
    return stats;
}
}
