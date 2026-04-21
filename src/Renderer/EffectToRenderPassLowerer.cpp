#include "Meridian/Renderer/EffectToRenderPassLowerer.h"

namespace Meridian::Renderer
{
std::vector<RenderPassSpec> EffectToRenderPassLowerer::Lower(const std::vector<EffectNode>& nodes) const
{
    std::vector<RenderPassSpec> passes;
    for (const EffectNode& node : nodes)
    {
        RenderPassSpec pass;
        pass.Name = node.Name + "_pass";
        pass.Domain = node.Domain;
        pass.Reads = node.Reads;
        pass.Writes = node.Writes;
        pass.TransientCandidate = node.Writes.size() == 1 && node.Writes.front() != "backbuffer";
        for (const std::string& write : node.Writes)
        {
            pass.AttachmentFormats.push_back(write == "backbuffer" ? "rgba8" : "rgba16f");
        }
        pass.PsoKey = node.ShaderName + "_pso";
        passes.push_back(std::move(pass));
    }
    return passes;
}
}
