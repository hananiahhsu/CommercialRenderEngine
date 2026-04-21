#include "Meridian/EditorFramework/StudioDocument.h"

#include <algorithm>
#include <sstream>

namespace Meridian::EditorFramework
{
namespace
{
std::string ToString(SceneItemType type)
{
    switch (type)
    {
    case SceneItemType::Folder:
        return "Folder";
    case SceneItemType::Camera:
        return "Camera";
    case SceneItemType::Light:
        return "Light";
    case SceneItemType::Mesh:
        return "StaticMesh";
    case SceneItemType::IfcModel:
        return "IfcModel";
    case SceneItemType::Grid:
        return "Grid";
    case SceneItemType::Environment:
        return "Environment";
    default:
        return "Unknown";
    }
}
}

StudioDocument::StudioDocument()
{
    ResetToDefaultScene();
}

const std::vector<SceneItem>& StudioDocument::GetSceneItems() const
{
    return m_sceneItems;
}

const std::vector<AssetItem>& StudioDocument::GetAssets() const
{
    return m_assets;
}

const std::deque<LogEntry>& StudioDocument::GetLogs() const
{
    return m_logs;
}

const SceneItem* StudioDocument::GetSelectedSceneItem() const
{
    return FindSceneItemById(m_selectedSceneItemId);
}

int StudioDocument::GetSelectedSceneItemId() const
{
    return m_selectedSceneItemId;
}

void StudioDocument::SetSelectedSceneItemById(int itemId)
{
    if (FindSceneItemById(itemId) == nullptr)
    {
        return;
    }

    m_selectedSceneItemId = itemId;
    const SceneItem* selected = GetSelectedSceneItem();
    if (selected != nullptr)
    {
        AppendLog("Selection", "Selected '" + selected->Name + "'.");
    }
}

std::vector<PropertyRow> StudioDocument::BuildSelectedPropertyRows() const
{
    std::vector<PropertyRow> rows;
    const SceneItem* selected = GetSelectedSceneItem();
    if (selected == nullptr)
    {
        rows.push_back({"Selection", "Object", "None"});
        return rows;
    }

    rows.push_back({"Identity", "Name", selected->Name});
    rows.push_back({"Identity", "Type", ToString(selected->Type)});
    rows.push_back({"Identity", "Visible", selected->Visible ? "True" : "False"});

    std::ostringstream position;
    position << selected->PositionX << ", " << selected->PositionY << ", " << selected->PositionZ;
    rows.push_back({"Transform", "Position", position.str()});

    std::ostringstream extent;
    extent << selected->ExtentX << " x " << selected->ExtentY << " x " << selected->ExtentZ;
    rows.push_back({"Bounds", "Extent", extent.str()});

    rows.push_back({"Render", "Mobility", selected->Type == SceneItemType::Camera ? "EditorOnly" : "Static"});
    rows.push_back({"Render", "Material Slot Count", selected->Type == SceneItemType::IfcModel ? "14" : "1"});
    rows.push_back({"Render", "Shadow Casting", selected->Type == SceneItemType::Light ? "N/A" : "Enabled"});
    rows.push_back({"Metadata", "Source", selected->Type == SceneItemType::IfcModel ? "Sample/office.ifc" : "SceneAuthored"});
    return rows;
}

void StudioDocument::AppendLog(std::string channel, std::string message)
{
    if (m_logs.size() >= 14)
    {
        m_logs.pop_front();
    }

    m_logs.push_back({std::move(channel), std::move(message)});
}

void StudioDocument::ResetToDefaultScene()
{
    m_sceneItems.clear();
    m_assets.clear();
    m_logs.clear();
    m_nextItemId = 1;

    const int worldId = AllocateId();
    m_sceneItems.push_back({worldId, -1, SceneItemType::Folder, "World", true, true, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::Grid, "EditorGrid", true, true, 0.0F, 0.0F, 0.0F, 20.0F, 0.0F, 20.0F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::Camera, "PerspectiveCamera", true, true, -4.0F, 4.0F, -6.0F, 0.5F, 0.5F, 0.5F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::Light, "DirectionalLight", true, true, 8.0F, 12.0F, -4.0F, 0.5F, 0.5F, 0.5F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::Environment, "SkyAtmosphere", true, true, 0.0F, 0.0F, 0.0F, 30.0F, 30.0F, 30.0F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::Mesh, "VehicleBody", true, true, -2.5F, 0.8F, 1.0F, 1.6F, 0.8F, 3.0F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::Mesh, "PipeRack_A", true, true, 3.5F, 1.2F, 0.0F, 1.4F, 1.2F, 2.2F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::IfcModel, "GlassTower_IFC", true, true, 0.0F, 4.0F, 8.0F, 3.0F, 8.0F, 3.0F});
    m_sceneItems.push_back({AllocateId(), worldId, SceneItemType::Mesh, "ManipulatorArm", true, true, 5.5F, 1.1F, -3.5F, 0.8F, 1.4F, 0.8F});

    m_assets.push_back({"office.ifc", "IFC", "Samples/IFC/office.ifc"});
    m_assets.push_back({"robot.step", "STEP", "Samples/CAD/robot.step"});
    m_assets.push_back({"carpaint.material", "Material", "Samples/Materials/carpaint.material"});
    m_assets.push_back({"industrial_lighting.hdr", "HDRI", "Samples/Textures/industrial_lighting.hdr"});
    m_assets.push_back({"vehicle_body.mesh", "Mesh", "DerivedData/vehicle_body.mesh"});

    m_selectedSceneItemId = worldId;
    AppendLog("Boot", "MeridianStudio editor shell initialized.");
    AppendLog("Layout", "Docked workspace prepared: Outliner | Viewport | Details | Output Log.");
    AppendLog("Scene", "Loaded default editor scene with IFC, CAD mesh and lighting actors.");
}

void StudioDocument::AddStaticMeshActor()
{
    SceneItem item;
    item.Id = AllocateId();
    item.ParentId = 1;
    item.Type = SceneItemType::Mesh;
    item.Name = "StaticMeshActor_" + std::to_string(item.Id);
    item.PositionX = -6.0F + static_cast<float>(item.Id % 5) * 2.5F;
    item.PositionY = 0.7F;
    item.PositionZ = -4.0F + static_cast<float>(item.Id % 4) * 2.0F;
    item.ExtentX = 0.9F;
    item.ExtentY = 0.7F;
    item.ExtentZ = 0.9F;
    m_sceneItems.push_back(item);
    m_selectedSceneItemId = item.Id;
    AppendLog("Create", "Added new static mesh actor '" + item.Name + "'.");
}

void StudioDocument::AddCameraActor()
{
    SceneItem item;
    item.Id = AllocateId();
    item.ParentId = 1;
    item.Type = SceneItemType::Camera;
    item.Name = "CameraActor_" + std::to_string(item.Id);
    item.PositionX = -2.0F + static_cast<float>(item.Id % 3) * 1.5F;
    item.PositionY = 2.5F;
    item.PositionZ = -7.0F + static_cast<float>(item.Id % 2) * 2.0F;
    item.ExtentX = 0.4F;
    item.ExtentY = 0.4F;
    item.ExtentZ = 0.8F;
    m_sceneItems.push_back(item);
    m_selectedSceneItemId = item.Id;
    AppendLog("Create", "Added new editor camera '" + item.Name + "'.");
}

int StudioDocument::AllocateId()
{
    return m_nextItemId++;
}

const SceneItem* StudioDocument::FindSceneItemById(int itemId) const
{
    auto iter = std::find_if(
        m_sceneItems.begin(),
        m_sceneItems.end(),
        [itemId](const SceneItem& item)
        {
            return item.Id == itemId;
        });
    return iter != m_sceneItems.end() ? &(*iter) : nullptr;
}
}
