# DD final project


## Introduction:
The project is Reverse Polish Notation Calculator (RPN), a calculator that uses to calculate the mathematical equations that supports arithmetic expression by using the data structure stack and infix and postfix methods.
## RPN:
RPN is Reverse Polish Notation, in some mathematical expression we can face many complicated like scanning direction and operator priority of parenthesis.
priority rule of arithmetic:
- level 1: parenthesis ‘()’.
-	level 2: exponentiation ‘^’.
-	Level 3: Multiplication ‘*’ and division ‘/’.
-	level 4: Addition ‘+’ subtract ‘-‘.

There is three ways to represent the arithmetic expression infix, postfix, and prefix.
- Infix: used the operator between the operands 
    ex: A + B.
- Prefix: used the operator before the operand 
    ex: + A B.
- postfix: used the operator after the operand 
    ex: A B +.

we need to convert infix to postfix by scanning from left to right, that why we need to use stack for temporary placement of operator then the result of postfix expression converting without using the separator such as parenthesis.

## software:
The software is built in two files one is the main and the other calculation both are c language.
-	Step 0: implement the max number is 256, infix, postfix, and stack array.
        Converting infix to postfix:
	Step 1: add the symbol # into the top of stack and the end of the infix array.
-	Step 2: input the infix array from left to right.
-	Step 3: if we have left parenthesis ‘(’, add it to infix.
-	Step 4: if we have right parenthesis ‘)’, pop all operator from stack until left parenthesis.
-	Step 5: if the symbol is operator +, -,*,/,%,^ then pop all operator have the same or hight percentage and add pop operator to  array postfix and add input symbol to stack.
-	Step 6: if the symbol is # pop all symbols from stack and add to postfix array.
-	Step 7: append \0 to postfix array to make a string and continue looping.

- Evaluation of RPN:
Read the postfix array from left to right, if the symbol is operator, we execute the operator’s equation and assign to local variable temp then pop the element of stack to be the result of evaluation.
## hardware:
We used a Digital Design Board that is connected to a OLED screen, we connected three buttons, first to go right and second to go left, and third to calculate.
So you use two buttons for the input the expression and third one press to calculate the equation and displays the output on the OLED screen .

