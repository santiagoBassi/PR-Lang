#include "ASTPrinter.h"
#include "AbstractSyntaxTree.h"
#include <stdio.h>

void printIndent(int indent) {
    for (int i = 0; i < indent; i++) putchar(' ');
}

void printProgram(Program *program) {
    puts("Program:");
    printStatements(program->statements, 1);
}

void printStatements(Statements *statements, int indent) {
    if (statements == NULL) return;

    printIndent(indent);
    puts("Statements:");
    printStatement(statements->statement, indent + 1);
    printStatements(statements->statements, indent);
}

void printStatement(Statement *statement, int indent) {
    printIndent(indent);
    switch (statement->type) {
        case DEFINITION:
            puts("Statement (def):");
            printDefinition(statement->definition, indent + 1);
            break;
        case EXPRESSION:
            puts("Statement (expr):");
            printExpression(statement->expression, indent + 1);
            break;
    }
}

void printDefinition(Definition *definition, int indent) {
    printIndent(indent);
    printf("Definition %s:\n", definition->fun);
    printFunctionArgs(definition->args, indent + 1);
    printDefinitionBody(definition->definitionBody, indent + 1);
}

void printFunctionArgs(FunctionArgs *args, int indent) {
    if (args == NULL) return;
    printIndent(indent);
    switch (args->type) {
        case VAR_ARG:
            printf("Arg (var): %s\n", args->arg);
            break;
        case NUM_ARG:
            printf("Arg (num): %d\n", args->num);
            break;
    }
    printFunctionArgs(args->args, indent);
}

void printDefinitionBody(DefinitionBody *body, int indent) {
    switch (body->type) {
        case COMPOSITION:
            printCompositionDef(body->compositionDef, indent);
            break;
        case RECURSIVE:
            printRecursiveDef(body->recursiveDef, indent);
            break;
    }
}

void printCompositionDef(CompositionDef* comp, int indent) {
    printIndent(indent);
    printf("CompositionDef: %s\n", comp->fun);
    printFunctionArgs(comp->args, indent + 1);
    printExpression(comp->expression, indent + 1);
}

void printRecursiveDef(RecursiveDef* rec, int indent) {
    printIndent(indent);
    printf("RecursiveDef:\n");
    printBaseCase(rec->baseCase, indent + 1);
    printNextCase(rec->nextCase, indent + 1);
}

void printBaseCase(BaseCase* base, int indent) {
    printIndent(indent);
    printf("BaseCase: %s\n", base->fun);
    printFunctionArgs(base->args, indent + 1);
    printExpression(base->expression, indent + 1);
}

void printNextCase(NextCase* next, int indent) {
    printIndent(indent);
    printf("NextCase: %s last_arg(%s, %d)\n", next->fun, next->plus, next->one);
    printFunctionArgs(next->args, indent + 1);
    printExpression(next->expression, indent + 1);
}

void printExpression(Expression* expr, int indent) {
    if (!expr) return;
    switch (expr->type) {
        case FACTOR:
            printFactor(expr->factor, indent);
            break;
        case FUNCTION:
            printFunctionExpression(expr->functionExpression, indent);
            break;
        case BINARY:
            printBinaryExpression(expr->binaryExpression, indent);
            break;
    }
}

void printFunctionExpression(FunctionExpression* funcExpr, int indent) {
    printIndent(indent);
    printf("FunctionExpression: %s\n", funcExpr->fun);
    printExpressionArgs(funcExpr->args, indent + 1);
}

void printBinaryExpression(BinaryExpression* binExpr, int indent) {
    printIndent(indent);
    printf("BinaryExpression: %s\n", binExpr->fun);
    printExpression(binExpr->left, indent + 1);
    printExpression(binExpr->right, indent + 1);
}

void printFactor(Factor* factor, int indent) {
    printIndent(indent);
    switch (factor->type) {
        case VAR:
            printf("Factor (VAR): %s\n", factor->var);
            break;
        case NUM:
            printf("Factor (NUM): %d\n", factor->num);
            break;
        case INPUT_TYPE:
            printf("Factor (INPUT_TYPE): %s\n", factor->var);
            break;
    }
}

void printExpressionArgs(ExpressionArgs* args, int indent) {
    if (!args) return;
    printExpression(args->expression, indent);
    printExpressionArgs(args->expressionArgs, indent);
}
