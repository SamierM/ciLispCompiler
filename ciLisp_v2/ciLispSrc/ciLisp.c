/**
 *
 * Name: Samier Mahagna
 * Lab/Task: Lab 9 Task 7
 *
 */



#include "ciLisp.h"

#define ROUND_UP_FLOOR .5
#define MAX_NUMBER_OF_CONDITIONALS 3
#define MINIMUM_BINARY_OPERANDS 2

//
// find out which function it is from the name and matches with corresponding value in .h file
//
int resolveFunc(char *func) {
    char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min",
                     "exp2", "cbrt", "hypot", "print", "equal", "smaller", "greater", "rand", "read", ""};

    int i = 0;
    while (funcs[i][0] != '\0') {
        if (!strcmp(funcs[i], func))
            return i;

        i++;
    }
    yyerror("invalid function");
    return INVALID_OPER;
}

//Assigns the list of symbols to the expression to contain it within the correct scope
//i.e. one abc may be defined twice within two different scopes but not two "abc"s within one scope
AST_NODE *setScope(SYMBOL_TABLE_NODE *childScope, AST_NODE *parent) {
    //first check for duplicates variables which will halt the program if found
    checkForDuplicateSymbols(childScope, (parent->scope));
    parent->scope = childScope;


    return parent;
}

//checks one symbol table list for definitions of a variable. If the variable exists then we produce an error and halt program
void checkForDuplicateSymbols(SYMBOL_TABLE_NODE *list1, SYMBOL_TABLE_NODE *list2) {
    //check each element from one list with each element from the other
    SYMBOL_TABLE_NODE *currentVariableList1 = list1;
    SYMBOL_TABLE_NODE *currentVariableList2 = list2;
    while ((currentVariableList1) != NULL && currentVariableList1->ident != NULL) {
        while (((currentVariableList2) != NULL) && currentVariableList2->ident != NULL) {
            //if they are the same then produce error
            if (strcmp(currentVariableList1->ident, currentVariableList2->ident) == 0) {
                yyerror("ERROR: VARIABLE PREVIOUSLY DEFINED!!!\n");
                exit(-22);
            }
            currentVariableList2 = currentVariableList2->next;
        }
        currentVariableList1 = currentVariableList1->next;
    }

}

//
// create a list which is a list of symbols to be assigned to a function
//
SYMBOL_TABLE_NODE *createSymbolList(SYMBOL_TABLE_NODE *headOfList, SYMBOL_TABLE_NODE *tailOfList) {
    //make a check potentially? for doubles will think about why I need double check later
    //i.e. let a 2 and let a 3 how to handle this
    headOfList->next = tailOfList;

    return headOfList;
}

//
// create a list which is a list of symbols to be assigned to a function
//
SYMBOL_TABLE_NODE *createArgumentList(char *argumentName, SYMBOL_TABLE_NODE *tailOfList) {
    SYMBOL_TABLE_NODE *headOfList = createArgumentNode(argumentName);
    headOfList->next = tailOfList;

    return headOfList;
}

/*
 * Similar to the above let list but creates a list of expressions to be used by a function
 */
AST_NODE *sExprList(AST_NODE *headOfList, AST_NODE *tailList) {
    headOfList->next = tailList;
    return headOfList;
}

//
// create let element
// assigns a variable a type, a value (which could be an s_expr), and a name. This is where the symbol is created
//
SYMBOL_TABLE_NODE *
let_elem(RETURN_VALUE *returnValNode, char *symbolName, AST_NODE *symbolValue, SYMBOL_TABLE_NODE *next) {
    SYMBOL_TABLE_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE) + sizeof(AST_NODE) + sizeof(RETURN_VALUE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

//    if (returnValNode->type == INTEGER_TYPE)//will change the value inside actual node
//        symbolValue->data.number.value = roundIntegerFromDouble(symbolValue, symbolName);

    symbolValue->parent = makeSymbol(symbolName);
    p->ident = symbolName;
    p->val_type = returnValNode->type;
    p->next = next;
    p->val = symbolValue;
    p->symbol_type = VARIABLE_TYPE;

    return p;
}

//creates an argument symbol which will be a part of a list and then a part of a user defined function
//

SYMBOL_TABLE_NODE *createArgumentNode(char *argumentName) {
    SYMBOL_TABLE_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE) + sizeof(AST_NODE) + sizeof(RETURN_VALUE) + sizeof(SYMB_VAL_STACK_ELEMENT);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->ident = argumentName;
    p->val_type = REAL_TYPE; //assume it is a real by default
    p->next = NULL;
    p->symbol_type = ARG_TYPE;

    return p;
}

