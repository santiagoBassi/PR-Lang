
#ifndef FUNCTION_TABLE_H
#define FUNCTION_TABLE_H

#include "ArgumentList.h"
#include <stdint.h>
#include "HashMap.h"
#include <stdio.h>
#include <ctype.h>

typedef struct FunctionTable * FunctionTableType;

FunctionTableType createFunctionTable();

boolean insertFunction(FunctionTableType functionTable, const char * functionName, ArgumentListType argumentList);

boolean containsFunction(FunctionTableType functionTable, const char * functionName);

int getArgumentCount(FunctionTableType functionTable, const char * functionName);

void freeFunctionTable(FunctionTableType functionTable);

char * getFunNameForGeneratedCode(FunctionTableType functionTable, const char * functionName);

#endif
