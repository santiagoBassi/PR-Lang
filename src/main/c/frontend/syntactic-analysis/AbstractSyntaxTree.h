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

typedef struct Evaluation Evaluation;
typedef struct FunctionEvaluation FunctionEvaluation;
typedef struct EvaluationFactor EvaluationFactor;
typedef struct EvaluationArgs EvaluationArgs;

typedef struct FunctionArgs FunctionArgs;

typedef struct DefinitionBody DefinitionBody;
typedef struct CompositionDef CompositionDef;

typedef struct Expression Expression;
typedef struct FunctionExpression FunctionExpression;
typedef struct ExpressionFactor ExpressionFactor;
typedef struct ExpressionArgs ExpressionArgs;
typedef struct RecursiveDef RecursiveDef;

typedef struct BaseCase BaseCase;
typedef struct NextCase NextCase;

typedef enum {
    DEFINITION,
    EVALUATION
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
    INPUT
} FactorType;

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
        Evaluation* evaluation;
    };
    StatementType type;
};

struct Definition {
    char* fun;

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
    char* arg;
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
    char* fun;
    FunctionArgs* args;
    Expression* expression;
};

struct Expression {
    union {
        FunctionExpression* functionExpression;
        ExpressionFactor* expressionFactor;
    };

    TermType type;
};

struct FunctionExpression {
    char* fun;
    ExpressionArgs* expressionArgs;
};

struct ExpressionFactor {
    union {
        char* var;
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
    char* fun;

    FunctionArgs* args;
    int zero;

    Expression* expression;
};

struct NextCase {
    char* fun;

    FunctionArgs* args;

    char* plus;
    int one;

    Expression* expression;
};

struct Evaluation {
    union {
        FunctionEvaluation* functionEvaluation;
        EvaluationFactor* evaluationFactor;
    };

    TermType type;
};

struct FunctionEvaluation {
    char* fun;
    EvaluationArgs* evaluationArgs;
};

struct EvaluationFactor {
    union {
        char* var;
        int num;
    };

    FactorType type;
};

struct EvaluationArgs {
    Evaluation* evaluation;
    EvaluationArgs* evaluationArgs;
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program * program);
void releaseStatements(Statements * statements);
void releaseStatement(Statement * statement);

void releaseFunctionArgs(FunctionArgs* functionArgs);

void releaseDefinition(Definition * definition);
void releaseDefintionBody(DefinitionBody* definitionBody);

void releaseExpression(Expression * expression);
void releaseFunctionExpression(FunctionExpression* functionExpression);
void releaseExpressionFactor(ExpressionFactor* expressionFactor);
void releaseExpressionArgs(ExpressionArgs* expressionArgs);

void releaseCompositionDef(CompositionDef * def);
void releaseRecursiveDef(RecursiveDef * def);
void releaseBaseCase(BaseCase * baseCase);
void releaseNextCase(NextCase * nextCase);

void releaseEvaluation(Evaluation * evaluation);
void releaseFunctionEvaluation(FunctionEvaluation * functionEvaluation);
void releaseEvaluationFactor(EvaluationFactor * evaluationFactor);
void releaseEvaluationArgs(EvaluationArgs * evaluationArgs);

#endif
