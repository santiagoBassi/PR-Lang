#ifndef ARGUMENT_LIST_H
#define ARGUMENT_LIST_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct ArgumentList * ArgumentListType;

ArgumentListType createArgumentList();

void insertArgument(ArgumentListType argumentList, const char * argument);

const char* lastArgument(ArgumentListType argumentList);

const char* nextArgument(ArgumentListType argumentList);

int hasNextArgument(ArgumentListType argumentList);

void resetArgumentList(ArgumentListType argumentList);

int containsArgument(ArgumentListType argumentList, const char * argument);

void freeArgumentList(ArgumentListType argumentList);

int getSize(ArgumentListType argumentList);

#endif
