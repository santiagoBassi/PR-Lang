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

void insertArgument(ArgumentListType argumentList, const char * argument) {
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

boolean hasNextArgument(ArgumentListType argumentList) {
    if (argumentList == NULL) return false;
    return argumentList->current != NULL;
}

void resetArgumentList(ArgumentListType argumentList) {
    if (argumentList == NULL) return;
    argumentList->current = argumentList->arguments;
}

boolean containsArgument(ArgumentListType argumentList, const char * argument) {
    if (argumentList == NULL || argument == NULL) return false;

    Argument * current = argumentList->arguments;
    while (current != NULL) {
        if (strcmp(current->value, argument) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

const char* removeLastArgument(ArgumentListType argumentList) {
    if (argumentList == NULL) return NULL;

    Argument * current = argumentList->arguments;

    if (current->next == NULL) {
        const char* value = current->value;
        free(current);
        argumentList->arguments = NULL;
        argumentList->size = 0;
        argumentList->last = NULL;
        return value;
    }

    while (current->next != argumentList->last && current->next != NULL) {
        current = current->next;
    }

    const char* value = argumentList->last->value;
    free(argumentList->last);

    current->next = NULL;
    argumentList->last = current;

    return value;
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
