#include "Meridian/Renderer/RenderPassObjectModel.h"
#include <sstream>

namespace Meridian::Renderer
{
std::vector<RenderPassObject> RenderPassObjectModel::BuildObjects(const std::vector<RenderPassSpec>& passes) const
{
    std::vector<RenderPassObject> objects;
    for (const RenderPassSpec& pass : passes)
    {
        RenderPassObject object;
        object.Name = pass.Name;
        object.Domain = pass.Domain;
        object.AttachmentFormats = pass.AttachmentFormats;
        object.PsoKey = pass.PsoKey;
        std::ostringstream compat;
        compat << static_cast<int>(pass.Domain);
        for (const std::string& fmt : pass.AttachmentFormats)
        {
            compat << "|" << fmt;
        }
        object.CompatibilityKey = compat.str();
        objects.push_back(std::move(object));
    }
    return objects;
}
}
