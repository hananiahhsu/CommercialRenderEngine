#pragma once
#include "Meridian/Renderer/Common.h"
#include <unordered_map>
namespace Meridian::Renderer
{
class PsoCompatibilityGrouper
{
public:
    std::unordered_map<std::string, int> Group(const std::vector<PsoObject>& objects) const;
};
}
