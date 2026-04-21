#pragma once

#include <string>

namespace Meridian::Application
{
struct ApplicationConfiguration
{
    std::string ProductName = "MeridianStudio";
    std::string WindowTitle = "MeridianStudio";
    int InitialWidth = 1600;
    int InitialHeight = 900;
    int TargetFramesPerSecond = 60;
    int HeadlessFrameBudget = 120;
};

class DesktopApplication
{
public:
    explicit DesktopApplication(ApplicationConfiguration configuration = {});
    int Run();

private:
    ApplicationConfiguration m_configuration;
};
}
