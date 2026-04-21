#pragma once

namespace Meridian::EditorFramework
{
struct PanelRect
{
    int X = 0;
    int Y = 0;
    int Width = 0;
    int Height = 0;

    [[nodiscard]] int Right() const { return X + Width; }
    [[nodiscard]] int Bottom() const { return Y + Height; }
};

struct LayoutVisibility
{
    bool ShowSceneOutliner = true;
    bool ShowContentBrowser = true;
    bool ShowDetails = true;
    bool ShowOutputLog = true;
};

struct StudioLayoutSnapshot
{
    PanelRect TitleBar;
    PanelRect SceneOutliner;
    PanelRect ContentBrowser;
    PanelRect Viewport;
    PanelRect Details;
    PanelRect OutputLog;
    PanelRect StatusBar;
};

class StudioLayoutEngine
{
public:
    [[nodiscard]] static StudioLayoutSnapshot Build(int clientWidth, int clientHeight, const LayoutVisibility& visibility);
};
}
