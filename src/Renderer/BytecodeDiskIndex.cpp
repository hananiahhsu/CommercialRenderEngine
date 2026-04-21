#include "Meridian/Renderer/BytecodeDiskIndex.h"

namespace Meridian::Renderer
{
void BytecodeDiskIndex::AddRecord(const std::string& shaderName, const std::string& hash)
{
    m_records[shaderName] = hash;
}

int BytecodeDiskIndex::GetRecordCount() const noexcept
{
    return static_cast<int>(m_records.size());
}

std::string BytecodeDiskIndex::FindHash(const std::string& shaderName) const
{
    auto it = m_records.find(shaderName);
    return it == m_records.end() ? std::string() : it->second;
}
}
