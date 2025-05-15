#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */
void releaseFactor(Factor *factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (factor != NULL) {
        switch (factor->type) {
            case VAR:
                free(factor->var);
                break;
            default:
                break;
        }
        free(factor);
    }
}

void releaseExpressionArgs(ExpressionArgs *expressionArgs) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (expressionArgs != NULL) {
        releaseExpressionArgs(expressionArgs->expressionArgs);
        releaseExpression(expressionArgs->expression);
        free(expressionArgs);
    }
}

void releaseFunctionExpression(FunctionExpression *functionExpression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (functionExpression != NULL) {
        releaseExpressionArgs(functionExpression->args);
        free(functionExpression->fun);
        free(functionExpression);
    }
}

void releaseExpression(Expression *expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (expression != NULL) {
        switch (expression->type) {
            case FACTOR:
                releaseFactor(expression->factor);
                break;
            case FUNCTION:
                releaseFunctionExpression(expression->functionExpression);
                break;
        }
        free(expression);
    }
}

void releaseNextCase(NextCase *nextCase) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (nextCase != NULL) {
        releaseExpression(nextCase->expression);
        releaseFunctionArgs(nextCase->args);
        free(nextCase->plus);
        free(nextCase->fun);
        free(nextCase);
    }
}

void releaseBaseCase(BaseCase *baseCase) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (baseCase != NULL) {
        releaseExpression(baseCase->expression);
        releaseFunctionArgs(baseCase->args);
        free(baseCase->fun);
        free(baseCase);
    }
}

void releaseRecursiveDef(RecursiveDef *def) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (def != NULL) {
        releaseBaseCase(def->baseCase);
        releaseNextCase(def->nextCase);
        free(def);
    }
}

void releaseCompositionDef(CompositionDef *def) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (def != NULL) {
        releaseFunctionArgs(def->args);
        releaseExpression(def->expression);
        free(def->fun);
        free(def);
    }
}

void releaseDefinitionBody(DefinitionBody *definitionBody) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (definitionBody != NULL) {
        switch (definitionBody->type) {
            case RECURSIVE:
                releaseRecursiveDef(definitionBody->recursiveDef);
                break;
            case COMPOSITION:
                releaseCompositionDef(definitionBody->compositionDef);
                break;
        }

        free(definitionBody);
    }
}

void releaseFunctionArgs(FunctionArgs *functionArgs) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (functionArgs != NULL) {
        releaseFunctionArgs(functionArgs->args);
        free(functionArgs->arg);
        free(functionArgs);
    }
}

void releaseDefinition(Definition *definition) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (definition != NULL) {
        if (definition->args != NULL) {
            releaseFunctionArgs(definition->args);
            releaseDefinitionBody(definition->definitionBody);
        } else {
            releaseCompositionDef(definition->compositionDef);
        }

        free(definition->fun);
        free(definition);
    }
}

void releaseStatement(Statement *statement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statement != NULL) {
        switch (statement->type) {
            case EXPRESSION: 
                releaseExpression(statement->expression);
                break;
            case DEFINITION:
                releaseDefinition(statement->definition);
                break;
        }
        free(statement);
    }
}

void releaseStatements(Statements *statements) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statements != NULL) {
        releaseStatement(statements->statement);
        releaseStatements(statements->statements);
        free(statements);
    }
}

void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseStatements(program->statements);
		free(program);
	}
}
