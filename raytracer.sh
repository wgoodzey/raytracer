#!/usr/bin/env bash

set -euo pipefail

# ---- Config (override via env) ------------------------------------------------
BUILD_DIR="build"
BUILD_TYPE="Release"
RENDERS_DIR="output"
BIN_NAME="raytracer"
BIN="./${BUILD_DIR}/${BIN_NAME}"

# ---- Helpers -----------------------------------------------------------------
cpu_count() {
  if command -v nproc >/dev/null 2>&1; then
    nproc
  elif [[ "$(uname -s)" == "Darwin" ]]; then
    sysctl -n hw.logicalcpu 2>/dev/null || sysctl -n hw.ncpu
  else
    echo 1
  fi
}

usage() {
  cat <<'EOF'
Usage:
  ./raytracer.sh build          [cmake-args...]
                 run            [program-args...]
                 open
                 convert <file.ppm>
                 clean
                 clean-outputs
                 help
EOF
}

ensure_dirs() {
  mkdir -p "${BUILD_DIR}" "${RENDERS_DIR}"
}

latest_render() {
  ls -t "${RENDERS_DIR}"/*.ppm 2>/dev/null | head -n 1 || true
}

# ---- Commands -----------------------------------------------------------------
cmd="${1:-help}"; shift || true

case "${cmd}" in
  build)
    ensure_dirs
    cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${1:+$*}
    cmake --build "${BUILD_DIR}" -- -j"$(cpu_count)"
    ;;

  run)
    ensure_dirs
    if [[ ! -x "${BIN}" ]]; then
      echo "Binary not found at ${BIN}. Building first..."
      cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
      cmake --build "${BUILD_DIR}" -- -j"$(cpu_count)"
    fi
    echo "Running ${BIN} $*"
    "${BIN}" "$@"
    ;;

  open)
    file="$(latest_render)"
    if [[ -z "${file}" ]]; then
      echo "No .ppm files found in ${RENDERS_DIR}" >&2
      exit 1
    fi
    opener="$(detect_open)"
    if [[ -z "${opener}" ]]; then
      echo "Open this file manually: ${file}"
    else
      # shellcheck disable=SC2086
      ${opener} "${file}"
    fi
    ;;

  convert)
    target="${1:-}"
    if [[ -z "${target}" ]]; then
      echo "Usage: ./raytracer.sh convert <file.ppm>" >&2
      exit 1
    fi
    if [[ ! -f "${target}" ]]; then
      echo "File not found: ${target}" >&2
      exit 2
    fi
    if [[ "${target##*.}" != "ppm" ]]; then
      echo "File must have .ppm extension" >&2
      exit 3
    fi
    if ! command -v ffmpeg >/dev/null 2>&1; then
      echo "Error: ffmpeg not found. Please install it to use convert." >&2
      exit 4
    fi
    out="${target%.ppm}.png"
    echo "Converting ${target} -> ${out}"
    ffmpeg -y -loglevel error -i "${target}" "${out}"
    echo "Wrote: ${out}"
    ;;

  clean)
    rm -rf "${BUILD_DIR}"
    echo "Removed ${BUILD_DIR}/"
    ;;

  clean-outputs|clean-renders)
    rm -rf "${RENDERS_DIR}"
    echo "Removed ${RENDERS_DIR}/"
    ;;

  help|-h|--help)
    usage
    ;;

  *)
    echo "Unknown command: ${cmd}" >&2
    echo
    usage
    exit 2
    ;;
esac
