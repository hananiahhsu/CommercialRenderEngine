#include "Meridian/Renderer/TransientAliasingPlanner.h"

namespace Meridian::Renderer
{
std::vector<AliasGroup> TransientAliasingPlanner::Plan(const std::vector<RenderPassSpec>& passes) const
{
    std::vector<AliasGroup> groups;
    int nextId = 0;
    for (const RenderPassSpec& pass : passes)
    {
        if (!pass.TransientCandidate || pass.Writes.empty())
        {
            continue;
        }
        // Simple CPU-side model: pair transient outputs into alias groups of size 2.
        if (groups.empty() || groups.back().Resources.size() >= 2)
        {
            groups.push_back({nextId++, {}});
        }
        groups.back().Resources.push_back(pass.Writes.front());
    }
    return groups;
}
}
