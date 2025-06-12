#ifndef ARGUMENTS
#define ARGUMENTS

#define MIN_PROGRAM_PARAM_COUNT 3

#include "Type.h"
#include <stdio.h>

const char* getInputFilename(const int count, const char** arguments); 
const char* getOutputFilename(const int count, const char** arguments); 

#endif
