#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class ResidencyMigrationPlanner
{
public:
    std::vector<ResidencyMigration> Plan(const std::vector<QueueBatch>& batches,
                                         const std::vector<RenderPassObject>& objects) const;
};
}
