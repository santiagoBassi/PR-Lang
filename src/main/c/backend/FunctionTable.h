
#ifndef FUNCTION_TABLE_H
#define FUNCTION_TABLE_H

#include "ArgumentList.h"

typedef struct FunctionTable * FunctionTableType;

FunctionTableType createFunctionTable();

void insertFunction(FunctionTableType functionTable, const char * functionName, ArgumentListType argumentList);

int containsFunction(FunctionTableType functionTable, const char * functionName);

int getArgumentCount(FunctionTableType functionTable, const char * functionName);

void freeFunctionTable(FunctionTableType functionTable);

#endif