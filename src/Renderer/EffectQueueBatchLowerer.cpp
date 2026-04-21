#include "Meridian/Renderer/EffectQueueBatchLowerer.h"

namespace Meridian::Renderer
{
std::vector<QueueBatch> EffectQueueBatchLowerer::Lower(const std::vector<EffectNode>& nodes) const
{
    std::vector<QueueBatch> batches;
    QueueBatch current;
    bool hasCurrent = false;
    for (const EffectNode& node : nodes)
    {
        if (!hasCurrent || current.Domain != node.Domain)
        {
            if (hasCurrent)
            {
                batches.push_back(current);
            }
            current = {};
            current.Domain = node.Domain;
            hasCurrent = true;
        }
        current.PassNames.push_back(node.Name + "_effect_batch");
    }
    if (hasCurrent)
    {
        batches.push_back(current);
    }
    return batches;
}
}
