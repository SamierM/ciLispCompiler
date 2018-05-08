/**
 *
 * Name: Marco Antonio Bustamante
 * Lab/Task: Lab 9 Task 1
 *
 */

#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../cmake-build-debug/ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

typedef enum oper { // must be in sync with funcs in resolveFunc()
    NEG,
    ABS,
    EXP,
    SQRT,
    ADD,
    SUB,
    MULT,
    DIV,
    REMAINDER,
    LOG,
    POW,
    MAX,
    MIN,
    EXP2,
    CBRT,
    HYPOT,
    PRINTOP,
    EQUALOP,
    SMALLER,
    LARGER,
    RAND,
    READ,
    INVALID_OPER=255
} OPER_TYPE;

typedef enum {
    NUM_TYPE, FUNC_TYPE, SYMBOL_TYPE
} AST_NODE_TYPE;


typedef enum { NO_TYPE, INTEGER_TYPE, REAL_TYPE} DATA_TYPE;

typedef enum { VARIABLE_TYPE, LAMBDA_TYPE, ARG_TYPE } TYPE_OF_SYMBOL;

typedef struct return_value {
    DATA_TYPE type;
    double value;
} RETURN_VALUE;

typedef struct {
    double value;
} NUMBER_AST_NODE;

typedef struct {
    char *name;
    struct ast_node* opList;
} FUNCTION_AST_NODE;


typedef struct symbol_ast_node {
    char *name;
} SYMBOL_AST_NODE;


typedef struct symbol_table_node {
    TYPE_OF_SYMBOL symbol_type;
    DATA_TYPE val_type;
    char *ident;
    struct ast_node *val;
    struct symbol_table_node *next;
    struct symbol_value_stack *stack;
} SYMBOL_TABLE_NODE;

typedef struct symbol_value_stack {
    struct symbol_value_stack* next;
    struct ast_node *valueOnStack;
}SYMB_VAL_STACK_ELEMENT;


typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE* scope;
    struct ast_node *parent;
    union {
        NUMBER_AST_NODE number;
        FUNCTION_AST_NODE function;
        SYMBOL_AST_NODE symbol;
    } data;
    struct ast_node *next;
} AST_NODE;

//create specific AST NODE TYPES
AST_NODE *number(double value);
AST_NODE *function(char *funcName, AST_NODE *operandList);
AST_NODE *makeSymbol(char *symbolName);
RETURN_VALUE* makeDataType(char *variableTypeAsString);
SYMBOL_TABLE_NODE* createUserFunction(RETURN_VALUE* returnValueNode,char* functionName,SYMBOL_TABLE_NODE* argumentList, AST_NODE* functionDefinition);


//CREATE MODIFICATIONS TO AST NODES
SYMBOL_TABLE_NODE *createSymbolList(SYMBOL_TABLE_NODE *headOfList, SYMBOL_TABLE_NODE* tailOfList);
SYMBOL_TABLE_NODE *createArgumentList(char* argumentName, SYMBOL_TABLE_NODE *tailOfList);
AST_NODE* sExprList(AST_NODE* headOfList, AST_NODE* tailList);
AST_NODE* setScope(SYMBOL_TABLE_NODE* childScope, AST_NODE* parentList);
SYMBOL_TABLE_NODE *let_elem(RETURN_VALUE *returnValNode, char *symbolName, AST_NODE *symbolValue, SYMBOL_TABLE_NODE *next);
SYMBOL_TABLE_NODE* createArgumentNode(char* argumentName);
double roundIntegerFromDouble(AST_NODE *symbolValue, char* symbolName);
SYMB_VAL_STACK_ELEMENT* createNewStackElement(SYMBOL_TABLE_NODE* formalParameter, AST_NODE* actualParameter);


void freeNode(AST_NODE *p);

//process made nodes
RETURN_VALUE eval(AST_NODE *ast);
SYMBOL_TABLE_NODE* evaluateSymbol(AST_NODE* symbolNodeToEvaluate);
RETURN_VALUE evaluateLambdaSymbol(AST_NODE* uFunctionToEvaluate);
void checkForDuplicateSymbols(SYMBOL_TABLE_NODE* list1, SYMBOL_TABLE_NODE* list2);
void printAnswer(RETURN_VALUE valueToPrint);
AST_NODE *conditional(AST_NODE *conditionalEvaluation, AST_NODE *trueStatement, AST_NODE *falseStatement);

//Auxillary Functions for EVAL
int validateMinimumNumberOfOperands(int numberOfOperands, double *resultValue, int enumeratedFunctionName, AST_NODE* functionThatContainsOperandList);
void considerNextOperand(int *operandCount, AST_NODE **currentOperand);
AST_NODE* callToUserFunction(char* userFunctionName,AST_NODE* actualParameters);
SYMB_VAL_STACK_ELEMENT* getTopOfStack(SYMBOL_TABLE_NODE* symbolVariableWithStack);


#endif
