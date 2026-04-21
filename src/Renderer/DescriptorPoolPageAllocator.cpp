#include "Meridian/Renderer/DescriptorPoolPageAllocator.h"

namespace Meridian::Renderer
{
DescriptorPoolPageAllocator::DescriptorPoolPageAllocator(int pageCapacity)
    : m_pageCapacity(pageCapacity)
{
}

DescriptorAllocation DescriptorPoolPageAllocator::Allocate(int descriptorCount)
{
    for (std::size_t i = 0; i < m_pages.size(); ++i)
    {
        DescriptorPage& page = m_pages[i];
        if (page.Used + descriptorCount <= page.Capacity)
        {
            const int offset = page.Used;
            page.Used += descriptorCount;
            ++m_allocationCount;
            return {static_cast<int>(i), offset, descriptorCount};
        }
    }

    m_pages.push_back({m_pageCapacity, descriptorCount});
    ++m_allocationCount;
    return {static_cast<int>(m_pages.size()) - 1, 0, descriptorCount};
}

int DescriptorPoolPageAllocator::GetPageCount() const noexcept
{
    return static_cast<int>(m_pages.size());
}

int DescriptorPoolPageAllocator::GetAllocationCount() const noexcept
{
    return m_allocationCount;
}
}
