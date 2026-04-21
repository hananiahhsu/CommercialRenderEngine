#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUT_DIR="${ROOT_DIR}/out"
BUILD_DIR="${OUT_DIR}/build-headless"
INSTALL_DIR="${OUT_DIR}/install-headless"
PACKAGE_DIR="${OUT_DIR}/package-headless"
LOG_DIR="${OUT_DIR}/logs"
CONFIG_PRESET="linux-headless-debug"
BUILD_CONFIG="Debug"

mkdir -p "${OUT_DIR}" "${LOG_DIR}"

for tool in cmake ctest cpack; do
    if ! command -v "${tool}" >/dev/null 2>&1; then
        echo "[ERROR] ${tool} was not found in PATH." >&2
        exit 1
    fi
done
