#pragma once
#include "Meridian/Renderer/Common.h"
#include <vector>

namespace Meridian::Renderer
{
struct TransientHeapPage
{
    int Capacity = 0;
    int Used = 0;
};

class TransientHeapAllocator
{
public:
    explicit TransientHeapAllocator(int pageCapacity = 16);
    int Allocate(const AliasGroup& group);
    int GetPageCount() const noexcept;
    int GetAllocationCount() const noexcept;
    const std::vector<TransientHeapPage>& GetPages() const noexcept { return m_pages; }

private:
    int m_pageCapacity = 16;
    int m_allocationCount = 0;
    std::vector<TransientHeapPage> m_pages;
};
}
