#include "Generator.h"
#include "../shared/Logger.h"
#include "../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "ArgumentList.h"
#include "FunctionTable.h"
#include <stdio.h>

static Logger * _logger = NULL;
FILE* _output_file = NULL;
FunctionTableType functionTable = NULL;

void initializeGeneratorModule() {
    functionTable = createFunctionTable();
    _output_file = stdout;
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
    if (functionTable != NULL) {
        freeFunctionTable(functionTable);
    }
}

int _validFunctionArgs(FunctionArgs* args, ArgumentListType argsList);
int _generateFunctionArgs(FunctionArgs* args);

int _generateRecursiveDef(RecursiveDef* def, ArgumentListType argsList);
int _generateBaseCase(BaseCase* baseCase, ArgumentListType argsList);
int _generateNextCase(NextCase* nextCase, ArgumentListType argsList);

int _generateExpression(Expression* expression);
int _generateFunctionExpression(FunctionExpression* functionExpression);
int _generateBinaryExpression(BinaryExpression* binaryExpression);
int _generateFactor(Factor* factor);
int _generateExpressionArgs(ExpressionArgs* expressionArgs);

int _generateExpressionInDefinition(Expression* expression, ArgumentListType argsList);
int _generateFunctionExpressionInDefinition(FunctionExpression* functionExpression, ArgumentListType argsList);
int _generateBinaryExpressionInDefinition(BinaryExpression* binaryExpression, ArgumentListType argsList);
int _generateFactorInDefinition(Factor* factor, ArgumentListType argsList);
int _generateExpressionArgsInDefinition(ExpressionArgs* expressionArgs, ArgumentListType argsList);

void _output(const char* const format, ...);

int _validFunctionArgs(FunctionArgs* args, ArgumentListType argsList) {
    for (FunctionArgs* arg = args; arg != NULL; arg = arg->args) {
        if (arg->type != VAR_ARG || !containsArgument(argsList, arg->arg)) {
            logError(_logger, "Argument %s is not defined in the current scope", arg->arg);
            return false;
        }
    }

    return true;
}

int _generateRecursiveDef(RecursiveDef* def, ArgumentListType argsList) {
    if (def == NULL) {
        return false;
    }

    return _generateBaseCase(def->baseCase, argsList) && _generateNextCase(def->nextCase, argsList);
}

int _validBaseCaseArgs(ArgumentListType argsList, FunctionArgs* args) {
    for (FunctionArgs* arg = args; arg != NULL; arg = arg->args) {
        if (arg->type == VAR_ARG && !containsArgument(argsList, arg->arg)) {
            logError(_logger, "Argument %s is not defined in the current scope", arg->arg);
            return false;
        } else if (arg->type == NUM_ARG && (arg->num != 0 || arg->args != NULL)) {
            logError(_logger, "Last argument of base case must be a zero. Got: %d", arg->num);
            return false;
        }
    }
    
    return true;
}

int _generateBaseCase(BaseCase* baseCase, ArgumentListType argsList) {
    if (baseCase == NULL || functionTable == NULL) {
        return false;
    }
    
    if (!containsFunction(functionTable, baseCase->fun)) {
        logError(_logger, "Error in base case: function name %s is not part of the definition", baseCase->fun);
        return false;
    }

    if (!_validBaseCaseArgs(argsList, baseCase->args)) {
        return false;
    }

    _output("if (%s == 0) {\n", lastArgument(argsList));
    int expressionStatus = _generateExpression(baseCase->expression);
    _output(";\n}\n\n");

    return expressionStatus;
}

int _generateNextCase(NextCase* nextCase, ArgumentListType argsList) {
    if (nextCase == NULL || functionTable == NULL) {
        return false;
    }

    if (!containsFunction(functionTable, nextCase->fun)) {
        logError(_logger, "Error in next case: function name %s is not part of the definition", nextCase->fun);
        return false;
    }

    if (!_validFunctionArgs(nextCase->args, argsList) || nextCase->one != 1 || !(nextCase->plus[0] == '+' && nextCase->plus[1] == '\0')) {
        logError(_logger, "Error in next case: bad args", nextCase->fun);
        return false;
    }

    _output("return ");
    int expressionStatus = _generateExpressionInDefinition(nextCase->expression, argsList);
    _output(";\n");

    return expressionStatus;
}

int _generateExpression(Expression* expression) {
    if (expression == NULL || functionTable == NULL) {
        return false;
    }

    switch (expression->type) {
        case FACTOR:
            return _generateFactor(expression->factor);
            break;
        case FUNCTION:
            return _generateFunctionExpression(expression->functionExpression);
            break;
        case BINARY:
            return _generateBinaryExpression(expression->binaryExpression);
    }
}

int _generateExpressionInDefinition(Expression* expression, ArgumentListType argsList) {
    if (expression == NULL || argsList == NULL) {
        return false;
    }

    switch (expression->type) {
        case FACTOR:
            return _generateFactorInDefinition(expression->factor, argsList);
            break;
        case FUNCTION:
            return _generateFunctionExpressionInDefinition(expression->functionExpression, argsList);
            break;
        case BINARY:
            return _generateBinaryExpressionInDefinition(expression->binaryExpression, argsList);
    }
}

