#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

INPUT="$1"
OUTPUT="$2"
shift 2
build/Compiler "$INPUT" "$OUTPUT" "$@"
