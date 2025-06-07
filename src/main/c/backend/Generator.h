#ifndef GENERATOR_H
#define GENERATOR_H

#include "../shared/CompilerState.h"

void initializeGeneratorModule();
void generateCode(CompilerState *compilerState);
void shutdownGeneratorModule();

#endif