int _generateFactor(Factor* factor) {
    if (factor == NULL) {
        return false;
    }

    switch (factor->type) {
        case VAR:
            logError(_logger, "Error in factor: can't use symbols in evaluations");
            return false;
        case NUM:
            _output(" %d ", factor->num);
            return true;
        case INPUT_TYPE:
            _output(" get_positive_integer_from_stdin() ");
            return false;
    }
}

int _generateFactorInDefinition(Factor* factor, ArgumentListType argsList) {
    if (factor == NULL || argsList == NULL) {
        return false;
    }

    switch (factor->type) {
        case VAR:
            if (!containsArgument(argsList, factor->var)) {
                logError(_logger, "Error in factor: %s is not defined", factor->var);
                return false;
            }
            _output(" %s ", factor->var);
            return true;
        case NUM:
            _output(" %d ", factor->num);
            return true;
        case INPUT_TYPE:
            logError(_logger, "Error in factor: can't use inputs in function definitions");
            return false;
    }
}

int _getExpressionArgsLen(ExpressionArgs* expressionArgs) {
    if (expressionArgs == NULL)
        return 0;

    return 1 + _getExpressionArgsLen(expressionArgs->expressionArgs);
}

int _generateFunctionExpression(FunctionExpression* expression) {
    if (expression == NULL || functionTable == NULL) {
        return false;
    }

    if (!containsFunction(functionTable, expression->fun)) {
        logError(_logger, "Error in expression: function %s is not defined", expression->fun);
        return false;
    }

    if (_getExpressionArgsLen(expression->args) != getArgumentCount(functionTable, expression->fun)) {
        logError(_logger, "Error in arguments: wrong argument count passed to function");
        return false;
    }

    _output("%s(", expression->fun);
    int expressionArgsStatus = _generateExpressionArgs(expression->args);
    _output(")");

    return expressionArgsStatus;
}

int _generateFunctionExpressionInDefinition(FunctionExpression* expression, ArgumentListType argsList) {
    if (expression == NULL || argsList == NULL || functionTable == NULL) {
        return false;
    }

    if (!containsFunction(functionTable, expression->fun)) {
        logError(_logger, "Error in expression: function %s is not defined", expression->fun);
        return false;
    }

    if (_getExpressionArgsLen(expression->args) != getSize(argsList)) {
        logError(_logger, "Error in arguments: wrong argument count passed to function");
        return false;
    }

    _output("%s(", expression->fun);
    int expressionArgsStatus = _generateExpressionArgsInDefinition(expression->args, argsList);
    _output(")");
    return expressionArgsStatus;
}

int _generateExpressionArgs(ExpressionArgs* expressionArgs) {
    if (expressionArgs == NULL || functionTable == NULL) {
        return false;
    }

    for (ExpressionArgs* arg = expressionArgs; arg != NULL; arg = arg->expressionArgs) {
        if (!_generateExpression(arg->expression)) {
            logError(_logger, "Error in expression arguments");
            return false;
        }
        _output(",");
    }

    return true;
}

int _generateExpressionArgsInDefinition(ExpressionArgs* expressionArgs, ArgumentListType argsList) {
    if (expressionArgs == NULL || argsList == NULL) {
        return false;
    }

    for (ExpressionArgs* arg = expressionArgs; arg != NULL; arg = arg->expressionArgs) {
        if (!_generateExpressionInDefinition(arg->expression, argsList)) {
            logError(_logger, "Error in expression arguments");
            return false;
        }
        _output(",");
    }

    return true;
}

int _generateBinaryExpression(BinaryExpression* expression) {
    if (expression == NULL || functionTable == NULL) {
        return false;
    }

    if (!containsFunction(functionTable, expression->fun)) {
        logError(_logger, "Error in binary expression: function %s is not defined", expression->fun);
        return false;
    }

    _output("%s(", expression->fun);
    int leftExpressionStatus = _generateExpression(expression->left);
    _output(",");
    int rightExpressionStatus = _generateExpression(expression->right);
    _output(")");

    return leftExpressionStatus && rightExpressionStatus;
}

int _generateBinaryExpressionInDefinition(BinaryExpression* expression, ArgumentListType argsList) {
    if (expression == NULL || functionTable == NULL || argsList == NULL) {
        return false;
    }

    if (!containsFunction(functionTable, expression->fun)) {
        logError(_logger, "Error in binary expression: function %s is not defined", expression->fun);
        return false;
    }

    _output("%s(", expression->fun);
    int leftExpressionStatus = _generateExpressionInDefinition(expression->left, argsList);
    _output(",");
    int rightExpressionStatus = _generateExpressionInDefinition(expression->right, argsList);
    _output(")");

    return leftExpressionStatus && rightExpressionStatus;
}

void _output(const char* const format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(_output_file, format, args);
    fflush(_output_file);

    va_end(args);
}
