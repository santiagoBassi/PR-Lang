#include "Generator.h"
#include "../shared/Logger.h"
#include "../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "ArgumentList.h"
#include "FunctionTable.h"
#include <stdio.h>
#include <string.h>

static Logger * _logger = NULL;
FILE* _output_file = NULL;
FunctionTableType functionTable = NULL;

void _initializeSucInTable();

void initializeGeneratorModule() {
    functionTable = createFunctionTable();
    _output_file = stdout;
	_logger = createLogger("Generator");

    ArgumentListType sucArgs = createArgumentList();
    insertArgument(sucArgs, "x");
    insertFunction(functionTable, "suc", sucArgs);
    freeArgumentList(sucArgs);
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
    if (functionTable != NULL) {
        freeFunctionTable(functionTable);
    }
}

void _preamble();

int _generateProgram(Program* program);
int _generateStatements(Statements* statements);
int _generateStatement(Statement* statement);
int _generateDefinition(Definition* definition);

int _validFunctionArgs(FunctionArgs* args, ArgumentListType argsList);
int _generateFunctionArgs(FunctionArgs* args);

int _generateDefinitionBody(DefinitionBody* definitionBody, const char* functionName, ArgumentListType argsList);
int _generateCompositionDef(CompositionDef* compositionDef, const char* functionName, ArgumentListType argsList);

int _generateRecursiveDef(RecursiveDef* def, const char* functionName, ArgumentListType argsList);
int _generateBaseCase(BaseCase* baseCase, const char* functionName, ArgumentListType argsList);
int _generateNextCase(NextCase* nextCase, const char* functionName, ArgumentListType argsList);

int _generateExpression(Expression* expression);
int _generateFunctionExpression(FunctionExpression* functionExpression);
int _generateBinaryExpression(BinaryExpression* binaryExpression);
int _generateFactor(Factor* factor);
int _generateExpressionArgs(ExpressionArgs* expressionArgs);

int _generateExpressionInDefinition(Expression* expression, const char* functionName, ArgumentListType argsList);
int _generateFunctionExpressionInDefinition(FunctionExpression* functionExpression, const char* functionName, ArgumentListType argsList);
int _generateBinaryExpressionInDefinition(BinaryExpression* binaryExpression, const char* functionName, ArgumentListType argsList);
int _generateFactorInDefinition(Factor* factor, ArgumentListType argsList);
int _generateExpressionArgsInDefinition(ExpressionArgs* expressionArgs, const char* functionName, ArgumentListType argsList);

void _output(const char* const format, ...);

void generateCode(CompilerState* compilerState){
    if (compilerState == NULL) return;

    _preamble();
    compilerState->succeed = _generateProgram((Program*) compilerState->abstractSyntaxtTree);
}

void _preamble(){
    _output(
        "#include <stdio.h>\n"
        "int get_positive_integer_from_stdin(){\n"
        "int out = 0; scanf(\"%%d\", &out);\n"
        "return (out < 0 ? 0 : out);\n"
        "}\n"
    );
}

int _generateProgram(Program* program){
    if (program == NULL) return false;

    return _generateStatements(program->statements);
}

int _generateStatements(Statements* statements){
    if(statements == NULL) return false;

    for (Statements* statement = statements; statement != NULL; statement = statement->statements) {
        if (statement->statement->type != DEFINITION) continue;
        if (!_generateStatement(statement->statement)) {
            logError(_logger, "Exited because of invalid statement");
            return false;
        }
    }
    
    for (Statements* statement = statements; statement != NULL; statement = statement->statements) {
        if (statement->statement->type != EXPRESSION) continue; 
        if (!_generateStatement(statement->statement)) {
            logError(_logger, "Exited because of invalid statement");
            return false;
        }
    }

    return true;
}

int _generateStatement(Statement* statement){
    if (statement == NULL) return false;

    switch (statement->type) {
        case DEFINITION:
            return _generateDefinition(statement->definition);
        case EXPRESSION:
            return _generateExpression(statement->expression);
        default:
            logError(_logger, "Unknown expression type: %d", statement->type);
            return false;
    }
}

int _generateDefinition(Definition* definition){
    if (definition == NULL) return false;

    ArgumentListType argsList = createArgumentList();
    if (argsList == NULL) return false;

    for (FunctionArgs* arg = definition->args; arg != NULL; arg = arg->args) {
        insertArgument(argsList, arg->arg);
    }

    if (!insertFunction(functionTable, definition->fun, argsList)) {
        logError(_logger, "Error in function definition: could not create a function with name %s, make sure it's not already declared", definition->fun);
        freeArgumentList(argsList);
        return false;
    }

    _output("int %s(", definition->fun);
    if (!_generateFunctionArgs(definition->args)) {
        logError(_logger, "Invalid arguments error");
        freeArgumentList(argsList);
        return false;
    }
    _output("){\n");
    if (!_generateDefinitionBody(definition->definitionBody, definition->fun, argsList)) {
        logError(_logger, "Invalid definition body error");
        freeArgumentList(argsList);
        return false;
    }
    _output("}\n");

    freeArgumentList(argsList);

    return true;
}

