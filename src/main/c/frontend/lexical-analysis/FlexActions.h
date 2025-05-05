#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

void BeginCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);


Token IntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token FunctionNameLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token CommaSeparatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token FunctionDecoratorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token EqualsLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token FileNameLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token ParameterLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token NewLineLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);



Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif
