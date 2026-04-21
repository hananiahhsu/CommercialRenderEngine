#pragma once
#include "Meridian/Renderer/Common.h"
#include <unordered_map>
#include <vector>

namespace Meridian::Renderer
{
struct ResidencyHistoryRecord
{
    std::string ResourceName;
    std::vector<QueueDomain> Queues;
};

class ResourceResidencyHistory
{
public:
    void Observe(const std::vector<ResidencyMigration>& migrations);
    int GetTrackedResourceCount() const noexcept;
    int GetTotalTransitions() const noexcept;

private:
    std::unordered_map<std::string, ResidencyHistoryRecord> m_history;
    int m_totalTransitions = 0;
};
}
