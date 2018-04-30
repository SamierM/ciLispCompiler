%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct return_value* returnNode;
    struct ast_node* astNode;
    struct symbol_table_node* symbolNode;
    struct symbol_table_node* scopeNode;
};

%token <sval> FUNC
%token <sval> SYMBOL
%token <dval> NUMBER
%token <sval> INTEGER
%token <sval> REAL
%token LPAREN RPAREN EOL QUIT LET CONDITIONAL LAMBDA

%type <astNode> s_expr
%type <astNode> s_expr_list
%type <symbolNode> let_elem
%type <symbolNode> let_list
%type <symbolNode> arg_list
%type <symbolNode> scope
%type <returnNode> type

%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printf("\nyacc: %lf\n",(eval($1)).value);
            freeNode($1);
        }
    };

s_expr:
    QUIT {
        fprintf(stderr, "QUIT\n");
        exit(0);
    }
    | NUMBER {
        fprintf(stderr, "yacc: NUMBER %lf", $1);
        $$ = number($1);
    }
    | SYMBOL {
        fprintf(stderr, "yacc: SYMBOL\n");
        $$ = makeSymbol($1);

    }
    | LPAREN FUNC s_expr_list RPAREN {
        fprintf(stderr, "yacc: LPAREN FUNC s_expr_list RPAREN\n");
        $$ = function($2, $3);
    }
    | LPAREN scope s_expr RPAREN
    {
        fprintf(stderr, "LPAREN scope s_expr RPAREN\n");
        $$ = setScope($2,$3);
    }
    | LPAREN CONDITIONAL s_expr s_expr s_expr RPAREN
    {
        fprintf(stderr, "LPAREN CONDITIONAL s_expr s_expr s_expr RPAREN\n");
        $$ = conditional($3,$4,$5);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

s_expr_list:
    s_expr
    {
        fprintf(stderr, "s_expr\n");
        $$ = $1;
    }
    | s_expr s_expr_list
    {
        fprintf(stderr, "s_expr s_expr_list\n");
        $$ = sExprList($1, $2);
    };

scope:
    //empty
    {
        $$ = NULL;
    }

    | LPAREN LET let_list RPAREN
    {
        fprintf(stderr, "LPAREN LET let_list RPAREN\n");
        $$ = $3; //Just to return the list to the higher level. May be unnecessary, but useful currently to visualize structure
    };

let_list:
    let_elem {
        fprintf(stderr,"let_elem\n");
        $$ = $1;
    }

    | let_list let_elem
    {
        fprintf(stderr, "let_list let_elem\n");
        $$ = createSymbolList($1, $2);
    };

let_elem:
    LPAREN type SYMBOL s_expr RPAREN
    {
        fprintf(stderr, "LPAREN SYMBOL s_expr RPAREN\n");
        $$ = let_elem($2, $3, $4, NULL);
    }

    | LPAREN type SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN //( type symbol lambda ( arg_list ) s_expr );
    {
        fprintf(stderr, "LPAREN type SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN\n");
        $$ = createUserFunction($2, $3, $6, $8);
    };

arg_list:
    SYMBOL arg_list
    {
        fprintf(stderr, "yacc: SYMBOL arg_list\n");
        $$ = createArgumentList($1,$2);
    }

    | SYMBOL
    {
        fprintf(stderr, "yacc: SYMBOL --- for an arg_list\n");
        $$ = createArgumentNode($1);
    };

type:
    INTEGER
    {
        fprintf(stderr, "INTEGER\n");
        $$ = makeDataType($1);
    }
    | REAL
    {
        fprintf(stderr, "REAL\n");
        $$ = makeDataType($1);
    }
    |//empty
    {
        $$ = makeDataType(NULL);
    };

%%

