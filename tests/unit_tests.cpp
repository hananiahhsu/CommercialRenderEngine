#include "Meridian/Core/MainLoop.h"
#include "Meridian/Renderer/DescriptorLifetimeCompactor.h"
#include "Meridian/Renderer/DescriptorPoolPageAllocator.h"
#include "Meridian/Renderer/EffectGraph.h"
#include "Meridian/Renderer/EffectToRenderPassLowerer.h"
#include "Meridian/Renderer/PipelineCompiler.h"
#include "Meridian/Renderer/RenderPassObjectModel.h"
#include "Meridian/Renderer/RenderPassMergePlanner.h"
#include "Meridian/Renderer/ShaderReflection.h"
#include "Meridian/Renderer/TransientAliasingPlanner.h"
#include <iostream>

using namespace Meridian;

int main()
{
    // Descriptor compaction.
    Renderer::DescriptorLifetimeCompactor compactor;
    if (compactor.Compact({{0,2},{1,3},{4,5}}) != 1)
    {
        std::cerr << "Descriptor compaction mismatch\n";
        return 1;
    }

    // Page allocator.
    Renderer::DescriptorPoolPageAllocator allocator(4);
    allocator.Allocate(3);
    allocator.Allocate(2);
    if (allocator.GetPageCount() != 2)
    {
        std::cerr << "Descriptor page allocator mismatch\n";
        return 2;
    }

    // Effect lowering and merge.
    Renderer::EffectGraph graph;
    Renderer::EffectToRenderPassLowerer lowerer;
    Renderer::RenderPassMergePlanner merger;
    auto nodes = graph.BuildDefaultGraph();
    auto passes = lowerer.Lower(nodes);
    auto merged = merger.Merge(passes);
    if (passes.empty() || merged.empty())
    {
        std::cerr << "Effect lowering mismatch\n";
        return 3;
    }

    // Pipeline compiler.
    Renderer::RenderPassObjectModel objectModel;
    auto objects = objectModel.BuildObjects(merged);
    Renderer::PipelineCompiler compiler;
    auto compile = compiler.Compile(objects);
    if (compile.Objects.empty() || compile.CacheHits <= 0)
    {
        std::cerr << "Pipeline compiler mismatch\n";
        return 4;
    }

    // Aliasing planner.
    Renderer::TransientAliasingPlanner aliasing;
    auto aliasGroups = aliasing.Plan(passes);
    if (aliasGroups.empty())
    {
        std::cerr << "Aliasing planner mismatch\n";
        return 5;
    }

    // Main loop telemetry.
    Core::MainLoop mainLoop;
    auto result = mainLoop.RunOneFrame();
    if (result.Telemetry.GetMetric("Commands") <= 0)
    {
        std::cerr << "Telemetry mismatch\n";
        return 6;
    }

    std::cout << "unit_tests_passed\n";
    return 0;
}
