#include "ArgumentList.h"


typedef struct Argument {
    char * value;
    struct Argument * next;
} Argument;

typedef struct ArgumentList {
    Argument * arguments; 
    Argument * current;
    Argument * last; 
    int size;
} ArgumentList;


ArgumentListType createArgumentList() {
    ArgumentListType argumentList = calloc(1, sizeof(ArgumentList));
    if (argumentList == NULL) return NULL;

    return argumentList;
}

void insertArgumentList(ArgumentListType argumentList, const char * argument) {
    if (argumentList == NULL || argument == NULL) return;

    Argument * newArgument = calloc(1, sizeof(Argument));
    if (newArgument == NULL) return;

    newArgument->value = calloc(strlen(argument) + 1, sizeof(char));
    if (newArgument->value == NULL) {
        free(newArgument);
        return;     
    }
    strcpy(newArgument->value, argument);

    if (argumentList->arguments == NULL) {
        argumentList->arguments = newArgument;
        argumentList->current = newArgument;
        argumentList->last = newArgument;
    } else {
        argumentList->last->next = newArgument;
        argumentList->last = newArgument;
    }

    argumentList->size++;
}

const char* lastArgument(ArgumentListType argumentList) {
    if (argumentList == NULL || argumentList->last == NULL) return NULL;

    return argumentList->last->value;
}

const char* nextArgument(ArgumentListType argumentList) {
    if (argumentList == NULL || argumentList->current == NULL) return NULL;

    const char * value = argumentList->current->value;
    argumentList->current = argumentList->current->next;
    return value;
}

int hasNextArgumentList(ArgumentListType argumentList) {
    if (argumentList == NULL) return 0;
    return argumentList->current != NULL;
}

void resetArgumentList(ArgumentListType argumentList) {
    if (argumentList == NULL) return;
    argumentList->current = argumentList->arguments;
}

int containsArgument(ArgumentListType argumentList, const char * argument) {
    if (argumentList == NULL || argument == NULL) return 0;

    Argument * current = argumentList->arguments;
    while (current != NULL) {
        if (strcmp(current->value, argument) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void freeArgumentList(ArgumentListType argumentList) {
    if (argumentList == NULL) return;

    Argument * current = argumentList->arguments;
    while (current != NULL) {
        Argument * next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
    free(argumentList);
}

int getSize(ArgumentListType argumentList) {
    if (argumentList == NULL) return -1;
    return argumentList->size;
}
