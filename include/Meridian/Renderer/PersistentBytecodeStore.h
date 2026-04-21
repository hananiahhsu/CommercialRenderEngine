#pragma once
#include "Meridian/Renderer/Common.h"
#include <unordered_map>

namespace Meridian::Renderer
{
class PersistentBytecodeStore
{
public:
    void Persist(const BytecodeBlob& blob);
    int GetPersistedCount() const noexcept;
    bool TryLoad(const std::string& hash, BytecodeBlob& outBlob) const;

private:
    std::unordered_map<std::string, BytecodeBlob> m_store;
};
}
