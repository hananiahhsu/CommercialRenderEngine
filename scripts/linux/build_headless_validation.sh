#!/usr/bin/env bash
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common_linux.sh"
RUN_LOG="${LOG_DIR}/headless_validation_linux.log"

"${ROOT_DIR}/scripts/linux/configure_linux.sh"
"${ROOT_DIR}/scripts/linux/build_linux.sh"
"${ROOT_DIR}/scripts/linux/test_linux.sh"

echo "[INFO] Running headless validation."
echo "[INFO] Log   : ${RUN_LOG}"
"${BUILD_DIR}/MeridianHeadlessValidation" >"${RUN_LOG}" 2>&1

echo "[OK] Headless validation succeeded."