int _validFunctionArgs(FunctionArgs* args, ArgumentListType argsList) {
    resetArgumentList(argsList);
    for (FunctionArgs* arg = args; arg != NULL; arg = arg->args) {
        if (!hasNextArgument(argsList)) {
            logError(_logger, "Invalid argument count");
            return false;
        }
        if (arg->type != VAR_ARG || strcmp(nextArgument(argsList), arg->arg) != 0) {
            logError(_logger, "Argument %s is not defined in the current scope", arg->arg);
            return false;
        }
    }

    return true;
}

int _generateFunctionArgs(FunctionArgs* args){
    for (FunctionArgs* arg = args; arg != NULL; arg = arg->args) {
        if (arg->type != VAR){
            logError(_logger, "Invalid variable type: %d", arg->type);
            return false;
        }
        _output("int %s", arg->arg);
        if (arg->args != NULL) _output(",");
    }
    return true;
}

int _generateDefinitionBody(DefinitionBody* definitionBody, const char* functionName, ArgumentListType argsList){
    if (definitionBody == NULL) return false;

    switch (definitionBody->type) {
        case COMPOSITION:
            return _generateCompositionDef(definitionBody->compositionDef, functionName, argsList);
        case RECURSIVE:
            return _generateRecursiveDef(definitionBody->recursiveDef, functionName, argsList);
        default:
            logError(_logger, "Unknown function definition type: %d", definitionBody->type);
            return false;
    }
}

int _generateCompositionDef(CompositionDef* compositionDef, const char* functionName, ArgumentListType argsList){
    if (compositionDef == NULL) return false;

    if (strcmp(compositionDef->fun, functionName) != 0) {
        logError(_logger, "Error in composition definition: function name %s is not part of the definition", compositionDef->fun);
        return false;
    }

    _output("return ");
    if (!_generateExpressionInDefinition(compositionDef->expression, functionName, argsList)) {
        logError(_logger, "Invalid composition definition error");
        return false;
    }
    _output(";\n");

    return true;
}

int _generateRecursiveDef(RecursiveDef* def, const char* functionName, ArgumentListType argsList) {
    if (def == NULL) return false;

    if (!_generateBaseCase(def->baseCase, functionName, argsList)) {
        logError(_logger, "Could not generate base case of recursive definition");
        return false;
    }

    const char* last = lastArgument(argsList);
    _output("%s--;\n\n", last);

    if (!_generateNextCase(def->nextCase, functionName, argsList)) {
        logError(_logger, "Could not generate next case of recursive definition");
        return false;
    }

    return true;
}

int _validBaseCaseArgs(ArgumentListType argsList, FunctionArgs* args) {
    for (FunctionArgs* arg = args; arg != NULL; arg = arg->args) {
        if (arg->type == VAR_ARG && (!containsArgument(argsList, arg->arg) || arg->args == NULL)) {
            logError(_logger, "Argument %s is not defined in the current scope", arg->arg);
            return false;
        }
        if (arg->type == NUM_ARG && (arg->num != 0 || arg->args != NULL)) {
            logError(_logger, "Last argument of base case must be a zero. Got: %d", arg->num);
            return false;
        }
    }
    
    return true;
}

int _generateBaseCase(BaseCase* baseCase, const char* functionName, ArgumentListType argsList) {
    if (baseCase == NULL || functionTable == NULL) return false;
    
    if (strcmp(baseCase->fun, functionName) != 0) {
        logError(_logger, "Error in base case: function name %s is not part of the definition", baseCase->fun);
        return false;
    }

    if (!_validBaseCaseArgs(argsList, baseCase->args)) {
        logError(_logger, "Error in base case: invalid args");
        return false;
    }

    _output("if (%s == 0) {\nreturn", lastArgument(argsList));
    int expressionStatus = _generateExpressionInDefinition(baseCase->expression, functionName, argsList);
    _output(";\n}\n\n");

    return expressionStatus;
}

int _generateNextCase(NextCase* nextCase, const char* functionName, ArgumentListType argsList) {
    if (nextCase == NULL || functionTable == NULL) return false;

    if (strcmp(nextCase->fun, functionName) != 0) {
        logError(_logger, "Error in next case: function name %s is not part of the definition", nextCase->fun);
        return false;
    }

    if (!_validFunctionArgs(nextCase->args, argsList) || nextCase->one != 1 || !(nextCase->plus[0] == '+' && nextCase->plus[1] == '\0')) {
        logError(_logger, "Error in next case: bad args", nextCase->fun);
        return false;
    }

    _output("return ");
    int expressionStatus = _generateExpressionInDefinition(nextCase->expression, functionName, argsList);
    _output(";\n");

    return expressionStatus;
}

int _generateExpression(Expression* expression) {
    if (expression == NULL || functionTable == NULL) return false;

    switch (expression->type) {
        case FACTOR:
            return _generateFactor(expression->factor);
        case FUNCTION:
            return _generateFunctionExpression(expression->functionExpression);
        case BINARY:
            return _generateBinaryExpression(expression->binaryExpression);
        default:
            logError(_logger, "Unknown expression type: %d", expression->type);
            return false;
    }
}

