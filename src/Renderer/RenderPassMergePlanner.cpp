#include "Meridian/Renderer/RenderPassMergePlanner.h"

namespace Meridian::Renderer
{
std::vector<RenderPassSpec> RenderPassMergePlanner::Merge(const std::vector<RenderPassSpec>& passes) const
{
    if (passes.empty())
    {
        return {};
    }

    std::vector<RenderPassSpec> merged;
    merged.push_back(passes.front());
    for (std::size_t i = 1; i < passes.size(); ++i)
    {
        const RenderPassSpec& current = passes[i];
        RenderPassSpec& previous = merged.back();
        if (current.Domain == previous.Domain &&
            current.AttachmentFormats == previous.AttachmentFormats &&
            current.PsoKey == previous.PsoKey)
        {
            previous.Name += "+" + current.Name;
            previous.Reads.insert(previous.Reads.end(), current.Reads.begin(), current.Reads.end());
            previous.Writes.insert(previous.Writes.end(), current.Writes.begin(), current.Writes.end());
        }
        else
        {
            merged.push_back(current);
        }
    }
    return merged;
}
}
