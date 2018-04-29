/**
 *
 * Name: Samier Mahagna
 * Lab/Task: Lab 9 Task 4
 *
 */



#include "ciLisp.h"
#define ROUND_UP_FLOOR .5
#define MAX_NUMBER_OF_CONDITIONALS 3
#define MINIMUM_BINARY_OPERANDS 2

//
// find out which function it is
//
int resolveFunc(char *func)
{
    char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min",
                     "exp2", "cbrt", "hypot","print","equal","smaller","greater","rand", "read", ""};

    int i = 0;
    while (funcs[i][0] != '\0')
    {
        if (!strcmp(funcs[i], func))
            return i;

        i++;
    }
    yyerror("invalid function");
    return INVALID_OPER;
}

//Assigns the list of symbols to the expression to contain it within the correct scope
//i.e. one abc may be defined twice within two different scopes but not two "abc"s within one scope
AST_NODE* setScope(SYMBOL_TABLE_NODE* childScope, AST_NODE* parent)
{
    //first check for duplicates variables which will halt the program if found
    checkForDuplicateSymbols(childScope,(parent->scope));
    parent->scope = childScope;


    return parent;
}

//checks one symbol table list for definitions of a variable. If the variable exists then we produce an error and halt program
void checkForDuplicateSymbols(SYMBOL_TABLE_NODE* list1, SYMBOL_TABLE_NODE* list2)
{
    //check each element from one list with each element from the other
    SYMBOL_TABLE_NODE* currentVariableList1 = list1;
    SYMBOL_TABLE_NODE* currentVariableList2 = list2;
    while((currentVariableList1) != NULL && currentVariableList1->ident !=NULL)
    {
        while(((currentVariableList2) != NULL) && currentVariableList2->ident != NULL)
        {
            //if they are the same then produce error
            if (strcmp(currentVariableList1->ident, currentVariableList2->ident) == 0)
            {
                yyerror("ERROR: VARIABLE PREVIOUSLY DEFINED!!!\n");
                exit(-22);
            }
            currentVariableList2 = currentVariableList2->next;
        }
        currentVariableList1 = currentVariableList1->next;
    }

}

////assigns the list to become a scope
//SYMBOL_TABLE_NODE* setLetList(SYMBOL_TABLE_NODE* theList)
//{
//    return theList;
//}
//
//SYMBOL_TABLE_NODE* setLetElement(SYMBOL_TABLE_NODE* letElement)
//{
//    return letElement;
//}

//
// create let list
//
SYMBOL_TABLE_NODE *let_list(SYMBOL_TABLE_NODE* headOfList, SYMBOL_TABLE_NODE* tailOfList)
{
    //make a check potentially? for doubles will think about why I need double check later
    //i.e. let a 2 and let a 3 how to handle this
    headOfList->next = tailOfList;

    return headOfList;
}

//
// create let element
//
SYMBOL_TABLE_NODE *let_elem(RETURN_VALUE *returnValNode, char *symbolName, AST_NODE *symbolValue, SYMBOL_TABLE_NODE *next)
{
    SYMBOL_TABLE_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE) + sizeof(AST_NODE) + sizeof(RETURN_VALUE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    if (returnValNode->type == INTEGER_TYPE)
        symbolValue->data.number.value = roundIntegerFromDouble(symbolValue, symbolName); //will change the value inside actual node



    p->ident = symbolName;
    p->val_type = returnValNode->type;
    p->next = next;
    p->val = symbolValue;

    return p;
}

double roundIntegerFromDouble(AST_NODE *symbolValue, char* symbolName)
{
    double temporaryDouble;

    temporaryDouble = eval(symbolValue).value;
    if (fmod(temporaryDouble,1.0) >= ROUND_UP_FLOOR ) //round up if decimal is greater than or equal to .5
    {
        temporaryDouble += 1; //increase by 1 to simulate round up
        printf("WARNING: incompatible type assignment for variables %s\n", symbolName);
    }
    temporaryDouble = ((int) temporaryDouble)/1; //lose decimal

    return temporaryDouble; //update value
}

