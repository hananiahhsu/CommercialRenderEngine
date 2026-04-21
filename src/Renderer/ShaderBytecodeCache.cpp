#include "Meridian/Renderer/ShaderBytecodeCache.h"
#include <sstream>

namespace Meridian::Renderer
{
static std::string MakeHash(const std::string& text)
{
    std::uint64_t value = 1469598103934665603ull;
    for (unsigned char ch : text)
    {
        value ^= ch;
        value *= 1099511628211ull;
    }
    std::ostringstream oss;
    oss << std::hex << value;
    return oss.str();
}

std::vector<BytecodeBlob> ShaderBytecodeCache::CompileOrLoad(const std::vector<ShaderReflectionRecord>& reflection)
{
    std::vector<BytecodeBlob> result;
    m_reloadedCount = 0;
    for (const ShaderReflectionRecord& record : reflection)
    {
        std::string sourceSignature = record.ShaderName + "|" + record.Stage;
        for (const DescriptorBinding& binding : record.Bindings)
        {
            sourceSignature += "|" + binding.Signature();
        }

        const std::string hash = MakeHash(sourceSignature);
        BytecodeBlob blob;
        if (auto it = m_runtimeCache.find(hash); it != m_runtimeCache.end())
        {
            blob = it->second;
            ++m_reloadedCount;
        }
        else if (m_store.TryLoad(hash, blob))
        {
            m_runtimeCache[hash] = blob;
            ++m_reloadedCount;
        }
        else
        {
            blob.ShaderName = record.ShaderName;
            blob.Hash = hash;
            blob.Bytes.assign(sourceSignature.begin(), sourceSignature.end());
            m_runtimeCache[hash] = blob;
            m_store.Persist(blob);
            m_index.AddRecord(record.ShaderName, hash);
        }

        if (m_index.FindHash(record.ShaderName).empty())
        {
            m_index.AddRecord(record.ShaderName, hash);
        }

        result.push_back(blob);
    }
    return result;
}

int ShaderBytecodeCache::GetCacheSize() const noexcept
{
    return static_cast<int>(m_runtimeCache.size());
}

int ShaderBytecodeCache::GetReloadedCount() const noexcept
{
    return m_reloadedCount;
}
}
