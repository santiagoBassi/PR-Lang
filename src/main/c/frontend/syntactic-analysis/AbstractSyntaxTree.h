#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef struct Program Program;
typedef struct Statements Statements;
typedef struct Statement Statement;
typedef struct Definition Definition;

typedef struct FunctionArgs FunctionArgs;

typedef struct DefinitionBody DefinitionBody;
typedef struct CompositionDef CompositionDef;

typedef struct RecursiveDef RecursiveDef;
typedef struct BaseCase BaseCase;
typedef struct NextCase NextCase;

typedef struct Expression Expression;
typedef struct FunctionExpression FunctionExpression;
typedef struct Factor Factor;
typedef struct ExpressionArgs ExpressionArgs;

typedef enum {
    DEFINITION,
    EXPRESSION
} StatementType;

typedef enum {
    COMPOSITION,
    RECURSIVE
} DefinitionType;

typedef enum {
    NO_ARGUMENTS,
    HAS_ARGUMENTS
} ArgumentsType;

typedef enum {
    FACTOR,
    FUNCTION
} TermType;

typedef enum {
    VAR,
    NUM,
    INPUT_TYPE
} FactorType;

typedef enum {
    VAR_ARG,
    NUM_ARG
} FunctionArgsType;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

struct Program {
	Statements* statements;
};

struct Statements {
    Statement* statement;
    Statements* statements;
};

struct Statement {
    union {
        Definition* definition;
        Expression* expression;
    };
    StatementType type;
};

struct Definition {
    const char* fun;

    union {
        struct {
            FunctionArgs* args;
            DefinitionBody* definitionBody;
        };
        CompositionDef* compositionDef;
    };

    ArgumentsType type;
};

struct FunctionArgs {
    union {
        const char* arg;
        int num;
    };
    FunctionArgsType type;
    FunctionArgs* args;
};

struct DefinitionBody {
    union {
        CompositionDef* compositionDef;
        RecursiveDef* recursiveDef;
    };

    DefinitionType type;
};

struct CompositionDef {
    const char* fun;
    FunctionArgs* args;
    Expression* expression;
};

struct Expression {
    union {
        FunctionExpression* functionExpression;
        Factor* factor;
    };

    TermType type;
};

struct FunctionExpression {
    const char* fun;
    ExpressionArgs* args;
};

struct Factor {
    union {
        const char* var;
        int num;
    };

    FactorType type;
};

struct ExpressionArgs {
    Expression* expression;
    ExpressionArgs* expressionArgs;
};

struct RecursiveDef {
    BaseCase* baseCase;
    NextCase* nextCase;
};

struct BaseCase {
    const char* fun;
    FunctionArgs* args;
    Expression* expression;
};

struct NextCase {
    const char* fun;

    FunctionArgs* args;

    const char* plus;
    int one;

    Expression* expression;
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program * program);
void releaseStatements(Statements * statements);
void releaseStatement(Statement * statement);

void releaseFunctionArgs(FunctionArgs* functionArgs);

void releaseDefinition(Definition * definition);
void releaseDefinitionBody(DefinitionBody* definitionBody);

void releaseExpression(Expression * expression);
void releaseFunctionExpression(FunctionExpression* functionExpression);
void releaseFactor(Factor* factor);
void releaseExpressionArgs(ExpressionArgs* expressionArgs);

void releaseCompositionDef(CompositionDef * def);
void releaseRecursiveDef(RecursiveDef * def);
void releaseBaseCase(BaseCase * baseCase);
void releaseNextCase(NextCase * nextCase);

#endif
