#include "BisonActions.h"
#include "AbstractSyntaxTree.h"
#include <cstdio>

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

BaseCase* BaseCaseSemanticAction(const char *fun, FunctionArgs *args, const int zero, Expression *expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    BaseCase* baseCase = calloc(1, sizeof(BaseCase));
    baseCase->fun = fun;
    baseCase->args = args;
    baseCase->zero = zero;
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
