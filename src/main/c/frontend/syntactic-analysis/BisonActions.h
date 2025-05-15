#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

// Gogost
Factor* IntegerFactorSemanticAction(const int value);
Factor* InputFactorSemanticAction();
Factor* IdFactorSemanticAction(const char* id);

ExpressionArgs* ExpressionArgsSemanticActoin(ExpressionArgs* args, Expression* expression);

Expression* FactorExpressionSemanticAction(Factor* factor);
Expression* FunctionExpressionSemanticAction(FunctionExpression* function_expression);

Statement* EvaluationStatementSemanticAction(Expression* expression);

// Neichdoggy dog
NextCase* NextCaseSemanticAction(const char* fun, FunctionArgs* args, const char* last_id, const int last_int, Expression* expression);
BaseCase* BaseCaseSemanticAction(const char* fun, FunctionArgs* args, const int last, Expression* expression);
RecursiveDef* RecursiveDefSemanticAction(BaseCase* base_case, NextCase* next_case);

CompositionDef* CompositionDefSemanticAction(const char* fun, FunctionArgs* args, Expression* expression);

DefinitionBody* RecursiveDefBodySemanticAction(RecursiveDef* def);
DefinitionBody* CompositionDefBodySemanticAction(CompositionDef* def);

// Bass
Definition* DefinitionNoArgsSemanticAction(const char* fun, FunctionArgs* args, CompositionDef* def);
Definition* DefinitionSemanticAction(const char* fun, FunctionArgs* args, DefinitionBody* body);

FunctionArgs* FunctionArgsSemanticAction(const char* id, FunctionArgs* args);

Statement* DefinitionStatementSemanticAction(Definition* definition);

Statements* StatementsSemanticAction(Statements* statements, Statement* statement);
Program * ProgramSemanticAction(CompilerState * compiler_state, Statements* statements);

#endif
