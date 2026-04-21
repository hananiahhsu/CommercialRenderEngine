#pragma once
#include <string>
#include <unordered_map>

namespace Meridian::Renderer
{
class BytecodeDiskIndex
{
public:
    void AddRecord(const std::string& shaderName, const std::string& hash);
    int GetRecordCount() const noexcept;
    std::string FindHash(const std::string& shaderName) const;

private:
    std::unordered_map<std::string, std::string> m_records;
};
}
