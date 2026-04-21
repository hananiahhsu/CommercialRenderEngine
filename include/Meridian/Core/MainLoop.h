#pragma once
#include "Meridian/Core/FramePacer.h"
#include "Meridian/Core/Telemetry.h"

namespace Meridian::Core
{
struct MainLoopResult
{
    FramePacingState Pacing;
    int Visible = 0;
    int Draws = 0;
    int EffectNodes = 0;
    int ResourceDependencyEdges = 0;
    int LoweredRenderPasses = 0;
    int MergedRenderPasses = 0;
    int RenderPassObjects = 0;
    int CompatibilityGroups = 0;
    int QueueBatches = 0;
    int ResidencyMigrations = 0;
    int DescriptorPoolPages = 0;
    int DescriptorPoolAllocations = 0;
    int DescriptorCompacted = 0;
    int PipelineObjects = 0;
    int PsoCompatibilityGroups = 0;
    int PipelineCacheSize = 0;
    int PipelineCacheHits = 0;
    int BytecodeEntries = 0;
    int PersistedBytecodeEntries = 0;
    int DiskIndexRecords = 0;
    int AliasGroups = 0;
    int TransientHeapPages = 0;
    int TransientHeapAllocations = 0;
    int AcquireOps = 0;
    int PresentOps = 0;
    int QueueSubmitLists = 0;
    int Commands = 0;
    TelemetryFrame Telemetry;
};

class MainLoop
{
public:
    MainLoop();
    MainLoopResult RunOneFrame();

private:
    FramePacer m_framePacer;
};
}
