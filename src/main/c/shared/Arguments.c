#include "Arguments.h"
#include <stdio.h>

const char* getInputFilename(const int count, const char** arguments) {
    if (count < MIN_PROGRAM_PARAM_COUNT) return NULL;

    return arguments[1];
}

const char* getOutputFilename(const int count, const char** arguments) {
    if (count < MIN_PROGRAM_PARAM_COUNT) return NULL;

    return arguments[2];
}
