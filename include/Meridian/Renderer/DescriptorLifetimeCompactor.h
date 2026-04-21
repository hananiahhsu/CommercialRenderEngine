#pragma once
#include <vector>
namespace Meridian::Renderer
{
struct LifetimeRange
{
    int Begin = 0;
    int End = 0;
};

class DescriptorLifetimeCompactor
{
public:
    int Compact(const std::vector<LifetimeRange>& lifetimes) const;
};
}
