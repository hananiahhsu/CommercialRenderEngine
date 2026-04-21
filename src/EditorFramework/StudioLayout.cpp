#include "Meridian/EditorFramework/StudioLayout.h"

#include <algorithm>

namespace Meridian::EditorFramework
{
StudioLayoutSnapshot StudioLayoutEngine::Build(int clientWidth, int clientHeight, const LayoutVisibility& visibility)
{
    StudioLayoutSnapshot snapshot;

    const int clampedWidth = std::max(clientWidth, 960);
    const int clampedHeight = std::max(clientHeight, 640);

    constexpr int titleBarHeight = 40;
    constexpr int statusBarHeight = 24;
    constexpr int leftWidth = 300;
    constexpr int rightWidth = 320;
    constexpr int outputHeight = 180;
    constexpr int gutter = 6;

    snapshot.TitleBar = {0, 0, clampedWidth, titleBarHeight};
    snapshot.StatusBar = {0, clampedHeight - statusBarHeight, clampedWidth, statusBarHeight};

    int workspaceTop = titleBarHeight;
    int workspaceBottom = clampedHeight - statusBarHeight;

    if (visibility.ShowOutputLog)
    {
        snapshot.OutputLog = {0, workspaceBottom - outputHeight, clampedWidth, outputHeight};
        workspaceBottom = snapshot.OutputLog.Y;
    }
    else
    {
        snapshot.OutputLog = {0, clampedHeight, 0, 0};
    }

    const int leftPanelWidth = visibility.ShowSceneOutliner || visibility.ShowContentBrowser ? leftWidth : 0;
    const int rightPanelWidth = visibility.ShowDetails ? rightWidth : 0;

    const int viewportX = leftPanelWidth > 0 ? leftPanelWidth + gutter : 0;
    const int viewportWidth = std::max(clampedWidth - viewportX - rightPanelWidth - (rightPanelWidth > 0 ? gutter : 0), 320);
    const int viewportHeight = std::max(workspaceBottom - workspaceTop, 240);

    if (leftPanelWidth > 0)
    {
        const int sceneHeight = visibility.ShowSceneOutliner && visibility.ShowContentBrowser ? (viewportHeight * 3) / 5 : viewportHeight;
        snapshot.SceneOutliner = visibility.ShowSceneOutliner ? PanelRect{0, workspaceTop, leftWidth, sceneHeight} : PanelRect{};
        snapshot.ContentBrowser = visibility.ShowContentBrowser ? PanelRect{0, workspaceTop + sceneHeight, leftWidth, viewportHeight - sceneHeight} : PanelRect{};
    }
    else
    {
        snapshot.SceneOutliner = {};
        snapshot.ContentBrowser = {};
    }

    snapshot.Viewport = {viewportX, workspaceTop, viewportWidth, viewportHeight};
    snapshot.Details = visibility.ShowDetails ? PanelRect{snapshot.Viewport.Right() + gutter, workspaceTop, rightWidth, viewportHeight} : PanelRect{};
    return snapshot;
}
}
