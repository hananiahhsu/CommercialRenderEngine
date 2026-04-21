#include "Meridian/Renderer/DescriptorLifetimeCompactor.h"
#include <algorithm>
#include <vector>

namespace Meridian::Renderer
{
int DescriptorLifetimeCompactor::Compact(const std::vector<LifetimeRange>& lifetimes) const
{
    if (lifetimes.empty())
    {
        return 0;
    }

    std::vector<LifetimeRange> sorted = lifetimes;
    std::sort(sorted.begin(), sorted.end(), [](const LifetimeRange& a, const LifetimeRange& b)
    {
        return a.Begin < b.Begin;
    });

    int compacted = 0;
    int currentEnd = sorted.front().End;
    for (std::size_t i = 1; i < sorted.size(); ++i)
    {
        if (sorted[i].Begin <= currentEnd)
        {
            ++compacted;
            currentEnd = std::max(currentEnd, sorted[i].End);
        }
        else
        {
            currentEnd = sorted[i].End;
        }
    }
    return compacted;
}
}
