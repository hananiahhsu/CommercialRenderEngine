#include "Meridian/Renderer/PsoCompatibilityGrouper.h"

namespace Meridian::Renderer
{
std::unordered_map<std::string, int> PsoCompatibilityGrouper::Group(const std::vector<PsoObject>& objects) const
{
    std::unordered_map<std::string, int> groups;
    int nextId = 0;
    for (const PsoObject& object : objects)
    {
        if (!groups.contains(object.CompatibilityKey))
        {
            groups[object.CompatibilityKey] = nextId++;
        }
    }
    return groups;
}
}
