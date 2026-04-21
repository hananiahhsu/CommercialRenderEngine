#include "Meridian/Application/Application.h"
#include "Meridian/EditorFramework/MainFrameWindow.h"

#include <algorithm>
#include <utility>

namespace Meridian::Application
{
DesktopApplication::DesktopApplication(ApplicationConfiguration configuration)
    : m_configuration(std::move(configuration))
{
    m_configuration.InitialWidth = std::max(m_configuration.InitialWidth, 960);
    m_configuration.InitialHeight = std::max(m_configuration.InitialHeight, 640);
    m_configuration.TargetFramesPerSecond = std::max(m_configuration.TargetFramesPerSecond, 1);
    m_configuration.HeadlessFrameBudget = std::max(m_configuration.HeadlessFrameBudget, 1);
}

int DesktopApplication::Run()
{
    Meridian::EditorFramework::MainFrameWindow mainFrame(m_configuration);
    return mainFrame.Run();
}
}
