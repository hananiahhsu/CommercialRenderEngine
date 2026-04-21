#include "Meridian/Core/MainLoop.h"
#include <iostream>

int main()
{
    Meridian::Core::MainLoop loop;
    Meridian::Core::MainLoopResult result = loop.RunOneFrame();
    std::cout
        << "Frame=" << result.Pacing.FrameIndex
        << " Slot=" << result.Pacing.FrameSlot
        << " InFlight=" << result.Pacing.FramesInFlight
        << " Visible=" << result.Visible
        << " Draws=" << result.Draws
        << " EffectNodes=" << result.EffectNodes
        << " ResourceDependencyEdges=" << result.ResourceDependencyEdges
        << " LoweredRenderPasses=" << result.LoweredRenderPasses
        << " MergedRenderPasses=" << result.MergedRenderPasses
        << " RenderPassObjects=" << result.RenderPassObjects
        << " CompatibilityGroups=" << result.CompatibilityGroups
        << " QueueBatches=" << result.QueueBatches
        << " ResidencyMigrations=" << result.ResidencyMigrations
        << " DescriptorPoolPages=" << result.DescriptorPoolPages
        << " DescriptorPoolAllocations=" << result.DescriptorPoolAllocations
        << " DescriptorCompacted=" << result.DescriptorCompacted
        << " PipelineObjects=" << result.PipelineObjects
        << " PsoCompatibilityGroups=" << result.PsoCompatibilityGroups
        << " PipelineCacheSize=" << result.PipelineCacheSize
        << " PipelineCacheHits=" << result.PipelineCacheHits
        << " BytecodeEntries=" << result.BytecodeEntries
        << " PersistedBytecodeEntries=" << result.PersistedBytecodeEntries
        << " DiskIndexRecords=" << result.DiskIndexRecords
        << " AliasGroups=" << result.AliasGroups
        << " TransientHeapPages=" << result.TransientHeapPages
        << " TransientHeapAllocations=" << result.TransientHeapAllocations
        << " AcquireOps=" << result.AcquireOps
        << " PresentOps=" << result.PresentOps
        << " QueueSubmitLists=" << result.QueueSubmitLists
        << " Commands=" << result.Commands
        << "\n";
    return 0;
}
