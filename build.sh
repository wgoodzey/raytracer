#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
BUILD_TYPE="${BUILD_TYPE:-Release}"   # allow override: BUILD_TYPE=Debug ./build.sh build

# Portable CPU count for parallel builds
cpu_count() {
  if command -v nproc >/dev/null 2>&1; then
    nproc
  elif [[ "$(uname -s)" == "Darwin" ]]; then
    sysctl -n hw.ncpu
  else
    echo 1
  fi
}

usage() {
  cat <<EOF
Usage: $0 [command]

Commands:
  all        Same as 'build' (default)
  build      Configure and compile (type via \$BUILD_TYPE, default: ${BUILD_TYPE})
  test       Run ctest in ${BUILD_DIR} (verbose, show failures)
  clean      Remove ${BUILD_DIR}
  rebuild    Clean then build

Examples:
  $0                   # build
  BUILD_TYPE=Debug $0 build
  $0 test
EOF
}

ensure_built() {
  if [[ ! -d "${BUILD_DIR}" ]]; then
    echo "Error: '${BUILD_DIR}' does not exist. Run '$0 build' first." >&2
    exit 1
  fi
}

cmd="${1:-all}"

case "${cmd}" in
  all|build)
    cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    cmake --build "${BUILD_DIR}" -- -j"$(cpu_count)"
    ;;

  test)
    ensure_built
    ctest --test-dir "${BUILD_DIR}" --output-on-failure --verbose
    ;;

  clean)
    rm -rf "${BUILD_DIR}"
    echo "Removed ${BUILD_DIR}"
    ;;

  rebuild)
    rm -rf "${BUILD_DIR}"
    echo "Removed ${BUILD_DIR}"
    cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    cmake --build "${BUILD_DIR}" -- -j"$(cpu_count)"
    ;;

  -h|--help|help)
    usage
    ;;

  *)
    echo "Unknown command: ${cmd}" >&2
    usage
    exit 2
    ;;
esac

