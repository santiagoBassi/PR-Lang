#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static boolean _logIgnoredLexemes = true;

void initializeFlexActionsModule() {
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	_logger = createLogger("FlexActions");
}

void shutdownFlexActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/* PRIVATE FUNCTIONS */

static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext);

/**
 * Logs a lexical-analyzer context in DEBUGGING level.
 */
static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext) {
	char * escapedLexeme = escape(lexicalAnalyzerContext->lexeme);
	logDebugging(_logger, "%s: %s (context = %d, length = %d, line = %d)",
		functionName,
		escapedLexeme,
		lexicalAnalyzerContext->currentContext,
		lexicalAnalyzerContext->length,
		lexicalAnalyzerContext->line);
	free(escapedLexeme);
}

/* PUBLIC FUNCTIONS */

void BeginCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void EndCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

Token IntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->integer = atoi(lexicalAnalyzerContext->lexeme);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return INTEGER;
}

Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

Token IdLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = calloc(1 + lexicalAnalyzerContext->length, sizeof(char));
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return ID;
}

Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

Token CommaSeparatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = COMMA_SEPARATOR;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return COMMA_SEPARATOR;
}

Token FunctionDecoratorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

Token EqualsLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = EQUALS;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return EQUALS;
}

Token NewLineLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext){
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = NEW_LINE;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return NEW_LINE;
}

Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return UNKNOWN;
}
