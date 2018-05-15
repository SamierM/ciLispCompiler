This program is a compiler that has the expected result below.
It takes input in Polish notation and returns a result based on the function.
The end of this readme contains some test cases.

It has the following grammar utlizing C, lisp, Bison:

program ::= s-expr EOL

s-expr ::= 
     quit 
   | number 
   | symbol 
   | ( func s_expr_list )
   | ( scope s_expr )
   | ( cond s_expr s_expr s_expr )

func ::=
    neg|abs|exp|sqrt|add|sub|mult|div|remainder|log|pow|max|min|exp2
    |cbrt|hypot|rand|read|print|equal|smaller|larger

scope ::= <empty> | ( let let_list )

let_list ::= let_elem | let_list let_elem 

let_elem ::= 
   ( type symbol s_expr )
   | ( type symbol lambda ( arg_list ) s_expr )

type ::= <empty> | integer | real

arg_list ::= symbol arg_list | symbol

number ::= digit+ [ . digit+ ] 
symbol ::= letter+ 

letter ::= [a-zA-Z]
digit ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

-----------------------------------------------------------------------------------
//test cases
//format: 
//       input // expected output

(add 1 2 3 4) //10
(sub 4 3) // 1
(sub 4 3 11) //-10
(div 27 3 3) //3
(mult 3 3 3) //27
(remainder 100 51 6) //49
(log 2 8) // 3
(log 2 8 5) // 3
(pow 3 2) //9
(max 2 3) //3
(min 3 2) // 2
(print 3 4 5 6 7) //print sequentially on seperate lines and returns the final print number which is 7
(rand) //produces an unrestricted pseudorandom number 
(cond (smaller 3 4) (print 9 10) (print -9 -10)) //prints 9 and 10 and returns result of conditional expression
(add ((let (abcd 1)) (sub 3 abcd)) 4) //6
(mult ((let (a 1) (b 2)) (add a b)) (sqrt 2)) //4.24642
(add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c))) //5.398345
((let (first (sub 5 1)) (second 2)) (add (pow 2 first) (sqrt second))) 
((let (real a ((let (real c 3) (integer d 4)) (mult c d)))) (sqrt a))


//USER DEFINED FUNcTIONS
((let (fard lambda (x y) (add x y))) (fard 1 2)) //3
((let (a 1) (f lambda (x y) (add x y)))(f 1 (f 2 a))) // 4
((let (f lambda (x y) (add x y))) (f (sub 5 2) (mult 2 3))) //9
	//Recursive User Defined
	((let (f lambda (x y) (add x y))) (f (f 5 2 2) (mult 2 3))) //13.000000
	((let (integer f lambda (x y) (add x y))) (f (f 5 2 2) (mult 2 3))) //13


//Error/Warning Producing statements
	//Insufficient number of inputs
	(add 1)
	(sub 4)
	(div 27)
	(mult 3)
		//Produces: ERROR: too few parameters for the function mult
		//returns 0.00000
	(pow 3)
	ERROR: too few parameters for the function pow
	0.00



