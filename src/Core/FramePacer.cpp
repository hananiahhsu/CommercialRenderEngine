#include "Meridian/Core/FramePacer.h"

namespace Meridian::Core
{
FramePacer::FramePacer(int framesInFlight)
    : m_framesInFlight(framesInFlight)
{
}

FramePacingState FramePacer::BeginFrame()
{
    FramePacingState state;
    state.FrameIndex = m_frameIndex;
    state.FramesInFlight = m_framesInFlight;
    state.FrameSlot = m_frameIndex % m_framesInFlight;
    state.CpuWaitFenceValue = static_cast<std::uint64_t>(m_frameIndex / m_framesInFlight);
    ++m_frameIndex;
    return state;
}
}