/*
 * Function that defines a user function and attaches the argument list to it
 * The function is represented by a symbol, i.e.: f(x,y) will be a symbol of lambda type which has the necessary features attached
 *
 * $$ = createUserFunction($2, $3, $6, $7);
 *  LPAREN type SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN //( type symbol lambda ( arg_list ) s_expr );
 */
SYMBOL_TABLE_NODE *
createUserFunction(RETURN_VALUE *returnValueNode, char *functionName, SYMBOL_TABLE_NODE *argumentList,
                   AST_NODE *functionDefinition) {
    SYMBOL_TABLE_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE) + sizeof(AST_NODE) + sizeof(RETURN_VALUE) + sizeof(SYMB_VAL_STACK_ELEMENT);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->symbol_type = LAMBDA_TYPE;
    p->val_type = returnValueNode->type;
    p->ident = functionName;
    functionDefinition->scope = argumentList;
    p->val = functionDefinition;

    return p;
}

/*
 * Creates element and pushes onto stack so we can use user functions recursively. Top of stack is considered to be
 * end of linked list
 * Returns the top of the stack
 */
SYMB_VAL_STACK_ELEMENT* createNewStackElement(SYMBOL_TABLE_NODE* formalParameter, AST_NODE* actualParameter)
{
    SYMB_VAL_STACK_ELEMENT* currentStackElement = formalParameter->stack;
    SYMB_VAL_STACK_ELEMENT* topOfStack = formalParameter->stack;

    SYMB_VAL_STACK_ELEMENT *newStackElement;
    size_t nodeSize;

    nodeSize = sizeof(SYMB_VAL_STACK_ELEMENT);
    if ((newStackElement = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    newStackElement->next = NULL;
    newStackElement->valueOnStack = actualParameter;

    //find top of stack;
    while(currentStackElement != NULL)
    {
        topOfStack = currentStackElement;
        currentStackElement = currentStackElement->next;
    }
    if (topOfStack == NULL)
    {
        formalParameter->stack = newStackElement;
    }
    else
        topOfStack->next = newStackElement;
    //currentStackElement = newStackElement;

    return newStackElement;
}



/*
 * Takes a symbol which needs to be converted into a ASTNODE so that it can be sent to the evaluator
 */
AST_NODE *callToUserFunction(char *userFunctionName, AST_NODE *actualParameters) {

    AST_NODE *constructedLambdaCall = function(userFunctionName,
                                               actualParameters); //create a new AST node which we will return after constructing the appropriate function node
    constructedLambdaCall->type = SYMBOL_TYPE; //we do this so that when we get to the evaluator we will process this correctly
    return constructedLambdaCall;

}

/*
 * Rounds numbers up if .5 or greater and rounds down otherwise
 */
double roundIntegerFromDouble(AST_NODE *symbolValue, char *symbolName) {
    double temporaryDouble;

    temporaryDouble = eval(symbolValue).value;
    if (fmod(temporaryDouble, 1.0) >= ROUND_UP_FLOOR) //round up if decimal is greater than or equal to .5
    {
        temporaryDouble += 1; //increase by 1 to simulate round up
        printf("WARNING: incompatible type assignment for variables %s\n", symbolName);
    }
    temporaryDouble = ((int) temporaryDouble) / 1; //lose decimal

    return temporaryDouble; //update value
}

//
// create a node for a number
// this is an AST node which does not currently carry a INTEGER or REAL value (assumes REAL by default)
//
AST_NODE *number(double value) {
    AST_NODE *p;
    size_t nodeSize;


    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    p->parent = NULL;
    p->scope = NULL;

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}

//
// create a node for a function and set the linked list of operands and assigns their parents
// this creates an AST_NODE
//
AST_NODE *function(char *funcName, AST_NODE *operandList) {
    AST_NODE *p;
    size_t nodeSize;
    AST_NODE *currentOp = operandList;


    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    p->scope = NULL;
    p->parent = NULL;

    //assign the attributes of the node
    p->type = FUNC_TYPE;
    p->data.function.name = funcName;

    //set parent for the ENTIRE list of variables associated with the function
    while (currentOp != NULL) {
        currentOp->parent = p;
        if (currentOp->scope != NULL)
            setScope(currentOp->scope, p);

        currentOp = currentOp->next;
    }

    //append to list
    p->data.function.opList = operandList;
    return p;
}



//creates a conditional statement
/*
 *
 */
AST_NODE *conditional(AST_NODE *conditionalEvaluation, AST_NODE *trueStatement, AST_NODE *falseStatement) {
    AST_NODE *p;
    size_t nodeSize;

    AST_NODE *legs;


    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((legs = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    p->scope = NULL;
    p->parent = NULL;
    legs->scope = NULL;
    legs->parent = NULL;


    //assign parents appropriately
    //set parents
    if (conditionalEvaluation != NULL) {
        conditionalEvaluation->parent = p;
        if (conditionalEvaluation->scope != NULL)
            setScope(conditionalEvaluation->scope, p);
    }
    if (trueStatement != NULL) {
        trueStatement->parent = p;
        //Check and assign scope from children setScope(child scope, parent)
        if ((trueStatement->scope) != NULL)
            setScope((trueStatement->scope), conditionalEvaluation);
    }
    if (falseStatement != NULL) {
        falseStatement->parent = p;
        //Check and assign scope from children setScope(child scope, parent)
        if ((falseStatement->scope) != NULL)
            setScope((falseStatement->scope), conditionalEvaluation);
    }

    trueStatement->next = falseStatement;
    conditionalEvaluation->next = trueStatement;
    p->data.function.opList = conditionalEvaluation;
    if (conditionalEvaluation != NULL)
        p->data.function.name = conditionalEvaluation->data.function.name;
    p->type = FUNC_TYPE;


    return p;

}


/*
 * Creation of Symbol AST NODE (NOT TO BE CONFUSED WITH THE SYMBOL LIST WHICH HOLDS THESE ELEMENTS)
 */
AST_NODE *makeSymbol(char *symbolName) {
    AST_NODE *p;
    size_t nodeSize;


    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    p->parent = NULL;
    p->scope = NULL;

    p->type = SYMBOL_TYPE;
    p->data.symbol.name = symbolName;

    return p;

}

/*
 * Describes the kind of number as REAL or INTEGER
 */
RETURN_VALUE *makeDataType(char *variableTypeAsString) {
    RETURN_VALUE *p;
    size_t nodeSize;

    nodeSize = sizeof(RETURN_VALUE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    if (variableTypeAsString != NULL && strcmp((variableTypeAsString), "integer") == 0) {
        p->type = INTEGER_TYPE;
    } else //its a real by default
    {
        p->type = REAL_TYPE;
    }

    return p;
}

/*
 * Prints the value depending on data type
 * with formatting
 */
void printAnswer(RETURN_VALUE valueToPrint) {
    if (valueToPrint.type == REAL_TYPE) {
        printf("%.2lf\n", valueToPrint.value);
    } else {
        printf("%.0f", valueToPrint.value);
    }
}


//
// free a node
//
void freeNode(AST_NODE *p) {
    if (!p)
        return;

    if (p->type == FUNC_TYPE) {
        free(p->data.function.name);
        freeNode(p->data.function.opList);
    }

    free(p);
}

void popEvaluatedStackElement( SYMB_VAL_STACK_ELEMENT* bottomOfStack)
{
    SYMB_VAL_STACK_ELEMENT* currentElementOnStack = bottomOfStack;
    SYMB_VAL_STACK_ELEMENT* previousElementOnStack = bottomOfStack;
    while(previousElementOnStack->next != NULL)
    {
        if (currentElementOnStack->next == NULL)
        {
            previousElementOnStack->next = NULL;
        }
        currentElementOnStack = currentElementOnStack->next;
    }
    //free(currentElementOnStack);
    previousElementOnStack->next = NULL;
}

//
// evaluate an abstract syntax tree
//
// p points to the root
//
RETURN_VALUE eval(AST_NODE *p) {
    RETURN_VALUE result;
    result.value = 0;
    result.type = REAL_TYPE; //assume real by default
    int operandCount = 0;
    AST_NODE *currentOperand = p->data.function.opList;

    if (!p) {
        return result;
    } else if (p->type == NUM_TYPE) {
        result.value = p->data.number.value;
    } else if (p->type == FUNC_TYPE) {
        int enumeratedFunctionName = resolveFunc(p->data.function.name);

        AST_NODE *conditionalStatement = p->data.function.opList;
        AST_NODE *trueStatement = p->data.function.opList;
        AST_NODE *falseStatement = p->data.function.opList;

        switch (enumeratedFunctionName) {
            case NEG:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) { //checks to see if we have minimum # of operands and retursn 1 if we do
                    result.value = -eval(p->data.function.opList).value;
                }
                break;
            case ABS:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) { //checks to see if we have minimum # of operands and retursn 1 if we do
                    result.value = fabs(eval(p->data.function.opList).value);
                }
                break;
            case EXP:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) { //checks to see if we have minimum # of operands and retursn 1 if we do
                    result.value = exp(eval(p->data.function.opList).value);
                }
                break;
            case SQRT:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) { //checks to see if we have minimum # of operands and retursn 1 if we do
                    result.value = sqrt(eval(p->data.function.opList).value);
                }
                break;
            case ADD:
                //traverse through list and get next operand to calculate
                while (currentOperand != NULL) {
                    result.value += eval(currentOperand).value;
                    considerNextOperand(&operandCount, &currentOperand);
                }
                validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, NULL);
                break;
            case SUB:
                //need a prime read to subtract from first value
                // i.e. 3-8-4 needs to subtract from 3 first
                if (currentOperand != NULL) {
                    result.value = eval(currentOperand).value;
                    considerNextOperand(&operandCount, &currentOperand);
                }
                while (currentOperand != NULL) {
                    result.value -= eval(currentOperand).value;
                    considerNextOperand(&operandCount, &currentOperand);
                }
                validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, NULL);
                break;
            case MULT:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) //checks to see if we have minimum # of operands and retursn 1 if we do
                {
                    result.value = eval(currentOperand).value; //need to do a prime read like in subtraction example
                    considerNextOperand(&operandCount, &currentOperand);
                    while (currentOperand != NULL) {
                        result.value *= eval(currentOperand).value;
                        considerNextOperand(&operandCount, &currentOperand);
                    }
                }

                break;
            case DIV:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) //checks to see if we have minimum # of operands and retursn 1 if we do
                {
                    result.value = eval(currentOperand).value; //need to do a prime read like in subtraction example
                    considerNextOperand(&operandCount, &currentOperand);
                    while (currentOperand != NULL) {
                        result.value /= eval(currentOperand).value;
                        considerNextOperand(&operandCount, &currentOperand);
                    }
                }
                break;
            case REMAINDER:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    result.value = fmod(eval(p->data.function.opList).value,
                                        eval(p->data.function.opList->next).value);
                }
                break;
            case LOG:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    AST_NODE *op2 = p->data.function.opList->next; //for readability
                    switch ((int) eval(p->data.function.opList).value) { //it's either log base 2 or log base 10
                        case 2:
                            result.value = log2(eval(op2).value);
                            break;
                        case 10:
                            result.value = log10(eval(op2).value);
                            break;
                        default:
                            result.value = 0.0;
                            printf("The log base you entered is not supported. Please use base 2 or 10\n");
                    }
                }
                break;
            case POW:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    result.value = pow(eval(p->data.function.opList).value,
                                       eval(p->data.function.opList->next).value);
                }
                break;
            case MAX:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    result.value = fmax(eval(p->data.function.opList).value,
                                        eval(p->data.function.opList->next).value);
                }
                break;
            case MIN:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    result.value = fmin(eval(p->data.function.opList).value,
                                        eval(p->data.function.opList->next).value);
                }
                break;
            case EXP2:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) { //checks to see if we have minimum # of operands and retursn 1 if we do
                    result.value = exp2(eval(p->data.function.opList).value);
                }
                break;
            case CBRT:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName,
                                                    p)) { //checks to see if we have minimum # of operands and retursn 1 if we do
                    result.value = cbrt(eval(p->data.function.opList).value);
                }
                break;
            case HYPOT:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    result.value = hypot(eval(p->data.function.opList).value,
                                         eval(p->data.function.opList->next).value);
                }
                break;
            case PRINTOP:
                validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p);
                //traverse through list and get next operand to print
                while (currentOperand != NULL) {
                    result.value = eval(currentOperand).value;
//                    printAnswer(eval(currentOperand));
                    printAnswer(result);
                    considerNextOperand(&operandCount, &currentOperand);
                }
                //returns final printed value
                break;
            case EQUALOP:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    if (eval(conditionalStatement).value == eval(conditionalStatement->next).value) {
                        result.value = eval(trueStatement).value;
                    } else {
                        result.value = eval(falseStatement).value;
                    }
                }//produce the correct leg
                break;
            case SMALLER:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    if (eval(conditionalStatement).value < eval(conditionalStatement->next).value) {
                        result.value = eval(trueStatement).value;
                    } else {
                        result.value = eval(falseStatement).value;
                    }
                }//produce the correct leg
                break;
            case LARGER:
                if (validateMinimumNumberOfOperands(operandCount, &(result.value), enumeratedFunctionName, p)) {
                    if (eval(conditionalStatement).value > eval(conditionalStatement->next).value) {
                        result.value = eval(trueStatement).value;
                    } else {
                        result.value = eval(falseStatement).value;
                    }
                }//produce the correct leg
                break;
            case RAND: //produce a random number
                result.value = (double) rand() / (double) rand();
                break;
            case READ:
                printf("\nread %s := ", p->parent->data.symbol.name);
                scanf("%lf", &result.value);
                break;
            default:
                exit(-99); //-99 means we did not hit any correct arithmetic functions
        }
    } else if (p->type == SYMBOL_TYPE) {
        SYMBOL_TABLE_NODE *tableWithAllInformation = evaluateSymbol(p);
        SYMB_VAL_STACK_ELEMENT* topSymbolOnStack;

        switch (tableWithAllInformation->symbol_type) {
            case VARIABLE_TYPE:
//                tableWithAllInformation = evaluateSymbol(p);
//                //tableWithAllInformation = evaluateSymbol(p);
                result.value = eval(tableWithAllInformation->val).value;
                result.type = tableWithAllInformation->val_type;
                break;
            case ARG_TYPE:
                topSymbolOnStack = getTopOfStack(tableWithAllInformation);
                result.value = eval(topSymbolOnStack->valueOnStack).value;
                popEvaluatedStackElement(tableWithAllInformation->stack);
                result.type = tableWithAllInformation->val_type;
                break;
            case LAMBDA_TYPE:
                //evaluate lambda definition by traversing through arguments
                //tableWithAllInformation = evaluateSymbol(makeSymbol(p->data.function.name));  //we need to force our eval to find the correct symbol
                result = evaluateLambdaSymbol(p);
                break;
        }

    }

    return result;
}

