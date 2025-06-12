#!/bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

INPUT="$1"
shift 1

# Obtener el nombre base sin la extensión .rp
BASENAME="$(basename "$INPUT" .rp)"

# Archivo C y binario con el mismo nombre base
TMP_C_FILE="./output/${BASENAME}.c"
OUTPUT_BINARY="./output/${BASENAME}"

# Generar el código C y guardarlo en el archivo .c
cat "$INPUT" | build/Compiler "$@" > "$TMP_C_FILE"

# Compilar el código C a binario
gcc "$TMP_C_FILE" -o "$OUTPUT_BINARY"

# Ejecutar el binario (opcional)
# ./"$OUTPUT_BINARY"

# Borrar el archivo temporal (opcional)
# rm "$TMP_C_FILE"

echo "Compilación finalizada. Binario generado: $OUTPUT_BINARY"
