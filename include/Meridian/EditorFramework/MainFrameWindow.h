#pragma once

#include "Meridian/Application/Application.h"

namespace Meridian::EditorFramework
{
class MainFrameWindow
{
public:
    explicit MainFrameWindow(Meridian::Application::ApplicationConfiguration configuration);
    int Run();

private:
    Meridian::Application::ApplicationConfiguration m_configuration;
};
}
