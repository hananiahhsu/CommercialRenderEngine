#pragma once
#include "Meridian/Renderer/Common.h"
#include <vector>

namespace Meridian::Renderer
{
struct DescriptorPage
{
    int Capacity = 0;
    int Used = 0;
};

class DescriptorPoolPageAllocator
{
public:
    explicit DescriptorPoolPageAllocator(int pageCapacity = 8);
    DescriptorAllocation Allocate(int descriptorCount);
    int GetPageCount() const noexcept;
    int GetAllocationCount() const noexcept;
    const std::vector<DescriptorPage>& GetPages() const noexcept { return m_pages; }

private:
    int m_pageCapacity = 8;
    int m_allocationCount = 0;
    std::vector<DescriptorPage> m_pages;
};
}
