#include "Meridian/Renderer/RenderPreparationPipeline.h"
#include "Meridian/Renderer/DescriptorPoolPageAllocator.h"
#include "Meridian/Renderer/EffectGraph.h"
#include "Meridian/Renderer/EffectToRenderPassLowerer.h"
#include "Meridian/Renderer/QueueDomainLoweringCompiler.h"
#include "Meridian/Renderer/RenderPassMergePlanner.h"
#include "Meridian/Renderer/RenderPassObjectModel.h"
#include "Meridian/Renderer/ResidencyMigrationPlanner.h"
#include "Meridian/Renderer/ResourceDependencyCompiler.h"
#include "Meridian/Renderer/TransientAliasingPlanner.h"

namespace Meridian::Renderer
{
PreparedFrame RenderPreparationPipeline::BuildPreparedFrame() const
{
    PreparedFrame frame;

    EffectGraph graph;
    frame.EffectNodes = graph.BuildDefaultGraph();

    ResourceDependencyCompiler dependencyCompiler;
    frame.ResourceEdges = dependencyCompiler.Compile(frame.EffectNodes);

    EffectToRenderPassLowerer lowerer;
    frame.LoweredPasses = lowerer.Lower(frame.EffectNodes);

    RenderPassMergePlanner merger;
    frame.MergedPasses = merger.Merge(frame.LoweredPasses);

    RenderPassObjectModel objectModel;
    frame.PassObjects = objectModel.BuildObjects(frame.MergedPasses);

    DescriptorPoolPageAllocator descriptorAllocator(8);
    for (const EffectNode& node : frame.EffectNodes)
    {
        int count = 0;
        for (const DescriptorBinding& binding : node.Bindings)
        {
            count += binding.Count;
        }
        frame.DescriptorAllocations.push_back(descriptorAllocator.Allocate(count > 0 ? count : 1));
    }

    QueueDomainLoweringCompiler lowerToQueue;
    frame.QueueBatches = lowerToQueue.Lower(frame.PassObjects);

    ResidencyMigrationPlanner residency;
    frame.ResidencyMigrations = residency.Plan(frame.QueueBatches, frame.PassObjects);

    TransientAliasingPlanner aliasing;
    frame.AliasGroups = aliasing.Plan(frame.LoweredPasses);

    return frame;
}
}
