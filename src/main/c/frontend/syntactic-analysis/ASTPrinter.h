#ifndef PRINT_AST_HEADER
#define PRINT_AST_HEADER

#include "AbstractSyntaxTree.h"

void printProgram(Program* program);
void printStatements(Statements* statements, int indent);
void printStatement(Statement* statement, int indent);
void printDefinition(Definition* definition, int indent);
void printFunctionArgs(FunctionArgs* args, int indent);
void printDefinitionBody(DefinitionBody* body, int indent);
void printCompositionDef(CompositionDef* comp, int indent);
void printRecursiveDef(RecursiveDef* rec, int indent);
void printBaseCase(BaseCase* base, int indent);
void printNextCase(NextCase* next, int indent);
void printExpression(Expression* expr, int indent);
void printFunctionExpression(FunctionExpression* funcExpr, int indent);
void printBinaryExpression(BinaryExpression* binExpr, int indent);
void printFactor(Factor* factor, int indent);
void printExpressionArgs(ExpressionArgs* args, int indent);

#endif
