#include "Meridian/EditorFramework/MainFrameWindow.h"

#include "Meridian/Core/MainLoop.h"
#include "Meridian/EditorFramework/StudioCommandIds.h"
#include "Meridian/EditorFramework/StudioDocument.h"
#include "Meridian/EditorFramework/StudioLayout.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#endif

namespace Meridian::EditorFramework
{
namespace
{
struct CameraState
{
    float OrbitYawDegrees = 34.0F;
    float OrbitPitchDegrees = 22.0F;
    float Zoom = 1.0F;
    float PanX = 0.0F;
    float PanY = 0.0F;
};

struct RuntimeStats
{
    Meridian::Core::MainLoopResult Frame;
    int FrameIndex = 0;
};

std::string BuildFrameSummary(const RuntimeStats& stats)
{
    std::ostringstream stream;
    stream << "Frame=" << stats.FrameIndex
           << " Visible=" << stats.Frame.Visible
           << " Draws=" << stats.Frame.Draws
           << " Passes=" << stats.Frame.MergedRenderPasses
           << " Pipelines=" << stats.Frame.PipelineObjects
           << " Commands=" << stats.Frame.Commands;
    return stream.str();
}

std::string BuildFrameDetails(const RuntimeStats& stats)
{
    std::ostringstream stream;
    stream << "QueueBatches=" << stats.Frame.QueueBatches
           << " DescriptorPages=" << stats.Frame.DescriptorPoolPages
           << " BytecodeEntries=" << stats.Frame.BytecodeEntries
           << " CacheHits=" << stats.Frame.PipelineCacheHits
           << " AliasGroups=" << stats.Frame.AliasGroups;
    return stream.str();
}

#ifdef _WIN32
std::wstring Utf8ToWide(const std::string& value)
{
    if (value.empty())
    {
        return {};
    }

    const int count = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
    if (count <= 0)
    {
        return std::wstring(value.begin(), value.end());
    }

    std::wstring wide(static_cast<std::size_t>(count), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, wide.data(), count);
    if (!wide.empty() && wide.back() == L'\0')
    {
        wide.pop_back();
    }
    return wide;
}

std::wstring WideFromFloat(float value)
{
    wchar_t buffer[64]{};
    swprintf_s(buffer, L"%.2f", value);
    return buffer;
}

struct ChromeButtonRects
{
    RECT Minimize{};
    RECT Maximize{};
    RECT Close{};
};

struct MenuButtonDesc
{
    const wchar_t* Label = L"";
    int CommandId = 0;
    HMENU Menu = nullptr;
    RECT Rect{};
};

struct QuickActionDesc
{
    const wchar_t* Label = L"";
    int CommandId = 0;
    RECT Rect{};
};

struct StudioWindowState
{
    Meridian::Application::ApplicationConfiguration Configuration;
    StudioDocument Document;
    LayoutVisibility Visibility;
    StudioLayoutSnapshot Layout;
    RuntimeStats Stats;
    CameraState Camera;
    HWND Window = nullptr;
    std::wstring WindowClassName;
    std::wstring ProductName;
    bool Running = true;
    bool ShowStatsOverlay = true;
    bool ViewportDragging = false;
    bool ViewportPanning = false;
    POINT LastMousePoint{};
    HMENU FileMenu = nullptr;
    HMENU ViewMenu = nullptr;
    HMENU WindowMenu = nullptr;
    HMENU CreateMenu = nullptr;
    HMENU HelpMenu = nullptr;
    std::wstring LastWindowTitle;
    std::chrono::steady_clock::time_point LastFrameInvalidate = std::chrono::steady_clock::now();
    HDC BackBufferContext = nullptr;
    HBITMAP BackBufferBitmap = nullptr;
    HBITMAP BackBufferOldBitmap = nullptr;
    int BackBufferWidth = 0;
    int BackBufferHeight = 0;
};

bool ContainsPoint(const PanelRect& rect, int x, int y)
{
    return rect.Width > 0 && rect.Height > 0 && x >= rect.X && y >= rect.Y && x < rect.Right() && y < rect.Bottom();
}

bool ContainsPoint(const RECT& rect, int x, int y)
{
    return x >= rect.left && y >= rect.top && x < rect.right && y < rect.bottom;
}

RECT ToRect(const PanelRect& rect)
{
    RECT winRect{};
    winRect.left = rect.X;
    winRect.top = rect.Y;
    winRect.right = rect.Right();
    winRect.bottom = rect.Bottom();
    return winRect;
}

void FillRectColor(HDC deviceContext, const RECT& rect, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(deviceContext, &rect, brush);
    DeleteObject(brush);
}

void FrameRectColor(HDC deviceContext, const RECT& rect, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    FrameRect(deviceContext, &rect, brush);
    DeleteObject(brush);
}

void DrawTextLine(HDC deviceContext, int x, int y, const std::wstring& text, COLORREF color)
{
    SetTextColor(deviceContext, color);
    TextOutW(deviceContext, x, y, text.c_str(), static_cast<int>(text.size()));
}

void DrawPanelHeader(HDC deviceContext, const PanelRect& rect, const wchar_t* title)
{
    RECT panelRect = ToRect(rect);
    FillRectColor(deviceContext, panelRect, RGB(33, 35, 41));
    FrameRectColor(deviceContext, panelRect, RGB(67, 71, 81));

    RECT headerRect = panelRect;
    headerRect.bottom = headerRect.top + 28;
    FillRectColor(deviceContext, headerRect, RGB(43, 47, 56));
    DrawTextLine(deviceContext, rect.X + 10, rect.Y + 6, title, RGB(228, 232, 240));
}

HMENU CreateFileMenu()
{
    HMENU fileMenu = CreatePopupMenu();
    AppendMenuW(fileMenu, MF_STRING, CommandFileNewScene, L"New Scene");
    AppendMenuW(fileMenu, MF_STRING, CommandFileOpenSampleScene, L"Open Sample Scene");
    AppendMenuW(fileMenu, MF_STRING, CommandFileSaveScene, L"Save Scene");
    AppendMenuW(fileMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(fileMenu, MF_STRING, CommandFileExit, L"Exit");
    return fileMenu;
}

HMENU CreateViewMenu()
{
    HMENU viewMenu = CreatePopupMenu();
    AppendMenuW(viewMenu, MF_STRING, CommandViewResetLayout, L"Reset Layout");
    AppendMenuW(viewMenu, MF_STRING, CommandViewToggleStats, L"Toggle Stats Overlay");
    return viewMenu;
}

HMENU CreateWindowMenu()
{
    HMENU windowMenu = CreatePopupMenu();
    AppendMenuW(windowMenu, MF_STRING, CommandWindowToggleOutliner, L"Scene Outliner");
    AppendMenuW(windowMenu, MF_STRING, CommandWindowToggleContentBrowser, L"Content Browser");
    AppendMenuW(windowMenu, MF_STRING, CommandWindowToggleDetails, L"Details");
    AppendMenuW(windowMenu, MF_STRING, CommandWindowToggleOutputLog, L"Output Log");
    return windowMenu;
}

HMENU CreateCreateMenu()
{
    HMENU createMenu = CreatePopupMenu();
    AppendMenuW(createMenu, MF_STRING, CommandCreateAddStaticMesh, L"Add Static Mesh Actor");
    AppendMenuW(createMenu, MF_STRING, CommandCreateAddCamera, L"Add Camera Actor");
    return createMenu;
}

HMENU CreateHelpMenu()
{
    HMENU helpMenu = CreatePopupMenu();
    AppendMenuW(helpMenu, MF_STRING, CommandHelpAbout, L"About MeridianStudio");
    return helpMenu;
}

void DestroyPopupMenus(StudioWindowState& state)
{
    if (state.FileMenu != nullptr)
    {
        DestroyMenu(state.FileMenu);
        state.FileMenu = nullptr;
    }
    if (state.ViewMenu != nullptr)
    {
        DestroyMenu(state.ViewMenu);
        state.ViewMenu = nullptr;
    }
    if (state.WindowMenu != nullptr)
    {
        DestroyMenu(state.WindowMenu);
        state.WindowMenu = nullptr;
    }
    if (state.CreateMenu != nullptr)
    {
        DestroyMenu(state.CreateMenu);
        state.CreateMenu = nullptr;
    }
    if (state.HelpMenu != nullptr)
    {
        DestroyMenu(state.HelpMenu);
        state.HelpMenu = nullptr;
    }
}

void SyncPopupMenuChecks(StudioWindowState& state)
{
    if (state.WindowMenu != nullptr)
    {
        CheckMenuItem(state.WindowMenu, CommandWindowToggleOutliner, MF_BYCOMMAND | (state.Visibility.ShowSceneOutliner ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(state.WindowMenu, CommandWindowToggleContentBrowser, MF_BYCOMMAND | (state.Visibility.ShowContentBrowser ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(state.WindowMenu, CommandWindowToggleDetails, MF_BYCOMMAND | (state.Visibility.ShowDetails ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(state.WindowMenu, CommandWindowToggleOutputLog, MF_BYCOMMAND | (state.Visibility.ShowOutputLog ? MF_CHECKED : MF_UNCHECKED));
    }
    if (state.ViewMenu != nullptr)
    {
        CheckMenuItem(state.ViewMenu, CommandViewToggleStats, MF_BYCOMMAND | (state.ShowStatsOverlay ? MF_CHECKED : MF_UNCHECKED));
    }
}

void UpdateLayoutFromClientArea(StudioWindowState& state, HWND windowHandle)
{
    RECT clientRect{};
    GetClientRect(windowHandle, &clientRect);
    state.Layout = StudioLayoutEngine::Build(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, state.Visibility);
}

ChromeButtonRects GetChromeButtonRects(const PanelRect& titleBar)
{
    ChromeButtonRects rects;
    constexpr int buttonWidth = 46;
    rects.Close = RECT{titleBar.Right() - buttonWidth, titleBar.Y, titleBar.Right(), titleBar.Bottom()};
    rects.Maximize = RECT{rects.Close.left - buttonWidth, titleBar.Y, rects.Close.left, titleBar.Bottom()};
    rects.Minimize = RECT{rects.Maximize.left - buttonWidth, titleBar.Y, rects.Maximize.left, titleBar.Bottom()};
    return rects;
}

std::array<MenuButtonDesc, 5> BuildMenuButtons(const StudioWindowState& state)
{
    std::array<MenuButtonDesc, 5> buttons{{
        {L"File", 0, state.FileMenu, {}},
        {L"View", 0, state.ViewMenu, {}},
        {L"Window", 0, state.WindowMenu, {}},
        {L"Create", 0, state.CreateMenu, {}},
        {L"Help", 0, state.HelpMenu, {}}
    }};

    int x = state.Layout.TitleBar.X + 150;
    for (MenuButtonDesc& button : buttons)
    {
        const int width = 16 + static_cast<int>(std::wcslen(button.Label)) * 9;
        button.Rect = RECT{x, state.Layout.TitleBar.Y + 6, x + width, state.Layout.TitleBar.Bottom() - 6};
        x += width + 6;
    }
    return buttons;
}

std::array<QuickActionDesc, 5> BuildQuickActions(const StudioWindowState& state)
{
    std::array<QuickActionDesc, 5> actions{{
        {L"New Scene", CommandFileNewScene, {}},
        {L"Open Sample", CommandFileOpenSampleScene, {}},
        {L"Save", CommandFileSaveScene, {}},
        {L"Add Mesh", CommandCreateAddStaticMesh, {}},
        {L"Reset Layout", CommandViewResetLayout, {}}
    }};

    int x = state.Layout.TitleBar.Right() - 520;
    x = std::max(x, state.Layout.TitleBar.X + 520);
    for (QuickActionDesc& action : actions)
    {
        const int width = 26 + static_cast<int>(std::wcslen(action.Label)) * 8;
        action.Rect = RECT{x, state.Layout.TitleBar.Y + 6, x + width, state.Layout.TitleBar.Bottom() - 6};
        x += width + 8;
    }
    return actions;
}

void InvalidatePanel(HWND windowHandle, const PanelRect& rect)
{
    if (rect.Width <= 0 || rect.Height <= 0)
    {
        return;
    }
    const RECT winRect = ToRect(rect);
    InvalidateRect(windowHandle, &winRect, FALSE);
}

void InvalidateSelectionDependentPanels(const StudioWindowState& state, HWND windowHandle)
{
    InvalidatePanel(windowHandle, state.Layout.SceneOutliner);
    InvalidatePanel(windowHandle, state.Layout.Viewport);
    InvalidatePanel(windowHandle, state.Layout.Details);
    InvalidatePanel(windowHandle, state.Layout.StatusBar);
}

void InvalidateCameraDependentPanels(const StudioWindowState& state, HWND windowHandle)
{
    InvalidatePanel(windowHandle, state.Layout.Viewport);
    InvalidatePanel(windowHandle, state.Layout.Details);
}

bool RectanglesIntersect(const RECT& a, const RECT& b)
{
    RECT intersection{};
    return IntersectRect(&intersection, &a, &b) != FALSE;
}

void ReleaseBackBuffer(StudioWindowState& state)
{
    if (state.BackBufferContext != nullptr)
    {
        SelectObject(state.BackBufferContext, state.BackBufferOldBitmap);
        DeleteObject(state.BackBufferBitmap);
        DeleteDC(state.BackBufferContext);
        state.BackBufferContext = nullptr;
        state.BackBufferBitmap = nullptr;
        state.BackBufferOldBitmap = nullptr;
        state.BackBufferWidth = 0;
        state.BackBufferHeight = 0;
    }
}

void EnsureBackBuffer(StudioWindowState& state, HDC deviceContext, int width, int height)
{
    width = std::max(width, 1);
    height = std::max(height, 1);
    if (state.BackBufferContext != nullptr && state.BackBufferWidth == width && state.BackBufferHeight == height)
    {
        return;
    }

    ReleaseBackBuffer(state);
    state.BackBufferContext = CreateCompatibleDC(deviceContext);
    state.BackBufferBitmap = CreateCompatibleBitmap(deviceContext, width, height);
    state.BackBufferOldBitmap = static_cast<HBITMAP>(SelectObject(state.BackBufferContext, state.BackBufferBitmap));
    state.BackBufferWidth = width;
    state.BackBufferHeight = height;
    SetBkMode(state.BackBufferContext, TRANSPARENT);
}

struct ProjectedPoint
{
    int X = 0;
    int Y = 0;
};

ProjectedPoint ProjectWorldPoint(const PanelRect& viewportRect, const CameraState& camera, float x, float y, float z)
{
    const float yaw = camera.OrbitYawDegrees * 3.1415926535F / 180.0F;
    const float pitch = camera.OrbitPitchDegrees * 3.1415926535F / 180.0F;

    const float cosYaw = std::cos(yaw);
    const float sinYaw = std::sin(yaw);
    const float cosPitch = std::cos(pitch);
    const float sinPitch = std::sin(pitch);

    float rotatedX = x * cosYaw - z * sinYaw;
    float rotatedZ = x * sinYaw + z * cosYaw;
    float rotatedY = y * cosPitch - rotatedZ * sinPitch;
    rotatedZ = y * sinPitch + rotatedZ * cosPitch;

    const float scale = 20.0F * camera.Zoom;
    const float screenX = static_cast<float>(viewportRect.X + viewportRect.Width / 2) + rotatedX * scale + camera.PanX;
    const float screenY = static_cast<float>(viewportRect.Y + viewportRect.Height / 2) - rotatedY * scale + camera.PanY;
    return {static_cast<int>(screenX), static_cast<int>(screenY)};
}

void DrawTitleBar(HDC deviceContext, const StudioWindowState& state)
{
    RECT rect = ToRect(state.Layout.TitleBar);
    FillRectColor(deviceContext, rect, RGB(27, 30, 36));
    FrameRectColor(deviceContext, rect, RGB(58, 64, 74));

    RECT iconRect{state.Layout.TitleBar.X + 10, state.Layout.TitleBar.Y + 11, state.Layout.TitleBar.X + 22, state.Layout.TitleBar.Y + 23};
    FillRectColor(deviceContext, iconRect, RGB(116, 184, 255));
    FrameRectColor(deviceContext, iconRect, RGB(216, 228, 246));

    DrawTextLine(deviceContext, state.Layout.TitleBar.X + 32, state.Layout.TitleBar.Y + 11, state.ProductName, RGB(239, 243, 248));

    for (const MenuButtonDesc& button : BuildMenuButtons(state))
    {
        FillRectColor(deviceContext, button.Rect, RGB(48, 53, 61));
        FrameRectColor(deviceContext, button.Rect, RGB(78, 86, 98));
        DrawTextLine(deviceContext, button.Rect.left + 10, button.Rect.top + 6, button.Label, RGB(232, 236, 240));
    }

    for (const QuickActionDesc& action : BuildQuickActions(state))
    {
        FillRectColor(deviceContext, action.Rect, RGB(52, 63, 80));
        FrameRectColor(deviceContext, action.Rect, RGB(92, 118, 152));
        DrawTextLine(deviceContext, action.Rect.left + 10, action.Rect.top + 6, action.Label, RGB(218, 227, 240));
    }

    const ChromeButtonRects buttons = GetChromeButtonRects(state.Layout.TitleBar);
    FillRectColor(deviceContext, buttons.Minimize, RGB(36, 40, 48));
    FillRectColor(deviceContext, buttons.Maximize, RGB(36, 40, 48));
    FillRectColor(deviceContext, buttons.Close, RGB(160, 62, 62));
    DrawTextLine(deviceContext, buttons.Minimize.left + 18, buttons.Minimize.top + 8, L"_", RGB(230, 234, 239));
    DrawTextLine(deviceContext, buttons.Maximize.left + 14, buttons.Maximize.top + 7,
        IsZoomed(state.Window) ? L"[]" : L"O", RGB(230, 234, 239));
    DrawTextLine(deviceContext, buttons.Close.left + 17, buttons.Close.top + 8, L"X", RGB(245, 245, 245));
}

void DrawViewportToolbar(HDC deviceContext, const PanelRect& viewportRect)
{
    RECT toolbarRect{};
    toolbarRect.left = viewportRect.X + 8;
    toolbarRect.top = viewportRect.Y + 8;
    toolbarRect.right = viewportRect.X + 250;
    toolbarRect.bottom = viewportRect.Y + 38;
    FillRectColor(deviceContext, toolbarRect, RGB(41, 46, 58));
    FrameRectColor(deviceContext, toolbarRect, RGB(86, 94, 110));
    DrawTextLine(deviceContext, toolbarRect.left + 10, toolbarRect.top + 8, L"Perspective | Lit | Stats", RGB(233, 236, 242));
}

void DrawViewportScene(HDC deviceContext, const PanelRect& viewportRect, const StudioWindowState& state)
{
    RECT rect = ToRect(viewportRect);
    FillRectColor(deviceContext, rect, RGB(24, 28, 36));
    FrameRectColor(deviceContext, rect, RGB(73, 80, 92));
    DrawViewportToolbar(deviceContext, viewportRect);

    HPEN majorGridPen = CreatePen(PS_SOLID, 1, RGB(60, 66, 75));
    HPEN axisPenX = CreatePen(PS_SOLID, 2, RGB(220, 80, 80));
    HPEN axisPenY = CreatePen(PS_SOLID, 2, RGB(80, 200, 100));
    HPEN axisPenZ = CreatePen(PS_SOLID, 2, RGB(80, 140, 220));
    HPEN objectPen = CreatePen(PS_SOLID, 1, RGB(190, 198, 210));
    HPEN selectedPen = CreatePen(PS_SOLID, 2, RGB(255, 193, 76));

    auto drawLine = [&](float x0, float y0, float z0, float x1, float y1, float z1, HPEN pen)
    {
        HPEN oldPen = static_cast<HPEN>(SelectObject(deviceContext, pen));
        const ProjectedPoint p0 = ProjectWorldPoint(viewportRect, state.Camera, x0, y0, z0);
        const ProjectedPoint p1 = ProjectWorldPoint(viewportRect, state.Camera, x1, y1, z1);
        MoveToEx(deviceContext, p0.X, p0.Y, nullptr);
        LineTo(deviceContext, p1.X, p1.Y);
        SelectObject(deviceContext, oldPen);
    };

    for (int i = -12; i <= 12; ++i)
    {
        drawLine(static_cast<float>(i), 0.0F, -12.0F, static_cast<float>(i), 0.0F, 12.0F, majorGridPen);
        drawLine(-12.0F, 0.0F, static_cast<float>(i), 12.0F, 0.0F, static_cast<float>(i), majorGridPen);
    }

    drawLine(0.0F, 0.0F, 0.0F, 4.0F, 0.0F, 0.0F, axisPenX);
    drawLine(0.0F, 0.0F, 0.0F, 0.0F, 4.0F, 0.0F, axisPenY);
    drawLine(0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 4.0F, axisPenZ);

    for (const SceneItem& item : state.Document.GetSceneItems())
    {
        if (!item.Visible || item.Type == SceneItemType::Folder || item.Type == SceneItemType::Grid || item.Type == SceneItemType::Environment)
        {
            continue;
        }

        const float ex = item.ExtentX;
        const float ey = item.ExtentY;
        const float ez = item.ExtentZ;
        const float x = item.PositionX;
        const float y = item.PositionY;
        const float z = item.PositionZ;
        const bool selected = item.Id == state.Document.GetSelectedSceneItemId();
        HPEN pen = selected ? selectedPen : objectPen;

        const std::array<std::array<float, 3>, 8> corners = {{
            {x - ex, y - ey, z - ez}, {x + ex, y - ey, z - ez}, {x + ex, y + ey, z - ez}, {x - ex, y + ey, z - ez},
            {x - ex, y - ey, z + ez}, {x + ex, y - ey, z + ez}, {x + ex, y + ey, z + ez}, {x - ex, y + ey, z + ez}}};
        const int edgePairs[12][2] = {
            {0,1},{1,2},{2,3},{3,0},
            {4,5},{5,6},{6,7},{7,4},
            {0,4},{1,5},{2,6},{3,7}};

        for (const auto& edgePair : edgePairs)
        {
            const auto& a = corners[edgePair[0]];
            const auto& b = corners[edgePair[1]];
            drawLine(a[0], a[1], a[2], b[0], b[1], b[2], pen);
        }

        const ProjectedPoint labelPoint = ProjectWorldPoint(viewportRect, state.Camera, x, y + ey + 0.2F, z);
        DrawTextLine(deviceContext, labelPoint.X + 6, labelPoint.Y - 8, Utf8ToWide(item.Name), selected ? RGB(255, 208, 112) : RGB(214, 220, 228));
    }

    if (state.ShowStatsOverlay)
    {
        RECT overlayRect{};
        overlayRect.left = viewportRect.X + 8;
        overlayRect.top = viewportRect.Y + 50;
        overlayRect.right = viewportRect.X + 410;
        overlayRect.bottom = viewportRect.Y + 124;
        FillRectColor(deviceContext, overlayRect, RGB(36, 42, 54));
        FrameRectColor(deviceContext, overlayRect, RGB(86, 94, 110));
        DrawTextLine(deviceContext, overlayRect.left + 10, overlayRect.top + 8, Utf8ToWide(BuildFrameSummary(state.Stats)), RGB(241, 243, 247));
        DrawTextLine(deviceContext, overlayRect.left + 10, overlayRect.top + 32, Utf8ToWide(BuildFrameDetails(state.Stats)), RGB(192, 199, 208));
        DrawTextLine(deviceContext, overlayRect.left + 10, overlayRect.top + 56, L"LMB Orbit  RMB Pan  MouseWheel Zoom", RGB(154, 164, 178));
    }

    DeleteObject(majorGridPen);
    DeleteObject(axisPenX);
    DeleteObject(axisPenY);
    DeleteObject(axisPenZ);
    DeleteObject(objectPen);
    DeleteObject(selectedPen);
}

void DrawSceneOutliner(HDC deviceContext, const PanelRect& rect, const StudioWindowState& state)
{
    DrawPanelHeader(deviceContext, rect, L"Scene Outliner");
    int y = rect.Y + 36;
    for (const SceneItem& item : state.Document.GetSceneItems())
    {
        RECT rowRect{rect.X + 6, y, rect.Right() - 6, y + 24};
        const bool selected = item.Id == state.Document.GetSelectedSceneItemId();
        if (selected)
        {
            FillRectColor(deviceContext, rowRect, RGB(72, 80, 98));
        }
        const int indent = item.ParentId < 0 ? 0 : 18;
        DrawTextLine(deviceContext, rect.X + 14 + indent, y + 4, Utf8ToWide(item.Name), selected ? RGB(255, 221, 132) : RGB(219, 223, 228));
        DrawTextLine(deviceContext, rect.Right() - 78, y + 4, Utf8ToWide(item.Visible ? "Visible" : "Hidden"), RGB(144, 154, 166));
        y += 24;
        if (y + 24 > rect.Bottom())
        {
            break;
        }
    }
}

void DrawContentBrowser(HDC deviceContext, const PanelRect& rect, const StudioWindowState& state)
{
    DrawPanelHeader(deviceContext, rect, L"Content Browser");
    int y = rect.Y + 36;
    for (const AssetItem& item : state.Document.GetAssets())
    {
        DrawTextLine(deviceContext, rect.X + 12, y, Utf8ToWide(item.Name), RGB(222, 226, 231));
        DrawTextLine(deviceContext, rect.Right() - 90, y, Utf8ToWide(item.Type), RGB(145, 157, 170));
        y += 22;
        if (y + 22 > rect.Bottom())
        {
            break;
        }
    }
}

void DrawDetails(HDC deviceContext, const PanelRect& rect, const StudioWindowState& state)
{
    DrawPanelHeader(deviceContext, rect, L"Details");
    int y = rect.Y + 38;
    std::wstring selectionLabel = L"Selected: ";
    if (const SceneItem* selected = state.Document.GetSelectedSceneItem(); selected != nullptr)
    {
        selectionLabel += Utf8ToWide(selected->Name);
    }
    else
    {
        selectionLabel += L"None";
    }
    DrawTextLine(deviceContext, rect.X + 12, y, selectionLabel, RGB(240, 243, 247));
    y += 28;

    std::string currentCategory;
    for (const PropertyRow& row : state.Document.BuildSelectedPropertyRows())
    {
        if (row.Category != currentCategory)
        {
            currentCategory = row.Category;
            DrawTextLine(deviceContext, rect.X + 12, y, Utf8ToWide(currentCategory), RGB(152, 168, 190));
            y += 22;
        }
        DrawTextLine(deviceContext, rect.X + 18, y, Utf8ToWide(row.Name), RGB(204, 210, 216));
        DrawTextLine(deviceContext, rect.X + 150, y, Utf8ToWide(row.Value), RGB(237, 240, 244));
        y += 20;
        if (y + 24 > rect.Bottom())
        {
            break;
        }
    }

    y += 12;
    DrawTextLine(deviceContext, rect.X + 12, y, L"Editor Camera", RGB(152, 168, 190));
    y += 22;
    DrawTextLine(deviceContext, rect.X + 18, y, L"Orbit Yaw", RGB(204, 210, 216));
    DrawTextLine(deviceContext, rect.X + 150, y, WideFromFloat(state.Camera.OrbitYawDegrees), RGB(237, 240, 244));
    y += 20;
    DrawTextLine(deviceContext, rect.X + 18, y, L"Orbit Pitch", RGB(204, 210, 216));
    DrawTextLine(deviceContext, rect.X + 150, y, WideFromFloat(state.Camera.OrbitPitchDegrees), RGB(237, 240, 244));
    y += 20;
    DrawTextLine(deviceContext, rect.X + 18, y, L"Zoom", RGB(204, 210, 216));
    DrawTextLine(deviceContext, rect.X + 150, y, WideFromFloat(state.Camera.Zoom), RGB(237, 240, 244));
}

void DrawOutputLog(HDC deviceContext, const PanelRect& rect, const StudioWindowState& state)
{
    DrawPanelHeader(deviceContext, rect, L"Output Log");
    int y = rect.Y + 38;
    for (const LogEntry& entry : state.Document.GetLogs())
    {
        DrawTextLine(deviceContext, rect.X + 12, y, Utf8ToWide("[" + entry.Channel + "]"), RGB(154, 179, 220));
        DrawTextLine(deviceContext, rect.X + 124, y, Utf8ToWide(entry.Message), RGB(220, 225, 232));
        y += 22;
        if (y + 22 > rect.Bottom())
        {
            break;
        }
    }
}

void DrawStatusBar(HDC deviceContext, const PanelRect& rect, const StudioWindowState& state)
{
    RECT barRect = ToRect(rect);
    FillRectColor(deviceContext, barRect, RGB(37, 40, 46));
    FrameRectColor(deviceContext, barRect, RGB(72, 78, 88));

    std::ostringstream status;
    status << "Workspace: EditorShell   SceneItems=" << state.Document.GetSceneItems().size()
           << "   Assets=" << state.Document.GetAssets().size()
           << "   Selected=";
    if (const SceneItem* selected = state.Document.GetSelectedSceneItem(); selected != nullptr)
    {
        status << selected->Name;
    }
    else
    {
        status << "None";
    }
    DrawTextLine(deviceContext, rect.X + 10, rect.Y + 5, Utf8ToWide(status.str()), RGB(212, 216, 222));
}

void UpdateWindowTitle(StudioWindowState& state)
{
    std::ostringstream stream;
    stream << state.Configuration.ProductName
           << " | Scene=" << state.Document.GetSceneItems().size()
           << " Assets=" << state.Document.GetAssets().size();
    if (const SceneItem* selected = state.Document.GetSelectedSceneItem(); selected != nullptr)
    {
        stream << " Selected=" << selected->Name;
    }
    const std::wstring title = Utf8ToWide(stream.str());
    if (title != state.LastWindowTitle)
    {
        SetWindowTextW(state.Window, title.c_str());
        state.LastWindowTitle = title;
    }
}

void HandleMenuCommand(StudioWindowState& state, HWND windowHandle, int commandId)
{
    switch (commandId)
    {
    case CommandFileNewScene:
    case CommandFileOpenSampleScene:
        state.Document.ResetToDefaultScene();
        break;
    case CommandFileSaveScene:
        state.Document.AppendLog("Save", "Scene serialization stub executed. Persistent document backend pending.");
        break;
    case CommandFileExit:
        DestroyWindow(windowHandle);
        return;
    case CommandViewResetLayout:
        state.Visibility = {};
        state.ShowStatsOverlay = true;
        state.Document.AppendLog("Layout", "Docked layout reset to default workspace.");
        break;
    case CommandViewToggleStats:
        state.ShowStatsOverlay = !state.ShowStatsOverlay;
        state.Document.AppendLog("Viewport", state.ShowStatsOverlay ? "Stats overlay enabled." : "Stats overlay disabled.");
        break;
    case CommandWindowToggleOutliner:
        state.Visibility.ShowSceneOutliner = !state.Visibility.ShowSceneOutliner;
        state.Document.AppendLog("Window", state.Visibility.ShowSceneOutliner ? "Scene Outliner shown." : "Scene Outliner hidden.");
        break;
    case CommandWindowToggleContentBrowser:
        state.Visibility.ShowContentBrowser = !state.Visibility.ShowContentBrowser;
        state.Document.AppendLog("Window", state.Visibility.ShowContentBrowser ? "Content Browser shown." : "Content Browser hidden.");
        break;
    case CommandWindowToggleDetails:
        state.Visibility.ShowDetails = !state.Visibility.ShowDetails;
        state.Document.AppendLog("Window", state.Visibility.ShowDetails ? "Details panel shown." : "Details panel hidden.");
        break;
    case CommandWindowToggleOutputLog:
        state.Visibility.ShowOutputLog = !state.Visibility.ShowOutputLog;
        state.Document.AppendLog("Window", state.Visibility.ShowOutputLog ? "Output Log shown." : "Output Log hidden.");
        break;
    case CommandCreateAddStaticMesh:
        state.Document.AddStaticMeshActor();
        break;
    case CommandCreateAddCamera:
        state.Document.AddCameraActor();
        break;
    case CommandHelpAbout:
        MessageBoxW(windowHandle,
            L"MeridianStudio\n\nThis build now uses a custom studio chrome:\n- single top command row\n- captionless outer window\n- double-buffered editor painting\n- viewport-focused redraw policy\n\nIt remains a shell milestone, not the final renderer/editor product.",
            L"About MeridianStudio",
            MB_OK | MB_ICONINFORMATION);
        break;
    default:
        break;
    }

    UpdateLayoutFromClientArea(state, windowHandle);
    SyncPopupMenuChecks(state);
    InvalidateRect(windowHandle, nullptr, FALSE);
}

void ShowToolbarPopup(HWND windowHandle, const RECT& buttonRect, HMENU popupMenu)
{
    if (popupMenu == nullptr)
    {
        return;
    }

    POINT menuOrigin{buttonRect.left, buttonRect.bottom};
    ClientToScreen(windowHandle, &menuOrigin);
    TrackPopupMenu(popupMenu, TPM_LEFTALIGN | TPM_TOPALIGN, menuOrigin.x, menuOrigin.y, 0, windowHandle, nullptr);
}

bool HandleTitleBarClick(StudioWindowState& state, HWND windowHandle, int x, int y)
{
    if (!ContainsPoint(state.Layout.TitleBar, x, y))
    {
        return false;
    }

    const ChromeButtonRects buttons = GetChromeButtonRects(state.Layout.TitleBar);
    if (ContainsPoint(buttons.Close, x, y))
    {
        PostMessageW(windowHandle, WM_CLOSE, 0, 0);
        return true;
    }
    if (ContainsPoint(buttons.Maximize, x, y))
    {
        ShowWindow(windowHandle, IsZoomed(windowHandle) ? SW_RESTORE : SW_MAXIMIZE);
        return true;
    }
    if (ContainsPoint(buttons.Minimize, x, y))
    {
        ShowWindow(windowHandle, SW_MINIMIZE);
        return true;
    }

    for (const MenuButtonDesc& button : BuildMenuButtons(state))
    {
        if (ContainsPoint(button.Rect, x, y))
        {
            SyncPopupMenuChecks(state);
            ShowToolbarPopup(windowHandle, button.Rect, button.Menu);
            return true;
        }
    }

    for (const QuickActionDesc& action : BuildQuickActions(state))
    {
        if (ContainsPoint(action.Rect, x, y))
        {
            HandleMenuCommand(state, windowHandle, action.CommandId);
            return true;
        }
    }

    ReleaseCapture();
    SendMessageW(windowHandle, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(x, y));
    return true;
}

void HandleLeftClick(StudioWindowState& state, HWND windowHandle, int x, int y)
{
    if (HandleTitleBarClick(state, windowHandle, x, y))
    {
        return;
    }

    if (ContainsPoint(state.Layout.SceneOutliner, x, y))
    {
        const int rowIndex = (y - state.Layout.SceneOutliner.Y - 36) / 24;
        if (rowIndex >= 0)
        {
            const auto& items = state.Document.GetSceneItems();
            if (rowIndex < static_cast<int>(items.size()))
            {
                state.Document.SetSelectedSceneItemById(items[static_cast<std::size_t>(rowIndex)].Id);
                UpdateWindowTitle(state);
                InvalidateSelectionDependentPanels(state, windowHandle);
            }
        }
        return;
    }

    if (ContainsPoint(state.Layout.Viewport, x, y))
    {
        state.ViewportDragging = true;
        state.LastMousePoint = POINT{x, y};
        SetCapture(windowHandle);
    }
}

void HandleRightClick(StudioWindowState& state, HWND windowHandle, int x, int y)
{
    if (ContainsPoint(state.Layout.Viewport, x, y))
    {
        state.ViewportPanning = true;
        state.LastMousePoint = POINT{x, y};
        SetCapture(windowHandle);
    }
}

LRESULT CALLBACK MeridianMainFrameProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    StudioWindowState* state = reinterpret_cast<StudioWindowState*>(GetWindowLongPtrW(windowHandle, GWLP_USERDATA));

    switch (message)
    {
    case WM_NCCREATE:
        {
            auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
            auto* initialState = reinterpret_cast<StudioWindowState*>(createStruct->lpCreateParams);
            SetWindowLongPtrW(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(initialState));
            return TRUE;
        }
    case WM_ERASEBKGND:
        return 1;
    case WM_NCHITTEST:
        {
            const LRESULT defaultHit = DefWindowProcW(windowHandle, message, wParam, lParam);
            if (defaultHit != HTCLIENT || state == nullptr)
            {
                return defaultHit;
            }

            RECT windowRect{};
            GetWindowRect(windowHandle, &windowRect);
            const int borderX = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            const int borderY = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            const int x = GET_X_LPARAM(lParam) - windowRect.left;
            const int y = GET_Y_LPARAM(lParam) - windowRect.top;
            const int width = windowRect.right - windowRect.left;
            const int height = windowRect.bottom - windowRect.top;

            const bool left = x < borderX;
            const bool right = x >= width - borderX;
            const bool top = y < borderY;
            const bool bottom = y >= height - borderY;

            if (top && left) return HTTOPLEFT;
            if (top && right) return HTTOPRIGHT;
            if (bottom && left) return HTBOTTOMLEFT;
            if (bottom && right) return HTBOTTOMRIGHT;
            if (left) return HTLEFT;
            if (right) return HTRIGHT;
            if (top) return HTTOP;
            if (bottom) return HTBOTTOM;
            return HTCLIENT;
        }
    case WM_SIZE:
        if (state != nullptr)
        {
            ReleaseBackBuffer(*state);
            UpdateLayoutFromClientArea(*state, windowHandle);
            InvalidateRect(windowHandle, nullptr, FALSE);
        }
        return 0;
    case WM_COMMAND:
        if (state != nullptr)
        {
            HandleMenuCommand(*state, windowHandle, LOWORD(wParam));
        }
        return 0;
    case WM_LBUTTONDBLCLK:
        if (state != nullptr && ContainsPoint(state->Layout.TitleBar, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
        {
            const ChromeButtonRects buttons = GetChromeButtonRects(state->Layout.TitleBar);
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            if (!ContainsPoint(buttons.Minimize, x, y) && !ContainsPoint(buttons.Maximize, x, y) && !ContainsPoint(buttons.Close, x, y))
            {
                ShowWindow(windowHandle, IsZoomed(windowHandle) ? SW_RESTORE : SW_MAXIMIZE);
            }
        }
        return 0;
    case WM_LBUTTONDOWN:
        if (state != nullptr)
        {
            HandleLeftClick(*state, windowHandle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return 0;
    case WM_RBUTTONDOWN:
        if (state != nullptr)
        {
            HandleRightClick(*state, windowHandle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return 0;
    case WM_MOUSEMOVE:
        if (state != nullptr && (state->ViewportDragging || state->ViewportPanning))
        {
            const POINT current{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const int deltaX = current.x - state->LastMousePoint.x;
            const int deltaY = current.y - state->LastMousePoint.y;
            state->LastMousePoint = current;
            if (state->ViewportDragging)
            {
                state->Camera.OrbitYawDegrees += static_cast<float>(deltaX) * 0.35F;
                state->Camera.OrbitPitchDegrees = std::clamp(state->Camera.OrbitPitchDegrees + static_cast<float>(deltaY) * 0.25F, -80.0F, 80.0F);
            }
            if (state->ViewportPanning)
            {
                state->Camera.PanX += static_cast<float>(deltaX);
                state->Camera.PanY += static_cast<float>(deltaY);
            }
            InvalidateCameraDependentPanels(*state, windowHandle);
        }
        return 0;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        if (state != nullptr)
        {
            state->ViewportDragging = false;
            state->ViewportPanning = false;
            ReleaseCapture();
        }
        return 0;
    case WM_MOUSEWHEEL:
        if (state != nullptr)
        {
            const short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            state->Camera.Zoom = std::clamp(state->Camera.Zoom + (wheelDelta > 0 ? 0.08F : -0.08F), 0.35F, 3.5F);
            InvalidateCameraDependentPanels(*state, windowHandle);
        }
        return 0;
    case WM_CLOSE:
        DestroyWindow(windowHandle);
        return 0;
    case WM_DESTROY:
        if (state != nullptr)
        {
            ReleaseBackBuffer(*state);
            state->Running = false;
            state->Window = nullptr;
        }
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        if (state == nullptr)
        {
            return DefWindowProcW(windowHandle, message, wParam, lParam);
        }

        {
            PAINTSTRUCT paintStruct;
            HDC deviceContext = BeginPaint(windowHandle, &paintStruct);

            RECT clientRect{};
            GetClientRect(windowHandle, &clientRect);
            const int clientWidth = static_cast<int>(std::max<LONG>(clientRect.right - clientRect.left, 1L));
            const int clientHeight = static_cast<int>(std::max<LONG>(clientRect.bottom - clientRect.top, 1L));
            EnsureBackBuffer(*state, deviceContext, clientWidth, clientHeight);

            HFONT uiFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            HFONT oldFont = static_cast<HFONT>(SelectObject(state->BackBufferContext, uiFont));

            const RECT& dirtyRect = paintStruct.rcPaint;
            FillRectColor(state->BackBufferContext, dirtyRect, RGB(26, 29, 34));

            const RECT titleBarRect = ToRect(state->Layout.TitleBar);
            const RECT sceneRect = ToRect(state->Layout.SceneOutliner);
            const RECT contentRect = ToRect(state->Layout.ContentBrowser);
            const RECT viewportRect = ToRect(state->Layout.Viewport);
            const RECT detailsRect = ToRect(state->Layout.Details);
            const RECT outputRect = ToRect(state->Layout.OutputLog);
            const RECT statusRect = ToRect(state->Layout.StatusBar);

            if (RectanglesIntersect(dirtyRect, titleBarRect))
            {
                DrawTitleBar(state->BackBufferContext, *state);
            }
            if (state->Visibility.ShowSceneOutliner && RectanglesIntersect(dirtyRect, sceneRect))
            {
                DrawSceneOutliner(state->BackBufferContext, state->Layout.SceneOutliner, *state);
            }
            if (state->Visibility.ShowContentBrowser && RectanglesIntersect(dirtyRect, contentRect))
            {
                DrawContentBrowser(state->BackBufferContext, state->Layout.ContentBrowser, *state);
            }
            if (RectanglesIntersect(dirtyRect, viewportRect))
            {
                DrawViewportScene(state->BackBufferContext, state->Layout.Viewport, *state);
            }
            if (state->Visibility.ShowDetails && RectanglesIntersect(dirtyRect, detailsRect))
            {
                DrawDetails(state->BackBufferContext, state->Layout.Details, *state);
            }
            if (state->Visibility.ShowOutputLog && RectanglesIntersect(dirtyRect, outputRect))
            {
                DrawOutputLog(state->BackBufferContext, state->Layout.OutputLog, *state);
            }
            if (RectanglesIntersect(dirtyRect, statusRect))
            {
                DrawStatusBar(state->BackBufferContext, state->Layout.StatusBar, *state);
            }

            BitBlt(deviceContext,
                dirtyRect.left,
                dirtyRect.top,
                dirtyRect.right - dirtyRect.left,
                dirtyRect.bottom - dirtyRect.top,
                state->BackBufferContext,
                dirtyRect.left,
                dirtyRect.top,
                SRCCOPY);

            SelectObject(state->BackBufferContext, oldFont);
            DeleteObject(uiFont);
            EndPaint(windowHandle, &paintStruct);
        }
        return 0;
    default:
        return DefWindowProcW(windowHandle, message, wParam, lParam);
    }
}

int RunWin32MainFrame(Meridian::Application::ApplicationConfiguration configuration)
{
    HINSTANCE instanceHandle = GetModuleHandleW(nullptr);

    StudioWindowState state;
    state.Configuration = std::move(configuration);
    state.ProductName = Utf8ToWide(state.Configuration.ProductName);
    state.WindowClassName = L"MeridianStudioMainFrameWindow";
    state.FileMenu = CreateFileMenu();
    state.ViewMenu = CreateViewMenu();
    state.WindowMenu = CreateWindowMenu();
    state.CreateMenu = CreateCreateMenu();
    state.HelpMenu = CreateHelpMenu();

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    windowClass.lpfnWndProc = MeridianMainFrameProc;
    windowClass.hInstance = instanceHandle;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = nullptr;
    windowClass.lpszClassName = state.WindowClassName.c_str();

    if (RegisterClassExW(&windowClass) == 0)
    {
        DestroyPopupMenus(state);
        return 1;
    }

    const DWORD windowStyle = WS_OVERLAPPEDWINDOW & ~WS_CAPTION;
    RECT desiredRect{0, 0, std::max(state.Configuration.InitialWidth, 1280), std::max(state.Configuration.InitialHeight, 800)};
    AdjustWindowRect(&desiredRect, windowStyle, FALSE);

    state.Window = CreateWindowExW(
        0,
        state.WindowClassName.c_str(),
        state.ProductName.c_str(),
        windowStyle | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        desiredRect.right - desiredRect.left,
        desiredRect.bottom - desiredRect.top,
        nullptr,
        nullptr,
        instanceHandle,
        &state);

    if (state.Window == nullptr)
    {
        DestroyPopupMenus(state);
        UnregisterClassW(state.WindowClassName.c_str(), instanceHandle);
        return 1;
    }

    UpdateLayoutFromClientArea(state, state.Window);
    SyncPopupMenuChecks(state);
    UpdateWindowTitle(state);

    ShowWindow(state.Window, SW_SHOWDEFAULT);
    UpdateWindow(state.Window);

    Meridian::Core::MainLoop mainLoop;
    MSG message{};
    const auto targetFrameDuration = std::chrono::milliseconds(std::max(1, 1000 / std::max(state.Configuration.TargetFramesPerSecond, 1)));
    auto nextFrameDeadline = std::chrono::steady_clock::now();

    while (state.Running)
    {
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            if (message.message == WM_QUIT)
            {
                state.Running = false;
                break;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        if (!state.Running)
        {
            break;
        }

        if (std::chrono::steady_clock::now() >= nextFrameDeadline)
        {
            ++state.Stats.FrameIndex;
            state.Stats.Frame = mainLoop.RunOneFrame();
            InvalidatePanel(state.Window, state.Layout.Viewport);
            nextFrameDeadline = std::chrono::steady_clock::now() + targetFrameDuration;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (state.Window != nullptr)
    {
        DestroyWindow(state.Window);
        state.Window = nullptr;
    }
    DestroyPopupMenus(state);
    UnregisterClassW(state.WindowClassName.c_str(), instanceHandle);
    return 0;
}
#else
int RunHeadlessShell(const Meridian::Application::ApplicationConfiguration& configuration)
{
    Meridian::Core::MainLoop mainLoop;
    for (int frameIndex = 1; frameIndex <= configuration.HeadlessFrameBudget; ++frameIndex)
    {
        const Meridian::Core::MainLoopResult result = mainLoop.RunOneFrame();
        std::printf("[%s] %s\n", configuration.ProductName.c_str(), BuildFrameSummary({result, frameIndex}).c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    return 0;
}
#endif
}

MainFrameWindow::MainFrameWindow(Meridian::Application::ApplicationConfiguration configuration)
    : m_configuration(std::move(configuration))
{
}

int MainFrameWindow::Run()
{
#ifdef _WIN32
    return RunWin32MainFrame(std::move(m_configuration));
#else
    return RunHeadlessShell(m_configuration);
#endif
}
}
