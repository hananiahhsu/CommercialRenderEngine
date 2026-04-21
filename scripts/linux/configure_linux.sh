#!/usr/bin/env bash
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common_linux.sh"
LOG_FILE="${LOG_DIR}/configure_linux.log"

echo "[INFO] Configuring MeridianRenderEngine with preset ${CONFIG_PRESET}."
echo "[INFO] Root  : ${ROOT_DIR}"
echo "[INFO] Build : ${BUILD_DIR}"
echo "[INFO] Log   : ${LOG_FILE}"
cmake --preset "${CONFIG_PRESET}" "$@" >"${LOG_FILE}" 2>&1
echo "[OK] Configure succeeded."
