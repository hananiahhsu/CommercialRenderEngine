#pragma once
#include "Meridian/Renderer/BytecodeDiskIndex.h"
#include "Meridian/Renderer/Common.h"
#include "Meridian/Renderer/PersistentBytecodeStore.h"
#include <unordered_map>

namespace Meridian::Renderer
{
class ShaderBytecodeCache
{
public:
    std::vector<BytecodeBlob> CompileOrLoad(const std::vector<ShaderReflectionRecord>& reflection);
    int GetCacheSize() const noexcept;
    int GetReloadedCount() const noexcept;
    const BytecodeDiskIndex& GetDiskIndex() const noexcept { return m_index; }
    const PersistentBytecodeStore& GetPersistentStore() const noexcept { return m_store; }

private:
    std::unordered_map<std::string, BytecodeBlob> m_runtimeCache;
    BytecodeDiskIndex m_index;
    PersistentBytecodeStore m_store;
    int m_reloadedCount = 0;
};
}
