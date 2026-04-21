#pragma once
namespace Meridian::Renderer
{
class SwapchainModel
{
public:
    explicit SwapchainModel(int imageCount = 3) : m_imageCount(imageCount) {}
    int GetImageCount() const noexcept { return m_imageCount; }
private:
    int m_imageCount = 3;
};
}
