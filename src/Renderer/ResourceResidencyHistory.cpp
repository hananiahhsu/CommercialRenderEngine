#include "Meridian/Renderer/ResourceResidencyHistory.h"

namespace Meridian::Renderer
{
void ResourceResidencyHistory::Observe(const std::vector<ResidencyMigration>& migrations)
{
    for (const ResidencyMigration& migration : migrations)
    {
        ResidencyHistoryRecord& record = m_history[migration.ResourceName];
        record.ResourceName = migration.ResourceName;
        record.Queues.push_back(migration.From);
        record.Queues.push_back(migration.To);
        ++m_totalTransitions;
    }
}

int ResourceResidencyHistory::GetTrackedResourceCount() const noexcept
{
    return static_cast<int>(m_history.size());
}

int ResourceResidencyHistory::GetTotalTransitions() const noexcept
{
    return m_totalTransitions;
}
}
