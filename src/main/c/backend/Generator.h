#ifndef GENERATOR_H
#define GENERATOR_H

#include "../shared/CompilerState.h"

void initializeGeneratorModule(CompilerState* compilerState);
void generateCode(CompilerState *compilerState);
void shutdownGeneratorModule();

#endif
