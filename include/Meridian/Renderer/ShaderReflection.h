#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class ShaderReflection
{
public:
    std::vector<ShaderReflectionRecord> ReflectDefaultShaderSet() const;
};
}
