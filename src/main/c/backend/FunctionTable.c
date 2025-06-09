#include "FunctionTable.h"


typedef struct FunctionTable {
    struct hashmap * table; 
}FunctionTable;

typedef struct FunctionTableRow {
    char * functionName;
    int argumentCount;
} FunctionTableRow;




void _freeFunctionTableRow(void *row) {
    if (row == NULL) return;
    
    FunctionTableRow *functionTableRow = (FunctionTableRow *)row;
    
    free(functionTableRow->functionName);
}

int _compareFunctionTableRows(const void *a, const void *b, void *udata) {
    if (a == NULL || b == NULL) return 0;
    
    const FunctionTableRow *rowA = (const FunctionTableRow *)a;
    const FunctionTableRow *rowB = (const FunctionTableRow *)b;

    return strcmp(rowA->functionName, rowB->functionName);
}

uint64_t _hashFunctionTableRow(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct FunctionTableRow * row = item;
    return hashmap_sip(row->functionName, strlen(row->functionName), seed0, seed1);
}






FunctionTableType createFunctionTable() {
    FunctionTableType functionTable = malloc(sizeof(struct FunctionTable));
    if (functionTable == NULL) return NULL;
    
    functionTable->table = hashmap_new(sizeof(FunctionTableRow), 16, 0, 0, 
                                        _hashFunctionTableRow, 
                                        _compareFunctionTableRows, 
                                        _freeFunctionTableRow, NULL);
    if (functionTable->table == NULL) {
        free(functionTable);
        return NULL;
    }
    return functionTable;
}

int insertFunction(FunctionTableType functionTable, const char * functionName, ArgumentListType argumentList) {
    if (functionTable == NULL || functionName == NULL || argumentList == NULL || containsFunction(functionTable, functionName)) return 0;

    FunctionTableRow row;

    row.functionName = calloc(strlen(functionName) + 1, sizeof(char));
    if (row.functionName == NULL) return 0;


    strcpy((char *)row.functionName, functionName);

    row.argumentCount = getSize(argumentList);
    

    hashmap_set(functionTable->table, &row);
}

int containsFunction(FunctionTableType functionTable, const char * functionName) {
    if (functionTable == NULL || functionName == NULL) return 0;
    
    FunctionTableRow row;
    row.functionName = (char *)functionName;

    return hashmap_get(functionTable->table, &row) != NULL;
}

int getArgumentCount(FunctionTableType functionTable, const char * functionName) {
    if (functionTable == NULL || functionName == NULL) return -1;

    FunctionTableRow row;
    row.functionName = (char *)functionName;

    
    const FunctionTableRow * argumentList = hashmap_get(functionTable->table, &row);
    if (argumentList == NULL) return -1;
    
    return argumentList->argumentCount;
}   

void freeFunctionTable(FunctionTableType functionTable) {
    if (functionTable == NULL) {
        return;
    }
    hashmap_free(functionTable->table);
    free(functionTable);
}

