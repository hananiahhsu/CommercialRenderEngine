#include "Meridian/Renderer/RenderPassCompatibility.h"
#include <sstream>

namespace Meridian::Renderer
{
std::unordered_map<std::string, int> RenderPassCompatibility::GroupByCompatibility(const std::vector<RenderPassSpec>& passes) const
{
    std::unordered_map<std::string, int> groups;
    int nextId = 0;
    for (const RenderPassSpec& pass : passes)
    {
        std::ostringstream oss;
        oss << static_cast<int>(pass.Domain);
        for (const std::string& fmt : pass.AttachmentFormats)
        {
            oss << "|" << fmt;
        }
        groups.try_emplace(oss.str(), nextId++);
    }
    return groups;
}
}
