#include "BisonActions.h"
#include "AbstractSyntaxTree.h"
#include <stdio.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */
DefinitionBody* CompositionDefBodySemanticAction(CompositionDef *def) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DefinitionBody* body = calloc(1, sizeof(DefinitionBody));
    body->type = COMPOSITION;
    body->compositionDef = def;
    return body;
}

DefinitionBody* RecursiveDefBodySemanticAction(RecursiveDef *def) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DefinitionBody* body = calloc(1, sizeof(DefinitionBody));
    body->type = RECURSIVE;
    body->recursiveDef = def;
    return body;
}

CompositionDef* CompositionDefSemanticAction(const char *fun, FunctionArgs *args, Expression *expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CompositionDef* def = calloc(1, sizeof(CompositionDef));
    def->fun = fun;
    def->args = args;
    def->expression = expression;
    return def;
}

RecursiveDef* RecursiveDefSemanticAction(BaseCase *baseCase, NextCase *nextCase) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    RecursiveDef* def = calloc(1, sizeof(RecursiveDef));
    def->baseCase = baseCase;
    def->nextCase = nextCase;
    return def;
}

BaseCase* BaseCaseSemanticAction(const char *fun, FunctionArgs *args, Expression *expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    BaseCase* baseCase = calloc(1, sizeof(BaseCase));
    baseCase->fun = fun;
    baseCase->args = args;
    baseCase->expression = expression;
    return baseCase;
}

NextCase* NextCaseSemanticAction(const char *fun, FunctionArgs *args, const char *plus, const int one, Expression *expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    NextCase* nextCase = calloc(1, sizeof(NextCase));
    nextCase->fun = fun;
    nextCase->args = args;
    nextCase->plus = plus;
    nextCase->one = one;
    nextCase->expression = expression;
    return nextCase;
}


Definition* DefinitionNoArgsSemanticAction(const char* fun, CompositionDef* def) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition* definition = calloc(1, sizeof(Definition));
	definition->fun = fun;
	definition->compositionDef = def;
	definition->type = NO_ARGUMENTS;
	
	return definition;
}

Definition* DefinitionSemanticAction(const char* fun, FunctionArgs* args, DefinitionBody* body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Definition* definition = calloc(1, sizeof(Definition));
	definition->fun = fun;
	definition->args = args;
	definition->definitionBody = body;
	definition->type = HAS_ARGUMENTS;
	
	return definition;
}

FunctionArgs* FunctionArgsSemanticAction(const char* id, FunctionArgs* args) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FunctionArgs* functionArgs = calloc(1, sizeof(FunctionArgs));
    functionArgs->type = VAR_ARG;
	functionArgs->arg = id;
	functionArgs->args = args;
	return functionArgs;
}

FunctionArgs* FunctionArgsIntSemanticAction(int num) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FunctionArgs* functionArgs = calloc(1, sizeof(FunctionArgs));
    functionArgs->type = NUM_ARG;
	functionArgs->num = num;
	return functionArgs;
}

Statement* DefinitionStatementSemanticAction(Definition* definition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement* statement = calloc(1, sizeof(Statement));
	statement->definition = definition;
	statement->type = DEFINITION;
	return statement;
}

Statements* StatementsSemanticAction(Statements* statements, Statement* statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statements* newStatements = calloc(1, sizeof(Statements));
	newStatements->statement = statement;
	newStatements->statements = statements;
	return newStatements;
}

Program * ProgramSemanticAction(CompilerState * compilerState, Statements* statements) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->statements = statements;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext()) {
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else {
		compilerState->succeed = true;
	}
	return program;
}

Factor* IntegerFactorSemanticAction(const int value){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Factor* factor = calloc(1, sizeof(Factor));
    factor->num = value;
    factor->type = NUM;
    return factor;
}

Factor* InputFactorSemanticAction(){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Factor* factor = calloc(1, sizeof(Factor));
    factor->type = INPUT_TYPE;
    return factor;
}

Factor* IdFactorSemanticAction(const char* id){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Factor* factor = calloc(1, sizeof(Factor));
    factor->var = id;
    factor->type = VAR;
    return factor;
}

ExpressionArgs* ExpressionArgsSemanticAction(ExpressionArgs* args, Expression* expression){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    ExpressionArgs* expressionArgs = calloc(1, sizeof(ExpressionArgs));
    expressionArgs->expression = expression;
    expressionArgs->expressionArgs = args;
    return expressionArgs;
}

Expression* FactorExpressionSemanticAction(Factor* factor){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* expression = calloc(1, sizeof(Expression));
    expression->factor = factor;
    expression->type = FACTOR;
    return expression;
}

Expression* FunctionExpressionSemanticAction(FunctionExpression* functionExpression){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* expression = calloc(1, sizeof(Expression));
    expression->functionExpression = functionExpression;
    expression->type = FUNCTION;
    return expression;
}

Expression* BinaryExpressionSemanticAction(BinaryExpression* binaryExpression){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* expression = calloc(1, sizeof(Expression));
    expression->binaryExpression = binaryExpression;
    expression->type = BINARY;
    return expression;
}

FunctionExpression* FunctionSemanticAction(const char* fun, ExpressionArgs* args){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    FunctionExpression* functionExpression = calloc(1, sizeof(FunctionExpression));
    functionExpression->fun = fun;
    functionExpression->args = args;
    return functionExpression;
}

BinaryExpression* BinaryExpressionBodySemanticAction(const char* fun, Expression* leftExpression, Expression* rightExpression){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    BinaryExpression* binaryExpression = calloc(1, sizeof(BinaryExpression));
    binaryExpression->fun = fun;
    binaryExpression->left = leftExpression;
    binaryExpression->right = rightExpression;
    return binaryExpression;
}

Statement* ExpressionStatementSemanticAction(Expression* expression){
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->expression = expression;
    statement->type = EXPRESSION;
    return statement;
}
