#pragma once

#include <deque>
#include <string>
#include <vector>

namespace Meridian::EditorFramework
{
enum class SceneItemType
{
    Folder,
    Camera,
    Light,
    Mesh,
    IfcModel,
    Grid,
    Environment
};

struct SceneItem
{
    int Id = 0;
    int ParentId = -1;
    SceneItemType Type = SceneItemType::Mesh;
    std::string Name;
    bool Expanded = true;
    bool Visible = true;
    float PositionX = 0.0F;
    float PositionY = 0.0F;
    float PositionZ = 0.0F;
    float ExtentX = 1.0F;
    float ExtentY = 1.0F;
    float ExtentZ = 1.0F;
};

struct AssetItem
{
    std::string Name;
    std::string Type;
    std::string Source;
};

struct LogEntry
{
    std::string Channel;
    std::string Message;
};

struct PropertyRow
{
    std::string Category;
    std::string Name;
    std::string Value;
};

class StudioDocument
{
public:
    StudioDocument();

    const std::vector<SceneItem>& GetSceneItems() const;
    const std::vector<AssetItem>& GetAssets() const;
    const std::deque<LogEntry>& GetLogs() const;

    const SceneItem* GetSelectedSceneItem() const;
    int GetSelectedSceneItemId() const;
    void SetSelectedSceneItemById(int itemId);

    std::vector<PropertyRow> BuildSelectedPropertyRows() const;

    void AppendLog(std::string channel, std::string message);
    void ResetToDefaultScene();
    void AddStaticMeshActor();
    void AddCameraActor();

private:
    int AllocateId();
    const SceneItem* FindSceneItemById(int itemId) const;

    std::vector<SceneItem> m_sceneItems;
    std::vector<AssetItem> m_assets;
    std::deque<LogEntry> m_logs;
    int m_selectedSceneItemId = 0;
    int m_nextItemId = 1;
};
}
