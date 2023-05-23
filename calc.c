#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>

#define space ' '
#define X 256

int top;
int stack_arr[X];
char infix[X], postfix[X];

inline void push(int pushed_item);
inline int pop();
inline void show();
inline int prec(int op);
inline void intopost(void);
inline int rpn_eval(void);
inline int start(void);

inline void push(int pushed_item)
{
    if (top > X)
    {
        printf("You get max digit");
        exit(1);
    }
    else
    {
        top = top + 1;
        stack_arr[top] = pushed_item;
    }
}
inline int pop()
{
    if (top == -1)
    {
        printf("stack underflow");
        exit(2);
    }
    else
    {
        //top = top - 1;
        return stack_arr[top--];
    }
}
inline void show()
{
    int i;
    if (top == -1)
    {
        printf("stack is empty\n");
    }
    else
    {
        for (i = top; i >= 0; i--)
        {
            printf("%d\n", stack_arr[i]);
        }
    }
}
inline int prec(int op)
{
    if (op == '(')
    {
        return 0;
    }
    else if (op == '+' || op == '-')
    {
        return 1;
    }
    else if (op == '*' || op == '/' || op == '%')
    {
        return 2;
    }
    else if (op == '^')
    {
        return 3;
    }
    else
    {
        return 0;
    }
}
inline void intopost()
{
    int i;
    int p = 0;
    //int type = 0;
    int precedence;
    char next;

    stack_arr[top] = '#';
    int len = strlen(infix);
    infix[len] = '#';

    for (i = 0; infix[i] != '#'; i++)
    {
        if ((infix[i] != space) && (infix[i] != '\t') && (infix[i] != '\0'))
        {
            if (infix[i] == '(')
            {
                push(infix[i]);
            }
            else if (infix[i] == ')')
            {
                while ((next = pop()) != '(')
                {
                    postfix[p++] = next;
                }
            }
            else if (infix[i] == '+' || infix[i] == '-' || infix[i] == '*' || infix[i] == '/' || infix[i] == '%' || infix[i] == '^')
            {
                precedence = prec(infix[i]);
                while (stack_arr[top] != '#' && precedence <= prec(stack_arr[top]))
                {
                    postfix[p++] = pop();
                }
                push(infix[i]);
            }
            else
            {
                postfix[p++] = infix[i];
            }
        }
    }
    while (stack_arr[top] != '#')
    {
        postfix[p++] = pop();
    }
    postfix[p] = '\0';
}
//std::string expr
inline int rpn_eval()
{
    long int a;
    long int b;
    long int temp;
    int result;
    long int len;
    int i;
    len = strlen(postfix);
    postfix[len] = '#';
    for (i = 0;postfix[i] != '#';i++)
    {
        if (postfix[i] <= '9' && postfix[i] >= '0')
        {
            push(postfix[i] - 48);
        }
        else
        {
            a = pop();
            b = pop();
            if (postfix[i] == '+')
            {
                temp = b + a;
            }
            else if (postfix[i] == '-')
            {
                temp = b - a;
            }
            else if (postfix[i] == '*')
            {
                temp = b * a;
            }
            else if (postfix[i] == '/')
            {
                temp = b / a;
            }
            else if (postfix[i] == '%')
            {
                temp = b % a;
            }
            else if (postfix[i] == '^')
            {
                temp = pow(b, a);
            }
            push(temp);
        }
    }
    result = pop();
    return result;
}
inline int start()
{
    long int val;
    top = 0;
    fflush(stdin);
    gets(infix);
    intopost();
    sprintf("Postfix is : %s\n", postfix);
    val = rpn_eval();
    sprintf("%d",val);
    return 0;
}
