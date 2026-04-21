#include "Meridian/Renderer/ResidencyMigrationPlanner.h"
#include <unordered_map>

namespace Meridian::Renderer
{
std::vector<ResidencyMigration> ResidencyMigrationPlanner::Plan(const std::vector<QueueBatch>& batches,
                                                               const std::vector<RenderPassObject>& objects) const
{
    std::unordered_map<std::string, QueueDomain> resourceQueue;
    std::unordered_map<std::string, RenderPassObject> byName;
    for (const RenderPassObject& object : objects)
    {
        byName.emplace(object.Name, object);
    }

    std::vector<ResidencyMigration> migrations;
    for (const QueueBatch& batch : batches)
    {
        for (const std::string& passName : batch.PassNames)
        {
            auto it = byName.find(passName);
            if (it == byName.end())
            {
                continue;
            }
            const std::string resourceName = it->second.Name;
            auto rq = resourceQueue.find(resourceName);
            if (rq == resourceQueue.end())
            {
                resourceQueue.emplace(resourceName, batch.Domain);
            }
            else if (rq->second != batch.Domain)
            {
                migrations.push_back({resourceName, rq->second, batch.Domain});
                rq->second = batch.Domain;
            }
        }
    }
    return migrations;
}
}
