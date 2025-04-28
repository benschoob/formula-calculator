#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "formula.h"

/*
Comparator for arguments; Operators with higher precedence beat ones with lower precedence, and 
left-associative operators are beaten by non-left-associative ones

Returns: 1 if op1 has higher precedence than op2
         0 if op1 == op2 or one or more of the characters aren't operators
         -1 if op1 has lower precedence than op2
*/
int compare_ops(char op1, char op2) {
    if (op1 == op2) return 0;
    switch (op1)
    {
    case '+':
        if (op2 == '+') return 0;
        else if (op2 == '-') return 1;
        else if (op2 == '*') return -1;
        else if (op2 == '/') return -1;
        else if (op2 == '^') return -1;
        break;

    case '-':
        if (op2 == '+') return -1;
        else if (op2 == '-') return 0;
        else if (op2 == '*') return -1;
        else if (op2 == '/') return -1;
        else if (op2 == '^') return -1;
        break;

    case '*':
        if (op2 == '+') return 1;
        else if (op2 == '-') return 1;
        else if (op2 == '*') return 0;
        else if (op2 == '/') return 1;
        else if (op2 == '^') return -1;
        break;

    case '/':
        if (op2 == '+') return 1;
        else if (op2 == '-') return 1;
        else if (op2 == '*') return -1;
        else if (op2 == '/') return 0;
        else if (op2 == '^') return -1;
        break;

    case '^':
        if (op2 == '+') return 1;
        else if (op2 == '-') return 1;
        else if (op2 == '*') return 1;
        else if (op2 == '/') return 1;
        else if (op2 == '^') return 0;
        break;
    }
    return 0;
}

/* 
Implements the shunting yard algorithm (https://en.wikipedia.org/wiki/Shunting_yard_algorithm). 
Converts a string containing a formula in infix notation to outfix (reverse polish) notation.

Return Values:
0 - Success
1 - Incomplete parenteses error
*/
int infix_parse(char *in, char *out, int len) {
    char opstack[len]; /* Operator stack */
    int stacktop = 0;
    int outtop = 0;
    int i;

    for (i=0; i<len && (unsigned long)i < strlen(in); i++) {
        if (in[i] == ' ' || in[i] == '\n') continue;; /* Skip whitespace */
        /* Operators */
        if (in[i] == '+' || in[i] == '-' || in[i] == '*' || in[i] == '/' || in[i] == '^') {
            /* Add a space between numbers */
            out[outtop] = ' ';
            outtop++;

            /* Add operators with greater precedence to the output */
            while (stacktop > 0 && opstack[stacktop - 1] != '(' && compare_ops(in[i], opstack[stacktop - 1]) < 0) {
                stacktop--;
                out[outtop] = opstack[stacktop];
                outtop++;

                out[outtop] = ' ';
                outtop++;
            }
            opstack[stacktop] = in[i];
            stacktop++;
        } 
        /* Left Parentesis */
        else if (in[i] == '(') {
            /* Push it onto the stack */
            opstack[stacktop] = in[i];
            stacktop++;
        }
        /* Right Parentesis */
        else if (in[i] == ')') {
            while (stacktop > 0 && opstack[stacktop - 1] != '(') {
                /* Pop an operator off the stack */
                out[outtop] = ' ';
                outtop++;

                stacktop--;
                out[outtop] = opstack[stacktop];
                outtop++;
            }
            if (stacktop == 0) return 1; /* Parenteses mismatch */
            stacktop--; /* Pop the left parentesis off the stack */
        }
        /* Numbers/variables */
        else {
            /* Put it on the output queue */
            out[outtop] = in[i];
            outtop++;
        }
    }
    /* Put all operators still on the stack into the output */
    while (stacktop > 0) {
        out[outtop] = ' ';
        outtop++;

        stacktop--;
        out[outtop] = opstack[stacktop];
        outtop++;
    }
    out[outtop] = '\0';

    return 0;
}

/*
Calculates the result of a formula in postfix form
*/
double calculate(char *f, int f_len) {
    double stack[50];
    int stacktop = 0;

    char *f_cpy = malloc(f_len);
    char *found;

    double o1, o2;

    memcpy(f_cpy, f, f_len);
    while ((found = strsep(&f_cpy, " ")) != NULL) {
        if (strcmp(found, "+") == 0) {
            stacktop--;
            o2 = stack[stacktop];
            o1 = stack[stacktop - 1];
            stack[stacktop - 1] = o1 + o2;
        }
        else if (strcmp(found, "-") == 0) {
            stacktop--;
            o2 = stack[stacktop];
            o1 = stack[stacktop - 1];
            stack[stacktop - 1] = o1 - o2;
        }
        else if (strcmp(found, "*") == 0) {
            stacktop--;
            o2 = stack[stacktop];
            o1 = stack[stacktop - 1];
            stack[stacktop - 1] = o1 * o2;
        }
        else if (strcmp(found, "/") == 0) {
            stacktop--;
            o2 = stack[stacktop];
            o1 = stack[stacktop - 1];
            stack[stacktop - 1] = o1 / o2;
        }
        else if (strcmp(found, "^") == 0) {
            stacktop--;
            o2 = stack[stacktop];
            o1 = stack[stacktop - 1];
            stack[stacktop - 1] = pow(o1, o2);
        }
        else {
            stack[stacktop] = strtod(found, NULL);
            stacktop++;
        }
    }

    return stack[stacktop - 1];

}

calc_variable define_var(char *name, double value) {
    calc_variable var;

    memcpy(var.name, name, MAXNAMELEN);
    var.value = value;

    return var;
}
