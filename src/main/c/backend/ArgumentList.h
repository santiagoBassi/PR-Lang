
#ifndef ARGUMENT_LIST_H
#define ARGUMENT_LIST_H

typedef struct ArgumentList * ArgumentListType;

ArgumentListType createArgumentList();

void insertArgumentList(ArgumentListType argumentList, const char * argument);

const char* nextArgument(ArgumentListType argumentList);

int hasNextArgumentList(ArgumentListType argumentList);

void resetArgumentList(ArgumentListType argumentList);

int containsArgument(ArgumentListType argumentList, const char * argument);

void freeArgumentList(ArgumentListType argumentList);

#endif