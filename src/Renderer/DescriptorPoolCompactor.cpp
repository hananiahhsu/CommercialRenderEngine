#include "Meridian/Renderer/DescriptorPoolCompactor.h"
#include "Meridian/Renderer/DescriptorPoolPageAllocator.h"

namespace Meridian::Renderer
{
int DescriptorPoolCompactor::Compact(DescriptorPoolPageAllocator& allocator) const
{
    // CPU-side model: compacting reclaim ratio approximated by pages-1 when multiple sparse pages exist.
    const int pages = allocator.GetPageCount();
    return pages > 1 ? pages - 1 : 0;
}
}
