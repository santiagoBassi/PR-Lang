#include "Generator.h"
#include "../shared/Logger.h"
#include "../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "ArgumentList.h"
#include "FunctionTable.h"
#include <stdio.h>
#include <string.h>

static Logger * _logger = NULL;
static FILE* _output_file = NULL;
static FunctionTableType _function_table = NULL;

void _initializeSucInTable();

void initializeGeneratorModule(CompilerState* compilerState) {
    _output_file = stdout;
	_logger = createLogger("Generator");

    ArgumentListType sucArgs = createArgumentList();
    insertArgument(sucArgs, "x");
    insertFunction(compilerState->functionTable, "suc", sucArgs);
    freeArgumentList(sucArgs);
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

void _preamble();

boolean _generateProgram(Program* program);
boolean _generateStatements(Statements* statements);
boolean _generateStatement(Statement* statement);
boolean _generateDefinition(Definition* definition);

boolean _validFunctionArgs(FunctionArgs* args, ArgumentListType argsList);
boolean _generateFunctionArgs(FunctionArgs* args);

boolean _generateDefinitionBody(DefinitionBody* definitionBody, const char* functionName, ArgumentListType argsList);
boolean _generateCompositionDef(CompositionDef* compositionDef, const char* functionName, ArgumentListType argsList);

boolean _generateRecursiveDef(RecursiveDef* def, const char* functionName, ArgumentListType argsList);
boolean _generateBaseCase(BaseCase* baseCase, const char* functionName, ArgumentListType argsList);
boolean _generateNextCase(NextCase* nextCase, const char* functionName, ArgumentListType argsList);

boolean _generateEvaluation(Expression* expression);
boolean _generateExpression(Expression* expression, boolean isSemanticRepresentation);
boolean _generateFunctionExpression(FunctionExpression* functionExpression, boolean isSemanticRepresentation);
boolean _generateBinaryExpression(BinaryExpression* binaryExpression, boolean isSemanticRepresentation);
boolean _generateFactor(Factor* factor, boolean isSemanticRepresentation);
boolean _generateExpressionArgs(ExpressionArgs* expressionArgs, boolean isSemanticRepresentation);

boolean _generateExpressionInDefinition(Expression* expression, const char* functionName, ArgumentListType argsList);
boolean _generateFunctionExpressionInDefinition(FunctionExpression* functionExpression, const char* functionName, ArgumentListType argsList);
boolean _generateBinaryExpressionInDefinition(BinaryExpression* binaryExpression, const char* functionName, ArgumentListType argsList);
boolean _generateFactorInDefinition(Factor* factor, ArgumentListType argsList);
boolean _generateExpressionArgsInDefinition(ExpressionArgs* expressionArgs, const char* functionName, ArgumentListType argsList);

void _output(const char* const format, ...);

void generateCode(CompilerState* compilerState){
    if (compilerState == NULL) return;
    _function_table = compilerState->functionTable;

    FILE* file = fopen(compilerState->outputFile, "w");
    if (file == NULL) {
        logError(_logger, "Could not open output file: %s", compilerState->outputFile);
        compilerState->succeed = false;
    }

    _output_file = file;

    _preamble();
    compilerState->succeed = _generateProgram((Program*) compilerState->abstractSyntaxtTree);

    if (!compilerState->succeed) {
       remove(compilerState->outputFile); 
    }

    fclose(file);
}

void _preamble(){
    _output(
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "int get_positive_integer_from_stdin(){\n"
        "static int input_counter = 1;\n"
        "printf(\"Enter integer for program to use as %%d%%s input: \", input_counter, \n"
        "input_counter == 1 ? \"st\" : "
        "input_counter == 2 ? \"nd\" : "
        "input_counter == 3 ? \"rd\" : \"th\");\n"
        "input_counter++;"
        "int out = 0;\n"
        "int invalid = scanf(\"%%d\", &out);\n"
        "if (invalid != 1) {printf(\"Invalid non input integer, ending program\\n\"); exit(1);}\n"
        "return out;\n"
        "}\n"
        "int function0(int n){\n"
        "return n + 1;\n"
        "}\n"
    );
}

boolean _generateProgram(Program* program){
    if (program == NULL) return false;

    return _generateStatements(program->statements);
}

boolean _generateStatements(Statements* statements){
    if(statements == NULL) return false;

    for (Statements* statement = statements; statement != NULL; statement = statement->statements) {
        if (statement->statement->type != DEFINITION) continue;
        if (!_generateStatement(statement->statement)) {
            logError(_logger, "Exited because of invalid statement");
            return false;
        }
    }
    
    _output("int main() {\n");

    for (Statements* statement = statements; statement != NULL; statement = statement->statements) {
        if (statement->statement->type != EXPRESSION) continue; 
        if (!_generateStatement(statement->statement)) {
            logError(_logger, "Exited because of invalid statement");
            return false;
        }
    }
    
    _output(
        "return 0;\n"
        "}\n"
    );

    return true;
}

boolean _generateStatement(Statement* statement){
    if (statement == NULL) return false;

    switch (statement->type) {
        case DEFINITION:
            return _generateDefinition(statement->definition);
        case EXPRESSION:
            return _generateEvaluation(statement->expression);
        default:
            logError(_logger, "Unknown expression type: %d", statement->type);
            return false;
    }
}

boolean _generateDefinition(Definition* definition){
    if (definition == NULL) return false;

    ArgumentListType argsList = createArgumentList();
    if (argsList == NULL) return false;

    for (FunctionArgs* arg = definition->args; arg != NULL; arg = arg->args) {
        insertArgument(argsList, arg->arg);
    }

    if (!insertFunction(_function_table, definition->fun, argsList)) {
        logError(_logger, "Error in function definition: could not create a function with name %s, make sure it's not already declared", definition->fun);
        freeArgumentList(argsList);
        return false;
    }

    _output("int %s(", getFunNameForGeneratedCode(_function_table, definition->fun));
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

boolean _validFunctionArgs(FunctionArgs* args, ArgumentListType argsList) {
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

boolean _generateFunctionArgs(FunctionArgs* args){
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

boolean _generateDefinitionBody(DefinitionBody* definitionBody, const char* functionName, ArgumentListType argsList){
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

boolean _generateCompositionDef(CompositionDef* compositionDef, const char* functionName, ArgumentListType argsList){
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

boolean _generateRecursiveDef(RecursiveDef* def, const char* functionName, ArgumentListType argsList) {
    if (def == NULL) return false;

    const char* last = removeLastArgument(argsList); // x_n = 0 => base case can't use last arg
    
    _output("if (%s == 0) {\n", last);
    if (!_generateBaseCase(def->baseCase, functionName, argsList)) {
        logError(_logger, "Could not generate base case of recursive definition");
        free((void*)last);
        return false;
    }
    _output(";\n}\n\n");

    insertArgument(argsList, last);

    _output("%s--;\n\n", last);

    if (!_generateNextCase(def->nextCase, functionName, argsList)) {
        logError(_logger, "Could not generate next case of recursive definition");
        free((void*)last);
        return false;
    }

    free((void*)last);

    return true;
}

boolean _validBaseCaseArgs(ArgumentListType argsList, FunctionArgs* args) {
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

boolean _generateBaseCase(BaseCase* baseCase, const char* functionName, ArgumentListType argsList) {
    if (baseCase == NULL || _function_table == NULL) return false;
    
    if (strcmp(baseCase->fun, functionName) != 0) {
        logError(_logger, "Error in base case: function name %s is not part of the definition", baseCase->fun);
        return false;
    }

    if (!_validBaseCaseArgs(argsList, baseCase->args)) {
        logError(_logger, "Error in base case: invalid args");
        return false;
    }

    _output("return", lastArgument(argsList));
    return _generateExpressionInDefinition(baseCase->expression, functionName, argsList);
}

boolean _generateNextCase(NextCase* nextCase, const char* functionName, ArgumentListType argsList) {
    if (nextCase == NULL || _function_table == NULL) return false;

    if (strcmp(nextCase->fun, functionName) != 0) {
        logError(_logger, "Error in next case: function name %s is not part of the definition", nextCase->fun);
        return false;
    }

    if (!_validFunctionArgs(nextCase->args, argsList) || nextCase->one != 1 || !(nextCase->plus[0] == '+' && nextCase->plus[1] == '\0')) {
        logError(_logger, "Error in next case: bad args", nextCase->fun);
        return false;
    }

    _output("return ");
    boolean expressionStatus = _generateExpressionInDefinition(nextCase->expression, functionName, argsList);
    _output(";\n");

    return expressionStatus;
}

boolean _generateEvaluation(Expression* expression) {
    _output("printf(\"");

    if(!_generateExpression(expression, false)) {
        logError(_logger, "Error during evaluation");
        return false;
    }

    _output(" = %%d\\n\", ");

    if (!_generateExpression(expression, true)) {
        logError(_logger, "Error during evaluation");
        return false;
    }

    _output(");\n");

    return true;
}

boolean _generateExpression(Expression* expression, boolean isSemanticRepresentation) {
    if (expression == NULL || _function_table == NULL) return false;

    switch (expression->type) {
        case FACTOR:
            return _generateFactor(expression->factor, isSemanticRepresentation);
        case FUNCTION:
            return _generateFunctionExpression(expression->functionExpression, isSemanticRepresentation);
        case BINARY:
            return _generateBinaryExpression(expression->binaryExpression, isSemanticRepresentation);
        default:
            logError(_logger, "Unknown expression type: %d", expression->type);
            return false;
    }
}

boolean _generateExpressionInDefinition(Expression* expression, const char* functionName, ArgumentListType argsList) {
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

boolean _generateFactor(Factor* factor, boolean isSemanticRepresentation) {
    if (factor == NULL) return false;

    switch (factor->type) {
        case VAR:
            logError(_logger, "Error in factor: can't use symbols in evaluations");
            return false;
        case NUM:
            _output("%d", factor->num);
            return true;
        case INPUT_TYPE:
            if (isSemanticRepresentation) {
                _output("get_positive_integer_from_stdin()");
            } else {
                _output("input");
            }
            return true;
        default:
            logError(_logger, "Unknown factor type: %d", factor->type);
            return false;
    }
}

boolean _generateFactorInDefinition(Factor* factor, ArgumentListType argsList) {
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

boolean _generateFunctionExpression(FunctionExpression* expression, boolean isSemanticRepresentation) {
    if (expression == NULL || _function_table == NULL) return false;

    if (!containsFunction(_function_table, expression->fun)) {
        logError(_logger, "Error in expression: function %s is not defined", expression->fun);
        return false;
    }

    if (_getExpressionArgsLen(expression->args) != getArgumentCount(_function_table, expression->fun)) {
        logError(_logger, "Error in arguments: wrong argument count passed to function");
        return false;
    }

    if (isSemanticRepresentation) {
        _output("%s(", getFunNameForGeneratedCode(_function_table, expression->fun));
    } else {
        _output("%s(", expression->fun);
    }

    boolean expressionArgsStatus = _generateExpressionArgs(expression->args, isSemanticRepresentation);
    _output(")");

    return expressionArgsStatus;
}

boolean _validExpressionArgsInDefinition(ExpressionArgs* args, ArgumentListType argsList) {
    if (args == NULL) return false;

    resetArgumentList(argsList);
    for (ExpressionArgs* arg = args; arg != NULL; arg = arg->expressionArgs) {
        if (!hasNextArgument(argsList)) {
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

boolean _generateFunctionExpressionInDefinition(FunctionExpression* expression, const char* functionName, ArgumentListType argsList) {
    if (expression == NULL || argsList == NULL || _function_table == NULL) return false;

    if (!containsFunction(_function_table, expression->fun)) {
        logError(_logger, "Error in expression: function %s is not defined", expression->fun);
        return false;
    }

    if (_getExpressionArgsLen(expression->args) != getArgumentCount(_function_table, expression->fun)) {
        logError(_logger, "Error in arguments: wrong argument count passed to function");
        return false;
    }

    if (strcmp(expression->fun, functionName) == 0 && !_validExpressionArgsInDefinition(expression->args, argsList)) {
        logError(_logger, "Violation of RP definition: the defined function can only appear in the definition with the correct parameters");
        return false;
    }

    _output("%s(", getFunNameForGeneratedCode(_function_table,expression->fun));
    boolean expressionArgsStatus = _generateExpressionArgsInDefinition(expression->args, functionName, argsList);
    _output(")");
    return expressionArgsStatus;
}

boolean _generateExpressionArgs(ExpressionArgs* expressionArgs, boolean isSemanticRepresentation) {
    if (_function_table == NULL) return false;

    for (ExpressionArgs* arg = expressionArgs; arg != NULL; arg = arg->expressionArgs) {
        if (!_generateExpression(arg->expression, isSemanticRepresentation)) {
            logError(_logger, "Error in expression arguments");
            return false;
        }
        if (arg->expressionArgs != NULL) _output(",");
    }

    return true;
}

boolean _generateExpressionArgsInDefinition(ExpressionArgs* expressionArgs, const char* functionName, ArgumentListType argsList) {
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

boolean _generateBinaryExpression(BinaryExpression* expression, boolean isSemanticRepresentation) {
    if (expression == NULL || _function_table == NULL) return false;

    if (!containsFunction(_function_table, expression->fun)) {
        logError(_logger, "Error in binary expression: function %s is not defined", expression->fun);
        return false;
    }

    if (isSemanticRepresentation) {
        _output("%s(", getFunNameForGeneratedCode(_function_table, expression->fun));
    } else {
        _output("%s(", expression->fun);
    }

    boolean leftExpressionStatus = _generateExpression(expression->left, isSemanticRepresentation);
    _output(",");
    boolean rightExpressionStatus = _generateExpression(expression->right, isSemanticRepresentation);
    _output(")");

    return leftExpressionStatus && rightExpressionStatus;
}

boolean _generateBinaryExpressionInDefinition(BinaryExpression* expression, const char* functionName, ArgumentListType argsList) {
    if (expression == NULL || _function_table == NULL || argsList == NULL) return false;

    if (!containsFunction(_function_table, expression->fun)) {
        logError(_logger, "Error in binary expression: function %s is not defined", expression->fun);
        return false;
    }

    _output("%s(", getFunNameForGeneratedCode(_function_table, expression->fun));
    boolean leftExpressionStatus = _generateExpressionInDefinition(expression->left, functionName, argsList);
    _output(",");
    boolean rightExpressionStatus = _generateExpressionInDefinition(expression->right, functionName, argsList);
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
