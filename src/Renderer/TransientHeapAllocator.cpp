#include "Meridian/Renderer/TransientHeapAllocator.h"

namespace Meridian::Renderer
{
TransientHeapAllocator::TransientHeapAllocator(int pageCapacity)
    : m_pageCapacity(pageCapacity)
{
}

int TransientHeapAllocator::Allocate(const AliasGroup& group)
{
    const int demand = static_cast<int>(group.Resources.size()) * 2 + 1;
    for (TransientHeapPage& page : m_pages)
    {
        if (page.Used + demand <= page.Capacity)
        {
            page.Used += demand;
            ++m_allocationCount;
            return static_cast<int>(&page - &m_pages[0]);
        }
    }

    m_pages.push_back({m_pageCapacity, demand});
    ++m_allocationCount;
    return static_cast<int>(m_pages.size()) - 1;
}

int TransientHeapAllocator::GetPageCount() const noexcept
{
    return static_cast<int>(m_pages.size());
}

int TransientHeapAllocator::GetAllocationCount() const noexcept
{
    return m_allocationCount;
}
}
