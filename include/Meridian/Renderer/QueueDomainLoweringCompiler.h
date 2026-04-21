#pragma once
#include "Meridian/Renderer/Common.h"
namespace Meridian::Renderer
{
class QueueDomainLoweringCompiler
{
public:
    std::vector<QueueBatch> Lower(const std::vector<RenderPassObject>& passObjects) const;
};
}
