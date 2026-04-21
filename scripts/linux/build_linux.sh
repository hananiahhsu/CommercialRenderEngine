#!/usr/bin/env bash
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common_linux.sh"
LOG_FILE="${LOG_DIR}/build_linux.log"

if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
    echo "[INFO] Build tree not found. Running configure first..."
    "${ROOT_DIR}/scripts/linux/configure_linux.sh"
fi

echo "[INFO] Building MeridianRenderEngine (${BUILD_CONFIG})."
echo "[INFO] Log   : ${LOG_FILE}"
cmake --build "${BUILD_DIR}" -j"$(nproc)" >"${LOG_FILE}" 2>&1
echo "[OK] Build succeeded."
