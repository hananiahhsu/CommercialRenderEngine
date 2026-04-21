#include "Meridian/Renderer/PersistentBytecodeStore.h"

namespace Meridian::Renderer
{
void PersistentBytecodeStore::Persist(const BytecodeBlob& blob)
{
    m_store[blob.Hash] = blob;
}

int PersistentBytecodeStore::GetPersistedCount() const noexcept
{
    return static_cast<int>(m_store.size());
}

bool PersistentBytecodeStore::TryLoad(const std::string& hash, BytecodeBlob& outBlob) const
{
    auto it = m_store.find(hash);
    if (it == m_store.end())
    {
        return false;
    }
    outBlob = it->second;
    return true;
}
}