//
// create a node for a number
//
AST_NODE *number(double value)
{
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
// create a node for a function and set the linked list of opperands
//
AST_NODE *function(char *funcName, AST_NODE *operandList)
{
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

    //set parent
    while(currentOp != NULL)
    {
        currentOp->parent = p;
        if (currentOp->scope != NULL)
            setScope(currentOp->scope,p);

        currentOp = currentOp->next;

    }

    //append to list
    p->data.function.opList = operandList;

//    if (op1 != NULL) {
//        op1->parent = p;
//        //Check and assign scope from children setScope(child scope, parent)
//        if ((op1->scope) != NULL)
//            setScope((op1->scope),p);
//    }
//    if (op2 != NULL) {
//        op2->parent = p;
//        if ((op2->scope) != NULL)
//            setScope((op2->scope),p);
//    }
    ///////////////////

//    p->data.function.op1 = op1;
//    p->data.function.op2 = op2;



    return p;
}

AST_NODE* sExprList(AST_NODE* headOfList, AST_NODE* tailList)
{
    headOfList->next = tailList;
    return headOfList;
}

//creates a conditional statement
//legs will go to op2 and conditional statement will go to op1
AST_NODE *conditional(AST_NODE *conditionalEvaluation, AST_NODE *trueStatement, AST_NODE *falseStatement)
{
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
    if (conditionalEvaluation != NULL)
    {
        conditionalEvaluation->parent = p;
        if (conditionalEvaluation->scope != NULL)
            setScope(conditionalEvaluation->scope,p);
    }
    if (trueStatement != NULL) {
        trueStatement->parent = p;
        //Check and assign scope from children setScope(child scope, parent)
        if ((trueStatement->scope) != NULL)
            setScope((trueStatement->scope),conditionalEvaluation);
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
//    AST_NODE *currentConditional = p->data.function.opList;
//    //first element is conditional statement, second element is the true block and third is the false block
//    currentConditional = conditionalEvaluation;
//    currentConditional->next = trueStatement;
//    currentConditional->next->next = falseStatement;
//    p->data.function.op1 = conditionalEvaluation;
//    p->data.function.op2 = legs;
//    legs->data.function.op1 = trueStatement;
//    legs->data.function.op2 = falseStatement;
    if(conditionalEvaluation != NULL)
        p->data.function.name = conditionalEvaluation->data.function.name;
    p->type =FUNC_TYPE;



    return p;

}

//
// Evaluate the symbol by checking the parent's scope for its unique value
//
SYMBOL_TABLE_NODE* evaluateSymbol(AST_NODE* symbolNodeToEvaluate)
{

    AST_NODE* currentParent = symbolNodeToEvaluate; //need to go to the highest level parent to find the list of variables
    SYMBOL_TABLE_NODE *currentSymbol = currentParent->scope;
    char* variableToCheckName = symbolNodeToEvaluate->data.symbol.name;
    //traverse upwards to top parent node for the symbol
    do
    {
        currentSymbol = currentParent->scope;
        while (currentSymbol != NULL)
        {
            //check to match symbol
            if (strcmp(currentSymbol->ident, variableToCheckName) == 0) //the case where they are the same
            {
                if ( currentSymbol->val->type != SYMBOL_TYPE )
                {
                    return currentSymbol;//we found the correct symbol with its associated value
                }
                else { //update to new symbol to check for
                    variableToCheckName = (currentSymbol->val->data.symbol.name); //this is the new nested symbol we are looking for
                    currentSymbol = NULL; //force going to a higher parent level
                }
                //if it is a symbol we need to process it more by going to the parent
            } else //view next symbol
                currentSymbol = currentSymbol->next;
        }
        //went through the entire scope and need to go to higher parent level
        currentParent = currentParent->parent;

    }while(currentParent != NULL);

    yyerror("the symbol does not exist in this scope");
    return NULL; //for error checking

}


AST_NODE *makeSymbol(char *symbolName)
{
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

RETURN_VALUE* makeDataType(char *variableTypeAsString)
{
    RETURN_VALUE *p;
    size_t nodeSize;

    nodeSize = sizeof(RETURN_VALUE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    if (variableTypeAsString != NULL && strcmp((variableTypeAsString),"integer") == 0)
    {
        p->type = INTEGER_TYPE;
    }
    else //its a real by default
    {
        p->type = REAL_TYPE;
    }

    return p;
}

void printAnswer(RETURN_VALUE valueToPrint)
{
    if (valueToPrint.type == REAL_TYPE)
    {
        printf("%.2lf\n", valueToPrint.value);
    }
    else
    {
        printf("%.0f", valueToPrint.value);
    }
}



//
// free a node
//
void freeNode(AST_NODE *p)
{
    if (!p)
        return;

    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        freeNode(p->data.function.opList);
    }

    free(p);
}

//
// evaluate an abstract syntax tree
//
// p points to the root
//
RETURN_VALUE eval(AST_NODE *p)
{
    RETURN_VALUE result;
    result.value = 0;
    result.type = REAL_TYPE; //assume real by default
    int operandCount = 0;
    AST_NODE* currentOperand = p->data.function.opList;

    if (!p)
    {
        return result;
    }
    else if(p->type == NUM_TYPE)
    {
        result.value = p->data.number.value;
    }
    else if(p->type == FUNC_TYPE)
    {
        int temp = resolveFunc(p->data.function.name);
        AST_NODE* conditionalStatement = p->data.function.opList;
        AST_NODE* trueStatement = p->data.function.opList;
        AST_NODE* falseStatement = p->data.function.opList;

        switch (temp)
        {
            case NEG:
                result.value = -eval(p->data.function.opList).value;
                break;
            case ABS:
                result.value = fabs(eval(p->data.function.opList).value);
                break;
            case EXP:
                result.value = exp(eval(p->data.function.opList).value);
                break;
            case SQRT:
                result.value = sqrt(eval(p->data.function.opList).value);
                break;
            case ADD:

                //traverse through list and get next operand to calculate
                while(currentOperand != NULL)
                {
                    operandCount++;
                    result.value += eval(currentOperand).value;
                    currentOperand = currentOperand->next;
                }
                if(operandCount < MINIMUM_BINARY_OPERANDS) //need at least two operands for binary operations
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                break;
            case SUB:
                //need a prime read to subtract from first value
                // i.e. 3-8-4 needs to subtract from 3 first
                if (currentOperand != NULL) {
                    result.value = eval(currentOperand).value;
                    operandCount++;
                    currentOperand = currentOperand->next;
                }
                while(currentOperand != NULL)
                {
                    operandCount++;
                    result.value -= eval(currentOperand).value;
                    currentOperand = currentOperand->next;
                }
                if(operandCount < MINIMUM_BINARY_OPERANDS) //need at least two operands for binary operations
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                break;
            case MULT://hardcoding values for these functions. Can come back later to allow for 3*8*7*6
                if (p->data.function.opList->next == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = eval(p->data.function.opList).value * eval(p->data.function.opList->next).value;
                break;
            case DIV:
                if (p->data.function.opList->next == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = eval(p->data.function.opList).value / eval(p->data.function.opList->next).value;
                break;
            case REMAINDER:
                if (p->data.function.opList->next == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = remainder(eval(p->data.function.opList).value, eval(p->data.function.opList->next).value);
                break;
            case LOG:
                if (p->data.function.opList->next == NULL) //if we only have one opperandd when we need two
                {
                    result.value = 0;
                    printf("ERROR: too few parameters for the function\n");
                }
                else
                {
                    AST_NODE* op2 = p->data.function.opList->next; //for readability
                    switch ((int) eval(p->data.function.opList).value) { //it's either log base 2 or log base 10
                        case 2:
                            result.value = log2(eval(op2).value);
                            break;
                        case 10:
                            result.value = log10(eval(op2).value);
                            break;
                        default:

                            result.value = 0;
                            printf("The log base you entered is not supported. Please use base 2 or 10\n");
                    }
                }
                break;
            case POW:
                if (p->data.function.opList->next == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = pow(eval(p->data.function.opList).value, eval(p->data.function.opList->next).value);
                break;
            case MAX:
                if (p->data.function.opList->next == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = fmax(eval(p->data.function.opList).value, eval(p->data.function.opList->next).value);
                break;
            case MIN:
                if (p->data.function.opList->next == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = fmin(eval(p->data.function.opList).value, eval(p->data.function.opList->next).value);
                break;
            case EXP2:
                if (p->data.function.opList == NULL) //change this to a switch statement within another function later
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = exp2(eval(p->data.function.opList).value);
                break;
            case CBRT:
                if (p->data.function.opList == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = cbrt(eval(p->data.function.opList).value);
                break;
            case HYPOT:
                if (p->data.function.opList->next == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else
                    result.value = hypot(eval(p->data.function.opList).value, eval(p->data.function.opList->next).value);
                break;
            case PRINTOP:
                //traverse through list and get next operand to print
                while(currentOperand != NULL)
                {
                    result.value = eval(currentOperand).value;
                    printAnswer(eval(currentOperand));
                    operandCount++;
                    currentOperand = currentOperand->next;
                }
                //returns final printed value
                break;
            case EQUALOP:
                //evaluate conditional statement

                if (conditionalStatement == NULL || trueStatement == NULL || falseStatement == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else {
                    if (eval(conditionalStatement).value ==
                        eval(conditionalStatement->next).value) {
                        eval(trueStatement);
                        result.value = 1;
                    } else {
                        eval(falseStatement);
                        result.value = 0;
                    }
                }
                //produce the correct leg
                break;
            case SMALLER:
                if (conditionalStatement == NULL || trueStatement == NULL || falseStatement == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else {
                    if (eval(conditionalStatement).value <
                        eval(conditionalStatement->next).value) {
                        eval(trueStatement);
                        result.value = 1;
                    } else {
                        eval(falseStatement);
                        result.value = 0;
                    }
                }
                //produce the correct leg
                break;
            case LARGER:
                if (conditionalStatement == NULL || trueStatement == NULL || falseStatement == NULL)
                {
                    printf("ERROR: too few parameters for the function <name>\n");
                    result.value = 0.0;
                }
                else {
                    if (eval(conditionalStatement).value >
                        eval(conditionalStatement->next).value) {
                        eval(trueStatement);
                        result.value = 1;
                    } else {
                        eval(falseStatement);
                        result.value = 0;
                    }
                }
                //produce the correct leg
                break;
            case RAND: //produce a random number
                result.value = (double)rand() + (double)rand()/(double)rand();
                break;
            case READ:
                printf("\n%s = ", p->data.function.opList->data.symbol.name);
                scanf("%lf",&result.value);
                break;
            default:
                exit(-99); //-99 means we did not hit any correct arithmetic functions
        }
    }
    else if(p->type == SYMBOL_TYPE)
    {
        SYMBOL_TABLE_NODE* tableWithAllInformation = evaluateSymbol(p); //&(p->scope);//

        result.value = eval(tableWithAllInformation->val).value;
        result.type = tableWithAllInformation->val_type;
    }

    return result;
}  