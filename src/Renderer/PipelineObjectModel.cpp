#include "Meridian/Renderer/PipelineObjectModel.h"

namespace Meridian::Renderer
{
std::vector<PsoObject> PipelineObjectModel::Build(const std::vector<RenderPassObject>& passObjects) const
{
    std::vector<PsoObject> objects;
    objects.reserve(passObjects.size());
    for (const RenderPassObject& pass : passObjects)
    {
        PsoObject object;
        object.Name = pass.Name + "_pso";
        object.Signature = pass.PsoKey + "|" + pass.CompatibilityKey;
        object.CompatibilityKey = pass.CompatibilityKey;
        objects.push_back(std::move(object));
    }
    return objects;
}
}
