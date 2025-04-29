#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/errno.h>

#include "formula.h"

extern calc_variable *vars[];
extern int var_index;

char *get_var_value(char *name) {
    for (int i = 0; i < var_index; i++) {
        if (strcmp(name, vars[i]->name) == 0) {
            return vars[i]->value;
        }
    }
    return NULL;
}

int get_var_value_len(char *name) {
    for (int i = 0; i < var_index; i++) {
        if (strcmp(name, vars[i]->name) == 0) {
            return vars[i]->value_len;
        }
    }
    return 0;
}

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
            char *str_end;
            double converted = strtod(found, &str_end);
            if (str_end == found) { /* Conversion was unsuccessful, so it's a variable */
                /* variable */
                char *val = get_var_value(found);
                if (val != NULL) {
                    stack[stacktop] = calculate(val, get_var_value_len(found));
                    stacktop++;
                }
            }
            else {
                /* number */
                stack[stacktop] = converted;
                stacktop++;
            }
        }
    }

    return stack[stacktop - 1];

}

/**
 * Defines a variable with a name and an expression  
 */
calc_variable *define_var(char *name, int name_len, char *value, int value_len) {
    calc_variable *var;
    var = malloc(sizeof(calc_variable));

    memcpy(var->name, name, MAXNAMELEN);
    memcpy(var->value, value, MAXEXPRESSIONSIZE);
    var->name_len = name_len;
    var->value_len = value_len;

    return var;
}

/**
 * Parses a variable definition into a calc_variable
 */
calc_variable *parse_var(char *in, int len) {
    char name[MAXNAMELEN];
    char value[MAXEXPRESSIONSIZE];

    int next_letter = 0;

    /* Get variable name */
    int i;
    for (i = 0; i < len && i < MAXNAMELEN && in[i] != '='; i++) {
        if (in[i] != ' ') { /* Skip spaces */
            name[next_letter] = in[i];
            next_letter++;
        }
    }
    name[next_letter] = '\0';
    int name_len = next_letter;
    /* validate that variable name is correct and '=' was found */
    if (in[i] != '=') {
        return NULL;
    }

    i++; /* Skip the '=' */

    next_letter = 0;

    /* Get expression */
    for(int j = 0; i + j < len && j < MAXEXPRESSIONSIZE; j++) {
        if (in[i + j] != ' ') { /* Skip spaces */
            value[next_letter] = in[i + j];
            next_letter++;
        }
    }
    int value_len = next_letter;

    return define_var(name, name_len, value, value_len);
}
