#pragma once

namespace Meridian::Renderer
{
class DescriptorPoolPageAllocator;

class DescriptorPoolCompactor
{
public:
    int Compact(DescriptorPoolPageAllocator& allocator) const;
};
}
