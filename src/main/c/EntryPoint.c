#include "backend/FunctionTable.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "frontend/syntactic-analysis/ASTPrinter.h"
#include "shared/CompilerState.h"
#include "shared/Environment.h"
#include "shared/Logger.h"
#include "shared/String.h"
#include "shared/Arguments.h"
#include "backend/Generator.h"
#include <stdio.h>

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int count, const char ** arguments) {
	Logger * logger = createLogger("EntryPoint");

	// Logs the arguments of the application.
	for (int k = 0; k < count; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}

    CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
        .functionTable = NULL,
		.succeed = false,
		.value = 0
	};

    if ((compilerState.inputFile = getInputFilename(count, arguments)) == NULL || (compilerState.outputFile = getOutputFilename(count, arguments)) == NULL) {
        logError(logger, "The compiler must be called in the following way: compiler inputfile outputfile");
        destroyLogger(logger);
        return FAILED;
    }

    compilerState.functionTable = createFunctionTable();

	initializeFlexActionsModule();
	initializeBisonActionsModule();
	initializeSyntacticAnalyzerModule();
	initializeAbstractSyntaxTreeModule();
	initializeGeneratorModule(&compilerState);

	// Begin compilation process.
	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
	CompilationStatus compilationStatus = SUCCEED;
	Program * program = compilerState.abstractSyntaxtTree;
	if (syntacticAnalysisStatus == ACCEPT) {
		// ----------------------------------------------------------------------------------------
		// Beginning of the Backend... ------------------------------------------------------------
        //logInformation(logger, "The syntactic-analysis phase accepts the input program.");
		generateCode(&compilerState);
        compilationStatus = (compilerState.succeed? SUCCEED : FAILED);
		// ...end of the Backend. -----------------------------------------------------------------
		// ----------------------------------------------------------------------------------------
	}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}

    if (strcmp(getStringOrDefault("PRINT_AST", "NO"), "YES") == 0) {
        printProgram(program);
    }

    freeFunctionTable(compilerState.functionTable);
	logDebugging(logger, "Releasing AST resources...");
	releaseProgram(program);
	logDebugging(logger, "Releasing modules resources...");
	shutdownGeneratorModule();
	shutdownAbstractSyntaxTreeModule();
	shutdownSyntacticAnalyzerModule();
	shutdownBisonActionsModule();
	shutdownFlexActionsModule();
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	return compilationStatus;
}
