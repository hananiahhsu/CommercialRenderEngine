#include "Meridian/Renderer/ResourceDependencyCompiler.h"
#include <unordered_map>

namespace Meridian::Renderer
{
std::vector<ResourceEdge> ResourceDependencyCompiler::Compile(const std::vector<EffectNode>& nodes) const
{
    std::unordered_map<std::string, std::string> lastWriter;
    std::vector<ResourceEdge> edges;
    for (const EffectNode& node : nodes)
    {
        for (const std::string& input : node.Reads)
        {
            auto it = lastWriter.find(input);
            if (it != lastWriter.end())
            {
                edges.push_back({it->second, node.Name, input});
            }
        }
        for (const std::string& output : node.Writes)
        {
            lastWriter[output] = node.Name;
        }
    }
    return edges;
}
}
