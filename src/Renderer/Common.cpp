#include "Meridian/Renderer/Common.h"

namespace Meridian::Renderer
{
std::string DescriptorBinding::Signature() const
{
    return std::to_string(Set) + ":" + std::to_string(Binding) + ":" +
           std::to_string(static_cast<int>(Type)) + ":" + std::to_string(Count);
}
}
