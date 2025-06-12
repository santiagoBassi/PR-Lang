#!/bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

INPUT="$1"
shift 1

# Get the base name without the .rp extension
BASENAME="$(basename "$INPUT" .rp)"

# C file and binary with the same base name
TMP_C_FILE="./output/${BASENAME}.c"
OUTPUT_BINARY="./output/${BASENAME}"

# Generate the C code and save it to the .c file
cat "$INPUT" | build/Compiler "$@" > "$TMP_C_FILE"


# Compile the C code to a binary
gcc "$TMP_C_FILE" -o "$OUTPUT_BINARY"

echo "Compilation finished. Binary generated: $OUTPUT_BINARY"
