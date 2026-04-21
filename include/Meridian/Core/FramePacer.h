#pragma once
#include <cstdint>

namespace Meridian::Core
{
struct FramePacingState
{
    int FrameIndex = 0;
    int FrameSlot = 0;
    int FramesInFlight = 0;
    std::uint64_t CpuWaitFenceValue = 0;
};

class FramePacer
{
public:
    explicit FramePacer(int framesInFlight = 2);
    FramePacingState BeginFrame();
    int GetFramesInFlight() const noexcept { return m_framesInFlight; }

private:
    int m_frameIndex = 0;
    int m_framesInFlight = 2;
};
}
