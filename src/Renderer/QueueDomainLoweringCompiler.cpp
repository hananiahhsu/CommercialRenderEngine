#include "Meridian/Renderer/QueueDomainLoweringCompiler.h"

namespace Meridian::Renderer
{
std::vector<QueueBatch> QueueDomainLoweringCompiler::Lower(const std::vector<RenderPassObject>& passObjects) const
{
    // Use one queue batch per pass object. This keeps the CPU-side model explicit and
    // produces richer batch statistics for validation, similar to a non-merged submit list.
    std::vector<QueueBatch> batches;
    batches.reserve(passObjects.size());
    for (const RenderPassObject& pass : passObjects)
    {
        QueueBatch batch;
        batch.Domain = pass.Domain;
        batch.PassNames.push_back(pass.Name);
        batches.push_back(std::move(batch));
    }
    return batches;
}
}
