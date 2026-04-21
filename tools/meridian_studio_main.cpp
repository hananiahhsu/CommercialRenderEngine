#include "Meridian/Application/Application.h"

int main()
{
    Meridian::Application::ApplicationConfiguration configuration;
    configuration.ProductName = "MeridianStudio";
    configuration.WindowTitle = "MeridianStudio";
    configuration.InitialWidth = 1600;
    configuration.InitialHeight = 900;
    configuration.TargetFramesPerSecond = 60;
    configuration.HeadlessFrameBudget = 60;

    Meridian::Application::DesktopApplication application(configuration);
    return application.Run();
}