//AUXILIARY FUNCTIONS FOR EVAL
/*
 * Auxiliaryy function for eval that checks to see that binary operations have at least 2 elements
 * returns a zero value if minimum number is not reached
 */
int validateMinimumNumberOfOperands(int numberOfOperands, double *resultValue, int enumeratedFunctionName,
                                    AST_NODE *functionThatContainsOperandList) {
    char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max",
                     "min",
                     "exp2", "cbrt", "hypot", "print", "equal", "smaller", "greater", "rand", "read", ""};

    //for readability
//    AST_NODE *conditionalStatement = functionThatContainsOperandList->data.function.opList;
//    AST_NODE *trueStatement = functionThatContainsOperandList->data.function.opList;
//    AST_NODE *falseStatement = functionThatContainsOperandList->data.function.opList;

    switch (enumeratedFunctionName) {
        //SINGLEOPERANDS
        case CBRT:
        case PRINTOP:
        case EXP2:
        case SQRT:
        case EXP:
        case ABS:
        case NEG:
            if (functionThatContainsOperandList->data.function.opList == NULL) {
                printf("ERROR: too few parameters for the function %s\n", funcs[enumeratedFunctionName]);
                *resultValue = 0.0;
            }
            break;
            //cases where we have a count of operands
        case ADD:
        case SUB:
            if (numberOfOperands < MINIMUM_BINARY_OPERANDS) //need at least two operands for binary operations
            {
                printf("ERROR: too few parameters for the function %s\n", funcs[enumeratedFunctionName]);
                *resultValue = 0.0;
                return 0;
            }
            break;
        case MULT:
        case DIV:
        case REMAINDER:
        case LOG:
        case POW:
        case MAX:
        case MIN:
        case HYPOT:
            if (functionThatContainsOperandList->data.function.opList->next == NULL) {
                printf("ERROR: too few parameters for the function %s\n", funcs[enumeratedFunctionName]);
                *resultValue = 0.0;
                return 0;
            }
            break;
        case SMALLER:
        case LARGER:
        case EQUALOP:
            if (functionThatContainsOperandList->data.function.opList == NULL ||
                functionThatContainsOperandList->data.function.opList->next == NULL ||
                functionThatContainsOperandList->data.function.opList->next->next == NULL) {
                printf("ERROR: too few parameters for the function %s\n", funcs[enumeratedFunctionName]);
                *resultValue = 0.0;
            }
            break;
        default:
            printf("\nSome sort of error while validating number of operands");
            break;
    }
    return 1; //1 value means the function can be evaluated with current number of operands

}

