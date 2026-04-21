#!/usr/bin/env bash
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common_linux.sh"
INSTALL_LOG="${LOG_DIR}/install_linux.log"
PACKAGE_LOG="${LOG_DIR}/package_linux.log"

if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
    echo "[INFO] Build tree not found. Running configure first..."
    "${ROOT_DIR}/scripts/linux/configure_linux.sh"
fi

if [[ ! -x "${BUILD_DIR}/MeridianHeadlessValidation" ]]; then
    echo "[INFO] Package inputs not found. Running build first..."
    "${ROOT_DIR}/scripts/linux/build_linux.sh"
fi

mkdir -p "${INSTALL_DIR}" "${PACKAGE_DIR}"

echo "[INFO] Installing build outputs."
echo "[INFO] Log   : ${INSTALL_LOG}"
cmake --install "${BUILD_DIR}" --prefix "${INSTALL_DIR}" >"${INSTALL_LOG}" 2>&1

echo "[INFO] Packaging artifacts."
echo "[INFO] Log   : ${PACKAGE_LOG}"
cpack --config "${BUILD_DIR}/CPackConfig.cmake" -B "${PACKAGE_DIR}" >"${PACKAGE_LOG}" 2>&1

echo "[OK] Package succeeded."