int _generateExpressionInDefinition(Expression* expression, const char* functionName, ArgumentListType argsList) {
    if (expression == NULL || argsList == NULL) return false;

    switch (expression->type) {
        case FACTOR:
            return _generateFactorInDefinition(expression->factor, argsList);
        case FUNCTION:
            return _generateFunctionExpressionInDefinition(expression->functionExpression, functionName, argsList);
        case BINARY:
            return _generateBinaryExpressionInDefinition(expression->binaryExpression, functionName, argsList);
        default:
            logError(_logger, "Unknown expression type: %d", expression->type);
            return false;
    }
}

int _generateFactor(Factor* factor) {
    if (factor == NULL) return false;

    switch (factor->type) {
        case VAR:
            logError(_logger, "Error in factor: can't use symbols in evaluations");
            return false;
        case NUM:
            _output(" %d ", factor->num);
            return true;
        case INPUT_TYPE:
            _output(" get_positive_integer_from_stdin() ");
            return true;
        default:
            logError(_logger, "Unknown factor type: %d", factor->type);
            return false;
    }
}

int _generateFactorInDefinition(Factor* factor, ArgumentListType argsList) {
    if (factor == NULL || argsList == NULL) return false;

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
    if (expressionArgs == NULL) return 0;

    return 1 + _getExpressionArgsLen(expressionArgs->expressionArgs);
}

int _generateFunctionExpression(FunctionExpression* expression) {
    if (expression == NULL || functionTable == NULL) return false;

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

int _validExpressionArgsInDefinition(ExpressionArgs* args, ArgumentListType argsList) {
    if (args == NULL) return false;

    resetArgumentList(argsList);
    for (ExpressionArgs* arg = args; arg != NULL; arg = arg->expressionArgs) {
        if (!hasNextArgument(argsList)) {
            logError(_logger, "Invalid argument count");
            return false;
        }
        Expression* exp = arg->expression;
        if (exp == NULL || exp->type != FACTOR || exp->factor->type != VAR_ARG || strcmp(nextArgument(argsList), exp->factor->var) != 0) {
            logError(_logger, "Invalid argument %s in definition", exp->factor->var);
            return false;
        }
    }

    return true;
}

int _generateFunctionExpressionInDefinition(FunctionExpression* expression, const char* functionName, ArgumentListType argsList) {
    if (expression == NULL || argsList == NULL || functionTable == NULL) return false;

    if (!containsFunction(functionTable, expression->fun)) {
        logError(_logger, "Error in expression: function %s is not defined", expression->fun);
        return false;
    }

    if (_getExpressionArgsLen(expression->args) != getArgumentCount(functionTable, expression->fun)) {
        logError(_logger, "Error in arguments: wrong argument count passed to function");
        return false;
    }

    if (strcmp(expression->fun, functionName) == 0 && !_validExpressionArgsInDefinition(expression->args, argsList)) {
        logError(_logger, "Violation of RP definition: the defined function can only appear in the definition with the correct parameters");
        return false;
    }

    _output("%s(", expression->fun);
    int expressionArgsStatus = _generateExpressionArgsInDefinition(expression->args, functionName, argsList);
    _output(")");
    return expressionArgsStatus;
}

int _generateExpressionArgs(ExpressionArgs* expressionArgs) {
    if (functionTable == NULL) return false;

    for (ExpressionArgs* arg = expressionArgs; arg != NULL; arg = arg->expressionArgs) {
        if (!_generateExpression(arg->expression)) {
            logError(_logger, "Error in expression arguments");
            return false;
        }
        if (arg->expressionArgs != NULL) _output(",");
    }

    return true;
}

int _generateExpressionArgsInDefinition(ExpressionArgs* expressionArgs, const char* functionName, ArgumentListType argsList) {
    if (argsList == NULL) return false;

    for (ExpressionArgs* arg = expressionArgs; arg != NULL; arg = arg->expressionArgs) {
        if (!_generateExpressionInDefinition(arg->expression, functionName, argsList)) {
            logError(_logger, "Error in expression arguments");
            return false;
        }
        if (arg->expressionArgs != NULL) _output(",");
    }

    return true;
}

int _generateBinaryExpression(BinaryExpression* expression) {
    if (expression == NULL || functionTable == NULL) return false;

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

int _generateBinaryExpressionInDefinition(BinaryExpression* expression, const char* functionName, ArgumentListType argsList) {
    if (expression == NULL || functionTable == NULL || argsList == NULL) return false;

    if (!containsFunction(functionTable, expression->fun)) {
        logError(_logger, "Error in binary expression: function %s is not defined", expression->fun);
        return false;
    }

    _output("%s(", expression->fun);
    int leftExpressionStatus = _generateExpressionInDefinition(expression->left, functionName, argsList);
    _output(",");
    int rightExpressionStatus = _generateExpressionInDefinition(expression->right, functionName, argsList);
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