/*
 * Auxiliary Function that increments the count of operands by one and goes to next operand to be evaluated
 */
void considerNextOperand(int *operandCount, AST_NODE **currentOperand) {
    (*operandCount)++;
    *currentOperand = (*currentOperand)->next;
}

//
// Evaluate the symbol by checking the parent's scope for its unique value
//
SYMBOL_TABLE_NODE *evaluateSymbol(AST_NODE *symbolNodeToEvaluate) {

    AST_NODE *currentParent = symbolNodeToEvaluate; //we may not find the symbol in this scope and need to go to a higher level. This is how we go to higher levels
    if (currentParent->scope == NULL) {
        currentParent = symbolNodeToEvaluate->parent;
    }
    SYMBOL_TABLE_NODE *currentSymbol = currentParent->scope; //this is the placeholder to go through the list of symbols to match to the variable

    //this is the name of the variable we are looking for. i.e. int a = ?. In this the name refers to a
    char *variableToCheckName = symbolNodeToEvaluate->data.symbol.name;


    //traverse upwards to top parent node for the symbol
    do {
        currentSymbol = currentParent->scope;
        while (currentSymbol !=
               NULL) { //traverses through the list associated with the current parent before moving to the higher level
            if (strcmp(currentSymbol->ident, variableToCheckName) == 0) //found correct symbol in list
            {
                if (currentSymbol->symbol_type != VARIABLE_TYPE || currentSymbol->val->type != SYMBOL_TYPE) {
                    return currentSymbol;//we found the correct symbol with its associated value
                } else { //update to new symbol to check for

                    //this is the case where a = b. So now we have found the value of a which is b. So we must update name to b and begin to look for that
                    variableToCheckName = (currentSymbol->val->data.symbol.name);
                    //this is the new nested symbol we are looking for

                    currentSymbol = NULL; //force going to a higher parent level
                }
                //if it is a symbol we need to process it more by going to the parent
            } else { currentSymbol = currentSymbol->next; }
        }
        //went through the entire scope and need to go to higher parent level
        currentParent = currentParent->parent;

    } while (currentParent != NULL);

    yyerror("\nThe symbol does not exist in this scope!");
    return NULL; //for error checking

}

