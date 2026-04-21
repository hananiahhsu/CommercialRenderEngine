#pragma once
#include <string>
#include <vector>

namespace Meridian::Core
{
struct Metric
{
    std::string Name;
    int Value = 0;
};

class TelemetryFrame
{
public:
    void AddMetric(std::string name, int value);
    int GetMetric(const std::string& name) const;
    const std::vector<Metric>& GetMetrics() const noexcept { return m_metrics; }

private:
    std::vector<Metric> m_metrics;
};
}
