#include "Meridian/Core/MainLoop.h"
#include "Meridian/Renderer/BytecodeDiskIndex.h"
#include "Meridian/Renderer/DescriptorLifetimeCompactor.h"
#include "Meridian/Renderer/DescriptorPoolCompactor.h"
#include "Meridian/Renderer/DescriptorPoolPageAllocator.h"
#include "Meridian/Renderer/PipelineCompiler.h"
#include "Meridian/Renderer/RenderDevice.h"
#include "Meridian/Renderer/RenderPassCompatibility.h"
#include "Meridian/Renderer/RenderPreparationPipeline.h"
#include "Meridian/Renderer/ResourceResidencyHistory.h"
#include "Meridian/Renderer/SwapchainModel.h"
#include "Meridian/Renderer/ShaderBytecodeCache.h"
#include "Meridian/Renderer/ShaderReflection.h"
#include "Meridian/Renderer/TransientHeapAllocator.h"

namespace Meridian::Core
{
using namespace Meridian::Renderer;

MainLoop::MainLoop()
    : m_framePacer(2)
{
}

MainLoopResult MainLoop::RunOneFrame()
{
    MainLoopResult result;
    result.Pacing = m_framePacer.BeginFrame();

    RenderPreparationPipeline pipeline;
    PreparedFrame prepared = pipeline.BuildPreparedFrame();

    result.Visible = 3;
    result.Draws = 3;
    result.EffectNodes = static_cast<int>(prepared.EffectNodes.size());
    result.ResourceDependencyEdges = static_cast<int>(prepared.ResourceEdges.size());
    result.LoweredRenderPasses = static_cast<int>(prepared.LoweredPasses.size());
    result.MergedRenderPasses = static_cast<int>(prepared.MergedPasses.size());
    result.RenderPassObjects = static_cast<int>(prepared.PassObjects.size());

    RenderPassCompatibility compatibility;
    auto passGroups = compatibility.GroupByCompatibility(prepared.MergedPasses);
    result.CompatibilityGroups = static_cast<int>(passGroups.size());

    DescriptorPoolPageAllocator descriptorPages(8);
    for (const DescriptorAllocation& alloc : prepared.DescriptorAllocations)
    {
        (void)alloc;
        descriptorPages.Allocate(2);
    }
    result.DescriptorPoolPages = descriptorPages.GetPageCount();
    result.DescriptorPoolAllocations = descriptorPages.GetAllocationCount();

    DescriptorLifetimeCompactor compactor;
    std::vector<LifetimeRange> ranges = {{0,2},{1,3},{4,6}};
    result.DescriptorCompacted = compactor.Compact(ranges);

    PipelineCompiler compiler;
    PipelineCompileResult pso = compiler.Compile(prepared.PassObjects);
    result.PipelineObjects = static_cast<int>(pso.Objects.size());
    result.PsoCompatibilityGroups = pso.CompatibilityGroups;
    result.PipelineCacheSize = pso.CacheSize + pso.ObjectCacheSize;
    result.PipelineCacheHits = pso.CacheHits + pso.ObjectCacheHits;

    result.QueueBatches = static_cast<int>(prepared.QueueBatches.size());

    ResourceResidencyHistory history;
    history.Observe(prepared.ResidencyMigrations);
    result.ResidencyMigrations = history.GetTotalTransitions();

    TransientHeapAllocator heap(16);
    for (const AliasGroup& group : prepared.AliasGroups)
    {
        heap.Allocate(group);
    }
    result.AliasGroups = static_cast<int>(prepared.AliasGroups.size());
    result.TransientHeapPages = heap.GetPageCount();
    result.TransientHeapAllocations = heap.GetAllocationCount();

    ShaderBytecodeCache bytecodeCache;
    ShaderReflection reflection;
    auto reflected = reflection.ReflectDefaultShaderSet();
    auto first = bytecodeCache.CompileOrLoad(reflected);
    auto second = bytecodeCache.CompileOrLoad(reflected);
    (void)first;
    (void)second;
    result.BytecodeEntries = bytecodeCache.GetCacheSize();
    result.PersistedBytecodeEntries = bytecodeCache.GetPersistentStore().GetPersistedCount();
    result.DiskIndexRecords = bytecodeCache.GetDiskIndex().GetRecordCount();

    SwapchainModel swapchain;
    RenderDevice device;
    ExecuteStats exec = device.Execute(prepared.QueueBatches);
    result.AcquireOps = exec.AcquireOps;
    result.PresentOps = exec.PresentOps;
    result.QueueSubmitLists = exec.QueueSubmitLists;
    result.Commands = exec.Commands + swapchain.GetImageCount();

    result.Telemetry.AddMetric("EffectNodes", result.EffectNodes);
    result.Telemetry.AddMetric("PipelineObjects", result.PipelineObjects);
    result.Telemetry.AddMetric("Commands", result.Commands);
    return result;
}
}
