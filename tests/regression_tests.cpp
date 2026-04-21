#include "Meridian/Core/MainLoop.h"
#include <iostream>

using namespace Meridian::Core;

int main()
{
    MainLoop loop;
    MainLoopResult result = loop.RunOneFrame();

    if (result.EffectNodes < 7) return 1;
    if (result.ResourceDependencyEdges < 4) return 2;
    if (result.RenderPassObjects < 5) return 3;
    if (result.PipelineObjects < 4) return 4;
    if (result.BytecodeEntries < 4) return 5;
    if (result.QueueSubmitLists < 4) return 6;
    if (result.Commands < 8) return 7;

    std::cout
        << "Visible=" << result.Visible
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