//evaluate lambda definition by traversing through arguments
//userFunctionToEvaluate is the tree with node which has the user defined function
RETURN_VALUE evaluateLambdaSymbol(AST_NODE *uFunctionToEvaluate) {
    //first we must find where the function is located within the proper scope
    SYMBOL_TABLE_NODE *userFunctionDefinition = evaluateSymbol(
            uFunctionToEvaluate); //this stores the predefined function with list of arguments

    SYMBOL_TABLE_NODE *currentFormalArgument = userFunctionDefinition->val->scope;
    AST_NODE *currentActualArgument = uFunctionToEvaluate->data.function.opList;
    RETURN_VALUE evaluatedFunction;

    SYMB_VAL_STACK_ELEMENT *newFunctionStackElement = createNewStackElement( userFunctionDefinition, userFunctionDefinition->val);


    while (currentFormalArgument != NULL) //assign values to currently NULLed formal parameters
    {
        if (currentActualArgument == NULL) {
            printf("ERROR: too few parameters for the function %s\n", uFunctionToEvaluate->data.function.name);
            exit(-2);
        }

//        currentFormalArgument->val = currentActualArgument; //instead of this we will push items on the stack to be evaluated recursively
        createNewStackElement(currentFormalArgument,currentActualArgument);
        //update parameters to traverse
        currentFormalArgument = currentFormalArgument->next;
        currentActualArgument = currentActualArgument->next;
    }
    //newFunctionStackElement->valueOnStack = userFunctionDefinition->val;
    evaluatedFunction = eval(newFunctionStackElement->valueOnStack);
    popEvaluatedStackElement(userFunctionDefinition->stack);

    return evaluatedFunction;
}

SYMB_VAL_STACK_ELEMENT* getTopOfStack(SYMBOL_TABLE_NODE* symbolVariableWithStack)
{
        SYMB_VAL_STACK_ELEMENT* currentElementOnStack = symbolVariableWithStack->stack;
        SYMB_VAL_STACK_ELEMENT* previousElementOnStack = NULL;
        while(currentElementOnStack != NULL)
        {
            previousElementOnStack = currentElementOnStack;
            currentElementOnStack = currentElementOnStack->next;
        }
        return previousElementOnStack;

}