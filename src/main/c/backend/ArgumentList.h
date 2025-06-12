#ifndef ARGUMENT_LIST_H
#define ARGUMENT_LIST_H

#include "../shared/Type.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct ArgumentList * ArgumentListType;

ArgumentListType createArgumentList();

void insertArgument(ArgumentListType argumentList, const char * argument);

const char* lastArgument(ArgumentListType argumentList);

const char* nextArgument(ArgumentListType argumentList);

boolean hasNextArgument(ArgumentListType argumentList);

void resetArgumentList(ArgumentListType argumentList);

boolean containsArgument(ArgumentListType argumentList, const char * argument);

const char* removeLastArgument(ArgumentListType argumentList);

void freeArgumentList(ArgumentListType argumentList);

int getSize(ArgumentListType argumentList);

#endif
