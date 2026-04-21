#!/usr/bin/env bash
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common_linux.sh"
LOG_FILE="${LOG_DIR}/test_linux.log"

if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
    echo "[INFO] Build tree not found. Running configure first..."
    "${ROOT_DIR}/scripts/linux/configure_linux.sh"
fi

if [[ ! -x "${BUILD_DIR}/MeridianUnitTests" ]]; then
    echo "[INFO] Test binaries not found. Running build first..."
    "${ROOT_DIR}/scripts/linux/build_linux.sh"
fi

echo "[INFO] Running tests."
echo "[INFO] Log   : ${LOG_FILE}"
ctest --test-dir "${BUILD_DIR}" --output-on-failure >"${LOG_FILE}" 2>&1
echo "[OK] Tests passed."
