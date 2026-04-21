#include "Meridian/Core/Telemetry.h"

namespace Meridian::Core
{
void TelemetryFrame::AddMetric(std::string name, int value)
{
    m_metrics.push_back({std::move(name), value});
}

int TelemetryFrame::GetMetric(const std::string& name) const
{
    for (const Metric& metric : m_metrics)
    {
        if (metric.Name == name)
        {
            return metric.Value;
        }
    }
    return 0;
}
}
